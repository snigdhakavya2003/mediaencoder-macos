
#pragma once

#include <iostream>
#include <string>
#include <stdexcept>
#include <AVFoundation/AVFoundation.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/log.h>
#include <libavutil/imgutils.h>

namespace MediaEncoder
{
    class MediaWriter
    {
    private:
        static bool h264_avfoundation;
        static bool hevc_avfoundation;

        static bool IsValidEncoder(const char* name)
        {
            const AVCodec* videoCodec = avcodec_find_encoder_by_name(name);
            if (videoCodec)
            {
                AVCodecContext* videoCodecContext = avcodec_alloc_context3(videoCodec);
                try
                {
                    videoCodecContext->width = 1920;
                    videoCodecContext->height = 1080;
                    videoCodecContext->time_base = av_make_q(1, 60);
                    videoCodecContext->framerate = av_make_q(60, 1);
                    videoCodecContext->bit_rate = 10000000;

                    if (avcodec_open2(videoCodecContext, videoCodec, nullptr) < 0)
                    {
                        return false;
                    }
                    return true;
                }
                finally
                {
                    avcodec_free_context(&videoCodecContext);
                }
            }
            return false;
        }

    public:
        static void CheckHardwareCodec()
        {
            h264_avfoundation = IsValidEncoder("h264");
            hevc_avfoundation = IsValidEncoder("hevc");

            av_log(nullptr, AV_LOG_INFO, h264_avfoundation ? "H.264 is supported\n" : "H.264 is not supported\n");
            av_log(nullptr, AV_LOG_INFO, hevc_avfoundation ? "HEVC is supported\n" : "HEVC is not supported\n");
        }

        static bool IsSupportedH264() { return h264_avfoundation; }
        static bool IsSupportedHEVC() { return hevc_avfoundation; }

    private:
        int m_width;
        int m_height;
        int m_videoNumerator;
        int m_videoDenominator;
        std::string m_videoCodecName;
        uint64_t m_videoFramesCount;
        int m_videoBitrate;

        int m_audioSampleRate;
        std::string m_audioCodecName;
        uint64_t m_audioSamplesCount;
        int m_audioBitrate;

        std::string m_format;
        std::string m_url;

        bool m_disposed;

        void CheckIfWriterIsInitialized()
        {
            if (m_url.empty())
                throw std::runtime_error("Video file is not open, cannot access its properties.");
        }

        void CheckIfDisposed()
        {
            if (m_disposed)
                throw std::runtime_error("The object was already disposed.");
        }

    public:
        MediaWriter(int width, int height, int video_numerator, int video_denominator, const std::string& video_codec,
                    int video_bitrate, const std::string& audio_codec, int audio_bitrate)
            : m_width(width), m_height(height), m_videoNumerator(video_numerator), m_videoDenominator(video_denominator),
              m_videoCodecName(video_codec), m_videoBitrate(video_bitrate), m_audioSampleRate(44100), 
              m_audioCodecName(audio_codec), m_audioBitrate(audio_bitrate), m_disposed(false)
        {
            // Initialization code here
        }

        ~MediaWriter()
        {
            Close();
            m_disposed = true;
        }

        void Open(const std::string& url, const std::string& format)
        {
            m_url = url;
            m_format = format;
            // Open file logic here
        }

        void Close()
        {
            // Cleanup logic here
        }

        void EncodeVideoFrame(VideoFrame^ videoFrame)
        {
            // Encoding logic here
        }

        void EncodeAudioFrame(AudioFrame^ audioFrame)
        {
            // Encoding logic here
        }
    };
}