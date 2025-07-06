#pragma once

#include <string>
#include <stdexcept>
#include <memory>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/log.h>
#include <libavutil/imgutils.h>
}

#include "VideoFrame.h"
#include "AudioFrame.h"

namespace MediaEncoder {

class MediaWriter {
private:
    static bool h264_avfoundation;
    static bool hevc_avfoundation;

    static bool IsValidEncoder(const char* name) {
        const AVCodec* codec = avcodec_find_encoder_by_name(name);
        if (!codec) return false;

        AVCodecContext* ctx = avcodec_alloc_context3(codec);
        if (!ctx) return false;

        ctx->width = 1920;
        ctx->height = 1080;
        ctx->time_base = AVRational{1, 60};
        ctx->framerate = AVRational{60, 1};
        ctx->bit_rate = 10000000;

        bool success = avcodec_open2(ctx, codec, nullptr) >= 0;
        avcodec_free_context(&ctx);
        return success;
    }

public:
    static void CheckHardwareCodec() {
        h264_avfoundation = IsValidEncoder("h264");
        hevc_avfoundation = IsValidEncoder("hevc");

        av_log(nullptr, AV_LOG_INFO, h264_avfoundation ? "✅ H.264 (avfoundation) supported\n" : "❌ H.264 not supported\n");
        av_log(nullptr, AV_LOG_INFO, hevc_avfoundation ? "✅ HEVC (avfoundation) supported\n" : "❌ HEVC not supported\n");
    }

    static bool IsSupportedH264() { return h264_avfoundation; }
    static bool IsSupportedHEVC() { return hevc_avfoundation; }

private:
    int m_width;
    int m_height;
    int m_videoNumerator;
    int m_videoDenominator;
    std::string m_videoCodecName;
    uint64_t m_videoFramesCount = 0;
    int m_videoBitrate;

    int m_audioSampleRate = 44100;
    std::string m_audioCodecName;
    uint64_t m_audioSamplesCount = 0;
    int m_audioBitrate;

    std::string m_format;
    std::string m_url;

    bool m_disposed = false;

    void CheckIfWriterIsInitialized() const {
        if (m_url.empty())
            throw std::runtime_error("Writer is not initialized.");
    }

    void CheckIfDisposed() const {
        if (m_disposed)
            throw std::runtime_error("MediaWriter already disposed.");
    }

public:
    MediaWriter(int width, int height, int video_numerator, int video_denominator,
                const std::string& video_codec, int video_bitrate,
                const std::string& audio_codec, int audio_bitrate)
        : m_width(width), m_height(height),
          m_videoNumerator(video_numerator), m_videoDenominator(video_denominator),
          m_videoCodecName(video_codec), m_videoBitrate(video_bitrate),
          m_audioCodecName(audio_codec), m_audioBitrate(audio_bitrate) {}

    ~MediaWriter() {
        Close();
    }

    void Open(const std::string& url, const std::string& format) {
        CheckIfDisposed();
        m_url = url;
        m_format = format;

        // TODO: Setup AVFormatContext, AVStream, AVCodecContext here
    }

    void Close() {
        if (m_disposed) return;
        // TODO: Free all AV* contexts, flush encoder if needed
        m_disposed = true;
    }

    void EncodeVideoFrame(const std::shared_ptr<VideoFrame>& videoFrame) {
        CheckIfDisposed();
        CheckIfWriterIsInitialized();
        // TODO: Encode frame and write to stream
    }

    void EncodeAudioFrame(const std::shared_ptr<AudioFrame>& audioFrame) {
        CheckIfDisposed();
        CheckIfWriterIsInitialized();
        // TODO: Encode audio and write to stream
    }
};

// Define static variables
bool MediaWriter::h264_avfoundation = false;
bool MediaWriter::hevc_avfoundation = false;

} // namespace MediaEncoder