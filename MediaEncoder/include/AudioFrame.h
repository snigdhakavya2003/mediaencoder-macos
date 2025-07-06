#pragma once

#include <string>
#include <vector>
#include <stdexcept>
#include <cstring>

extern "C" {
#include <libavutil/frame.h>
#include <libavutil/samplefmt.h>
#include <libavutil/channel_layout.h>
#include <libavutil/imgutils.h>
}

namespace MediaEncoder
{
    class AudioFrame
    {
    private:
        AVFrame* m_avFrame;
        bool m_disposed;
        int m_channels;

        void CheckIfDisposed() const
        {
            if (m_disposed)
                throw std::runtime_error("The object was already disposed.");
        }

    public:
        AudioFrame(int sampleRate, int channels, AVSampleFormat sampleFormat, int samples)
            : m_disposed(false), m_channels(channels)
        {
            m_avFrame = av_frame_alloc();
            if (!m_avFrame)
                throw std::runtime_error("Failed to allocate AVFrame.");

            m_avFrame->sample_rate = sampleRate;
            m_avFrame->format = static_cast<int>(sampleFormat);
            m_avFrame->nb_samples = samples;

            // Use new AVChannelLayout API
            if (av_channel_layout_default(&m_avFrame->ch_layout, channels) < 0)
            {
                av_frame_free(&m_avFrame);
                throw std::runtime_error("Failed to set default channel layout.");
            }

            if (av_frame_get_buffer(m_avFrame, 0) < 0)
            {
                av_frame_free(&m_avFrame);
                throw std::runtime_error("Failed to allocate audio buffer.");
            }
        }

        ~AudioFrame()
        {
            if (m_avFrame)
            {
                av_frame_free(&m_avFrame);
                m_avFrame = nullptr;
            }
            m_disposed = true;
        }

        void FillFrame(const uint8_t* src)
        {
            CheckIfDisposed();
            int bufferSize = av_samples_get_buffer_size(nullptr, m_channels, m_avFrame->nb_samples,
                                                        static_cast<AVSampleFormat>(m_avFrame->format), 0);
            if (bufferSize > 0)
            {
                std::memcpy(m_avFrame->data[0], src, bufferSize);
            }
        }

        void ClearFrame()
        {
            CheckIfDisposed();
            int bufferSize = av_samples_get_buffer_size(nullptr, m_channels, m_avFrame->nb_samples,
                                                        static_cast<AVSampleFormat>(m_avFrame->format), 0);
            if (bufferSize > 0)
            {
                std::memset(m_avFrame->data[0], 0, bufferSize);
            }
        }

        int SampleRate() const
        {
            CheckIfDisposed();
            return m_avFrame->sample_rate;
        }

        int Channels() const
        {
            CheckIfDisposed();
            return m_channels;
        }

        int Samples() const
        {
            CheckIfDisposed();
            return m_avFrame->nb_samples;
        }

        std::vector<int> LineSize() const
        {
            CheckIfDisposed();
            std::vector<int> lineSizes(AV_NUM_DATA_POINTERS);
            for (int i = 0; i < AV_NUM_DATA_POINTERS; ++i)
            {
                lineSizes[i] = m_avFrame->linesize[i];
            }
            return lineSizes;
        }

        std::vector<uint8_t*> DataPointer() const
        {
            CheckIfDisposed();
            std::vector<uint8_t*> dataPointers(AV_NUM_DATA_POINTERS);
            for (int i = 0; i < AV_NUM_DATA_POINTERS; ++i)
            {
                dataPointers[i] = m_avFrame->data[i];
            }
            return dataPointers;
        }

        AVSampleFormat SampleFormat() const
        {
            CheckIfDisposed();
            return static_cast<AVSampleFormat>(m_avFrame->format);
        }

        AVFrame* NativePointer() const
        {
            CheckIfDisposed();
            return m_avFrame;
        }
    };
}