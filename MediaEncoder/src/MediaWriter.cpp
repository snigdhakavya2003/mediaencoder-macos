cpp
#include "MediaWriter.h"
#include <stdexcept>
#include <string>
#include <memory>
#include <vector>
#include <ffmpeg/avformat.h>
#include <ffmpeg/avcodec.h>
#include <ffmpeg/swscale.h>
#include <ffmpeg/swresample.h>

struct WriterPrivateData {
    AVFormatContext* FormatContext = nullptr;
    AVCodecContext* VideoCodecContext = nullptr;
    AVCodecContext* AudioCodecContext = nullptr;
    const AVCodec* VideoCodec = nullptr;
    const AVCodec* AudioCodec = nullptr;
    AVStream* VideoStream = nullptr;
    AVStream* AudioStream = nullptr;
    AVFrame* VideoFrame = nullptr;
    AVFrame* AudioFrame = nullptr;
    AVFrame* SoftwareVideoFrame = nullptr;
    int64_t NextVideoPts = 0;
    int64_t NextAudioPts = 0;
    int AudioSamplesCount = 0;
    SwsContext* SwsContext = nullptr;
    SwrContext* SwrContext = nullptr;
    int SwsSrcWidth = 0, SwsSrcHeight = 0;
    AVPixelFormat SwsSrcFormat = AV_PIX_FMT_NONE;
    AVBufferRef* HardwareDeviceContext = nullptr;

    ~WriterPrivateData() {
        if (VideoCodecContext) avcodec_free_context(&VideoCodecContext);
        if (AudioCodecContext) avcodec_free_context(&AudioCodecContext);
        if (FormatContext) avformat_free_context(FormatContext);
        if (VideoFrame) av_frame_free(&VideoFrame);
        if (AudioFrame) av_frame_free(&AudioFrame);
        if (SoftwareVideoFrame) av_frame_free(&SoftwareVideoFrame);
        if (SwsContext) sws_freeContext(SwsContext);
        if (SwrContext) swr_free(&SwrContext);
        if (HardwareDeviceContext) av_buffer_unref(&HardwareDeviceContext);
    }
};

static int write_frame(AVFormatContext* fmt_ctx, AVCodecContext* c, AVStream* st, AVFrame* frame) {
    int ret = avcodec_send_frame(c, frame);
    if (ret < 0) {
        throw std::runtime_error("avcodec_send_frame failed");
    }

    while (ret >= 0) {
        AVPacket pkt;
        av_init_packet(&pkt);
        pkt.data = nullptr;
        pkt.size = 0;

        ret = avcodec_receive_packet(c, &pkt);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) break;
        if (ret < 0) {
            throw std::runtime_error("avcodec_receive_packet failed");
        }

        if (pkt.duration == 0 && frame == nullptr) pkt.duration = 1;

        av_packet_rescale_ts(&pkt, c->time_base, st->time_base);
        pkt.stream_index = st->index;

        ret = av_interleaved_write_frame(fmt_ctx, &pkt);
        av_packet_unref(&pkt);
        if (ret < 0) {
            throw std::runtime_error("av_interleaved_write_frame failed");
        }
    }

    return ret == AVERROR_EOF ? 1 : 0;
}

static int set_hwframe_ctx(AVCodecContext* ctx, AVBufferRef* hw_device_ctx, int width, int height, AVPixelFormat hw_format) {
    AVBufferRef* hw_frames_ref = av_hwframe_ctx_alloc(hw_device_ctx);
    if (!hw_frames_ref) {
        fprintf(stderr, "Failed av_hwframe_ctx_alloc.\n");
        return -1;
    }

    AVHWFramesContext* frames_ctx = (AVHWFramesContext*)(hw_frames_ref->data);
    frames_ctx->format = hw_format;
    frames_ctx->sw_format = AV_PIX_FMT_NV12;
    frames_ctx->width = width;
    frames_ctx->height = height;
    if (hw_format == AV_PIX_FMT_QSV) {
        frames_ctx->initial_pool_size = 32;
    }

    int err = av_hwframe_ctx_init(hw_frames_ref);
    if (err < 0) {
        av_buffer_unref(&hw_frames_ref);
        return err;
    }

    ctx->hw_frames_ctx = av_buffer_ref(hw_frames_ref);
    if (!ctx->hw_frames_ctx) err = AVERROR(ENOMEM);
    av_buffer_unref(&hw_frames_ref);
    return err;
}

MediaWriter::MediaWriter(int width, int height, int video_numerator, int video_denominator,
                         VideoCodec video_codec, int video_bitrate,
                         AudioCodec audio_codec, int audio_bitrate)
    : m_width(width), m_height(height), m_videoNumerator(video_numerator), m_videoDenominator(video_denominator),
      m_videoBitrate(video_bitrate), m_videoCodec(static_cast<AVCodecID>(video_codec)),
      m_audioBitrate(audio_bitrate), m_audioCodec(static_cast<AVCodecID>(audio_codec)), m_data(std::make_unique<WriterPrivateData>()),
      m_disposed(false) {
    avformat_network_init();
}

