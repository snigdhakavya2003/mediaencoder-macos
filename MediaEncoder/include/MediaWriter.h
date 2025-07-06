#pragma once

#include <string>
#include <memory>

struct AVFormatContext;
struct AVCodecContext;
struct AVStream;
struct AVFrame;

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

        struct WriterPrivateData;
        std::unique_ptr<WriterPrivateData> m_data;
    };

} // namespace MediaEncoder