cpp
#pragma once

#include <string>
#include <vector>
#include <stdexcept>
#include <cstring>
#include <libavutil/frame.h>
#include <libavutil/samplefmt.h>
#include <libavutil/imgutils.h>

namespace MediaEncoder
{
    class AudioFrame
    {
    private:
        AVFrame* m_avFrame;
        bool m_disposed;

        void CheckIfDisposed()
        {
            if (m_disposed)
                throw std::runtime_error("The object was already disposed.");
        }

    public:
        AudioFrame(int sampleRate, int channels, AVSampleFormat sampleFormat, int samples)
            : m_disposed(false)
        {
            m_avFrame = av_frame_alloc();
            if (!m_avFrame)
            {
                throw std::runtime_error("Failed to allocate AVFrame.");
            }

            m_avFrame->sample_rate = sampleRate;
            m_avFrame->channels = channels;
            m_avFrame->format = static_cast<int>(sampleFormat);
            m_avFrame->nb_samples = samples;

            // Allocate buffer for audio data
            if (av_frame_get_buffer(m_avFrame, 0) < 0)
            {
                av_frame_free(&m_avFrame);
                throw std::runtime_error("Failed to allocate frame buffer.");
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
            int bufferSize = av_samples_get_buffer_size(nullptr, m_avFrame->channels, m_avFrame->nb_samples, static_cast<AVSampleFormat>(m_avFrame->format), 0);
            if (bufferSize > 0)
            {
                std::memcpy(m_avFrame->data[0], src, bufferSize);
            }
        }

        void ClearFrame()
        {
            CheckIfDisposed();
            std::memset(m_avFrame->data[0], 0, av_samples_get_buffer_size(nullptr, m_avFrame->channels, m_avFrame->nb_samples, static_cast<AVSampleFormat>(m_avFrame->format), 0));
        }

        int SampleRate()
        {
            CheckIfDisposed();
            return m_avFrame->sample_rate;
        }

        int Channels()
        {
            CheckIfDisposed();
            return av_get_channel_layout_nb_channels(m_avFrame->channel_layout);
        }

        int Samples()
        {
            CheckIfDisposed();
            return m_avFrame->nb_samples;
        }

        std::vector<int> LineSize()
        {
            CheckIfDisposed();
            std::vector<int> lineSizes(AV_NUM_DATA_POINTERS);
            for (int i = 0; i < AV_NUM_DATA_POINTERS; ++i)
            {
                lineSizes[i] = m_avFrame->linesize[i];
            }
            return lineSizes;
        }

        std::vector<uint8_t*> DataPointer()
        {
            CheckIfDisposed();
            std::vector<uint8_t*> dataPointers(AV_NUM_DATA_POINTERS);
            for (int i = 0; i < AV_NUM_DATA_POINTERS; ++i)
            {
                dataPointers[i] = m_avFrame->data[i];
            }
            return dataPointers;
        }

        AVSampleFormat SampleFormat()
        {
            CheckIfDisposed();
            return static_cast<AVSampleFormat>(m_avFrame->format);
        }
    };
}