void MediaWriter::Open(const std::string& url, const std::string& format, bool forceSoftwareEncoder) {
    CheckIfDisposed();
    Close();

    if (url.empty()) throw std::runtime_error("url cannot be empty");

    m_data->VideoFramesCount = 0;
    m_data->AudioSamplesCount = 0;

    AVFormatContext* formatContext = nullptr;
    if (avformat_alloc_output_context2(&formatContext, nullptr, format.c_str(), url.c_str()) < 0) {
        throw std::runtime_error("Cannot open the file");
    }
    m_data->FormatContext = formatContext;

    // Create Video Codec
    if (m_videoCodec != AV_CODEC_ID_NONE) {
        AVCodecContext* videoCodecContext = avcodec_alloc_context3(nullptr);
        videoCodecContext->width = m_width;
        videoCodecContext->height = m_height;
        videoCodecContext->time_base = av_make_q(m_videoDenominator, m_videoNumerator);
        videoCodecContext->framerate = av_make_q(m_videoNumerator, m_videoDenominator);
        videoCodecContext->bit_rate = m_videoBitrate > 0 ? m_videoBitrate : 10000000;

        const AVCodec* videoCodec = avcodec_find_encoder(m_videoCodec);
        if (!videoCodec) throw std::runtime_error("Cannot find video codec");

        if (avcodec_open2(videoCodecContext, videoCodec, nullptr) < 0) {
            throw std::runtime_error("Cannot open video codec");
        }

        AVStream* videoStream = avformat_new_stream(m_data->FormatContext, videoCodec);
        avcodec_parameters_from_context(videoStream->codecpar, videoCodecContext);
        videoStream->time_base = videoCodecContext->time_base;

        m_data->VideoCodecContext = videoCodecContext;
        m_data->VideoCodec = videoCodec;
        m_data->VideoStream = videoStream;
    }

    // Create Audio Codec
    if (m_audioCodec != AV_CODEC_ID_NONE) {
        AVCodecContext* audioCodecContext = avcodec_alloc_context3(nullptr);
        const AVCodec* audioCodec = avcodec_find_encoder(m_audioCodec);
        if (!audioCodec) throw std::runtime_error("Cannot find audio codec");

        audioCodecContext->sample_rate = m_audioBitrate > 0 ? m_audioBitrate : 128000;
        audioCodecContext->channels = 2; // Stereo
        audioCodecContext->channel_layout = AV_CH_LAYOUT_STEREO;

        if (avcodec_open2(audioCodecContext, audioCodec, nullptr) < 0) {
            throw std::runtime_error("Cannot open audio codec");
        }

        AVStream* audioStream = avformat_new_stream(m_data->FormatContext, audioCodec);
        avcodec_parameters_from_context(audioStream->codecpar, audioCodecContext);
        audioStream->time_base = av_make_q(1, audioCodecContext->sample_rate);

        m_data->AudioCodecContext = audioCodecContext;
        m_data->AudioCodec = audioCodec;
        m_data->AudioStream = audioStream;
    }

    if (!(m_data->FormatContext->oformat->flags & AVFMT_NOFILE)) {
        if (avio_open(&m_data->FormatContext->pb, url.c_str(), AVIO_FLAG_WRITE) < 0) {
            throw std::runtime_error("avio_open error");
        }
    }

    if (avformat_write_header(m_data->FormatContext, nullptr) < 0) {
        throw std::runtime_error("avformat_write_header error");
    }

    if (m_data->VideoCodecContext) {
        m_data->VideoFrame = av_frame_alloc();
        m_data->VideoFrame->width = m_width;
        m_data->VideoFrame->height = m_height;
        m_data->VideoFrame->format = m_data->VideoCodecContext->pix_fmt;
        if (av_frame_get_buffer(m_data->VideoFrame, 32) < 0) {
            throw std::runtime_error("av_frame_get_buffer error (VideoFrame)");
        }
    }

    if (m_data->AudioCodecContext) {
        m_data->AudioFrame = av_frame_alloc();
        m_data->AudioFrame->format = m_data->AudioCodecContext->sample_fmt;
        m_data->AudioFrame->channel_layout = m_data->AudioCodecContext->channel_layout;
        m_data->AudioFrame->sample_rate = m_data->AudioCodecContext->sample_rate;
        m_data->AudioFrame->nb_samples = m_data->AudioCodecContext->frame_size;
        av_frame_get_buffer(m_data->AudioFrame, 0);
    }
}

void MediaWriter::Close() {
    if (!m_data) return;

    if (m_data->VideoCodecContext) write_frame(m_data->FormatContext, m_data->VideoCodecContext, m_data->VideoStream, nullptr);
    if (m_data->AudioCodecContext) write_frame(m_data->FormatContext, m_data->AudioCodecContext, m_data->AudioStream, nullptr);
    av_write_trailer(m_data->FormatContext);
    avformat_close_input(&m_data->FormatContext);
}

void MediaWriter::EncodeVideoFrame(AVFrame* videoFrame) {
    if (!m_data || !videoFrame || !m_data->VideoCodecContext) return;

    // Copy or scale the video frame as needed
    av_frame_copy(m_data->VideoFrame, videoFrame);
    m_data->VideoFrame->pts = m_data->NextVideoPts++;
    write_frame(m_data->FormatContext, m_data->VideoCodecContext, m_data->VideoStream, m_data->VideoFrame);
}

void MediaWriter::EncodeAudioFrame(AVFrame* audioFrame) {
    if (!m_data || !audioFrame || !m_data->AudioCodecContext) return;

    m_data->AudioFrame->pts = m_data->NextAudioPts;
    m_data->NextAudioPts += m_data->AudioFrame->nb_samples;
    write_frame(m_data->FormatContext, m_data->AudioCodecContext, m_data->AudioStream, m_data->AudioFrame);
}