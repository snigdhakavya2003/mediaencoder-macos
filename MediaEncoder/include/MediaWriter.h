#pragma once

#include <string>
#include <memory>

extern "C" {
    #include <libavformat/avformat.h>
    #include <libavcodec/avcodec.h>
}

// Forward declarations
namespace MediaEncoder {
    class VideoFrame;
    class AudioFrame;

    class MediaWriter {
    public:
        MediaWriter(int width, int height, int videoNumerator, int videoDenominator,
                    const std::string& videoCodecName, int videoBitrate,
                    const std::string& audioCodecName, int audioBitrate);

        ~MediaWriter() = default;

        void Open(const std::string& url, const std::string& format);
        void EncodeVideoFrame(VideoFrame* frame);
        void EncodeAudioFrame(AudioFrame* frame);
        void Close();

        int GetWidth() const { return m_width; }
        int GetHeight() const { return m_height; }

    private:
        int m_width;
        int m_height;
        int m_videoNumerator;
        int m_videoDenominator;
        int m_videoBitrate;
        int m_audioBitrate;

        std::string m_videoCodecName;
        std::string m_audioCodecName;
        std::string m_url;
        std::string m_format;

        bool m_disposed;

        // ✅ Full definition required to avoid incomplete type error
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

        std::unique_ptr<WriterPrivateData> m_data;
    };

} // namespace MediaEncoder