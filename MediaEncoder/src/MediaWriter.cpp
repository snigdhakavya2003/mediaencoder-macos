#include "MediaWriter.h"
#include "VideoFrame.h"
#include "AudioFrame.h"

#include <stdexcept>
#include <string>
#include <memory>
#include <vector>

extern "C" {
    #include <libavformat/avformat.h>
    #include <libavcodec/avcodec.h>
    #include <libswscale/swscale.h>
    #include <libswresample/swresample.h>
    #include <libavutil/opt.h>
    #include <libavutil/imgutils.h>
    #include <libavutil/channel_layout.h>
}

namespace MediaEncoder {
    struct WriterPrivateData {
    AVFormatContext* formatCtx = nullptr;
    AVCodecContext* videoCtx = nullptr;
    AVCodecContext* audioCtx = nullptr;
    AVStream* videoStream = nullptr;
    AVStream* audioStream = nullptr;
    AVFrame* videoFrame = nullptr;
    AVFrame* audioFrame = nullptr;
    int64_t videoPts = 0;
    int64_t audioPts = 0;

    ~WriterPrivateData() {
        if (videoCtx) avcodec_free_context(&videoCtx);
        if (audioCtx) avcodec_free_context(&audioCtx);
        if (formatCtx) {
            if (!(formatCtx->oformat->flags & AVFMT_NOFILE)) {
                avio_closep(&formatCtx->pb);
            }
            avformat_free_context(formatCtx);
        }
        if (videoFrame) av_frame_free(&videoFrame);
        if (audioFrame) av_frame_free(&audioFrame);
    }
    };

// Helper for writing frames
static int WriteFrame(AVFormatContext* fmtCtx, AVCodecContext* codecCtx, AVStream* stream, AVFrame* frame) {
    int ret = avcodec_send_frame(codecCtx, frame);
    if (ret < 0) throw std::runtime_error("avcodec_send_frame failed");

    while (ret >= 0) {
        AVPacket pkt;
        av_new_packet(&pkt, 0);
        pkt.data = nullptr;
        pkt.size = 0;

        ret = avcodec_receive_packet(codecCtx, &pkt);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) break;
        if (ret < 0) throw std::runtime_error("avcodec_receive_packet failed");

        av_packet_rescale_ts(&pkt, codecCtx->time_base, stream->time_base);
        pkt.stream_index = stream->index;

        if (av_interleaved_write_frame(fmtCtx, &pkt) < 0)
            throw std::runtime_error("av_interleaved_write_frame failed");

        av_packet_unref(&pkt);
    }

    return 0;
}

// Constructor
MediaWriter::MediaWriter(int width, int height, int videoNum, int videoDen,
                         const std::string& videoCodec, int videoBitrate,
                         const std::string& audioCodec, int audioBitrate)
    : m_width(width), m_height(height),
      m_videoNumerator(videoNum), m_videoDenominator(videoDen),
      m_videoCodecName(videoCodec), m_videoBitrate(videoBitrate),
      m_audioCodecName(audioCodec), m_audioBitrate(audioBitrate),
      m_disposed(false)
{
    m_data = std::make_unique<WriterPrivateData>();
}

// Open method
void MediaWriter::Open(const std::string& url, const std::string& format) {
    m_url = url;
    m_format = format;

    avformat_alloc_output_context2(&m_data->formatCtx, nullptr, format.c_str(), url.c_str());
    if (!m_data->formatCtx) throw std::runtime_error("Failed to allocate output context");

    // Video
    if (!m_videoCodecName.empty()) {
        const AVCodec* codec = avcodec_find_encoder_by_name(m_videoCodecName.c_str());
        if (!codec) throw std::runtime_error("Video codec not found");

        m_data->videoStream = avformat_new_stream(m_data->formatCtx, nullptr);
        m_data->videoCtx = avcodec_alloc_context3(codec);
        AVCodecContext* ctx = m_data->videoCtx;

        ctx->codec_id = codec->id;
        ctx->width = m_width;
        ctx->height = m_height;
        ctx->time_base = {m_videoDenominator, m_videoNumerator};
        ctx->framerate = {m_videoNumerator, m_videoDenominator};
        ctx->pix_fmt = AV_PIX_FMT_YUV420P;
        ctx->bit_rate = m_videoBitrate;

        if (m_data->formatCtx->oformat->flags & AVFMT_GLOBALHEADER)
            ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

        if (avcodec_open2(ctx, codec, nullptr) < 0)
            throw std::runtime_error("Failed to open video codec");

        avcodec_parameters_from_context(m_data->videoStream->codecpar, ctx);
        m_data->videoStream->time_base = ctx->time_base;

        m_data->videoFrame = av_frame_alloc();
        m_data->videoFrame->format = ctx->pix_fmt;
        m_data->videoFrame->width = ctx->width;
        m_data->videoFrame->height = ctx->height;
        av_frame_get_buffer(m_data->videoFrame, 32);
    }

    // Audio
    if (!m_audioCodecName.empty()) {
        const AVCodec* codec = avcodec_find_encoder_by_name(m_audioCodecName.c_str());
        if (!codec) throw std::runtime_error("Audio codec not found");

        m_data->audioStream = avformat_new_stream(m_data->formatCtx, nullptr);
        m_data->audioCtx = avcodec_alloc_context3(codec);
        AVCodecContext* ctx = m_data->audioCtx;

        ctx->codec_id = codec->id;
        if (av_channel_layout_copy(&m_data->audioFrame->ch_layout, &ctx->ch_layout) < 0) {
            throw std::runtime_error("Failed to copy channel layout to frame");
        }
        ctx->bit_rate = m_audioBitrate;
        ctx->time_base = {1, ctx->sample_rate};

        if (m_data->formatCtx->oformat->flags & AVFMT_GLOBALHEADER)
            ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

        if (avcodec_open2(ctx, codec, nullptr) < 0)
            throw std::runtime_error("Failed to open audio codec");

        avcodec_parameters_from_context(m_data->audioStream->codecpar, ctx);
        m_data->audioStream->time_base = ctx->time_base;

        m_data->audioFrame = av_frame_alloc();
        m_data->audioFrame->format = ctx->sample_fmt;
        m_data->audioFrame->sample_rate = ctx->sample_rate;
        if (av_channel_layout_copy(&m_data->audioFrame->ch_layout, &ctx->ch_layout) < 0) {
            throw std::runtime_error("Failed to copy channel layout");
        }
        m_data->audioFrame->nb_samples = ctx->frame_size;
        av_frame_get_buffer(m_data->audioFrame, 0);
    }

    // File open
    if (!(m_data->formatCtx->oformat->flags & AVFMT_NOFILE)) {
        if (avio_open(&m_data->formatCtx->pb, url.c_str(), AVIO_FLAG_WRITE) < 0)
            throw std::runtime_error("Failed to open output file");
    }

    if (avformat_write_header(m_data->formatCtx, nullptr) < 0)
        throw std::runtime_error("Failed to write header");
}

// Encoding
void MediaWriter::EncodeVideoFrame(VideoFrame* frame) {
    if (!frame) return;
    AVFrame* src = frame->NativePointer();
    src->pts = m_data->videoPts++;
    WriteFrame(m_data->formatCtx, m_data->videoCtx, m_data->videoStream, src);
}

void MediaWriter::EncodeAudioFrame(AudioFrame* frame) {
    if (!frame) return;
    AVFrame* src = frame->NativePointer();
    src->pts = m_data->audioPts;
    m_data->audioPts += src->nb_samples;
    WriteFrame(m_data->formatCtx, m_data->audioCtx, m_data->audioStream, src);
}

// Cleanup
void MediaWriter::Close() {
    if (!m_data || !m_data->formatCtx) return;

    if (m_data->videoCtx) WriteFrame(m_data->formatCtx, m_data->videoCtx, m_data->videoStream, nullptr);
    if (m_data->audioCtx) WriteFrame(m_data->formatCtx, m_data->audioCtx, m_data->audioStream, nullptr);

    av_write_trailer(m_data->formatCtx);
}

} // namespace MediaEncoder