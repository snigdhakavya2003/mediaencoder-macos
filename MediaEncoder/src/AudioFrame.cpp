#include "AudioFrame.h"
#include <cstring>

namespace MediaEncoder
{
    AudioFrame::AudioFrame(int sampleRate, int channels, AVSampleFormat sampleFormat, int samples)
        : m_disposed(false), m_channels(channels)
    {
        m_avFrame = av_frame_alloc();
        if (!m_avFrame)
            throw std::runtime_error("Failed to allocate AVFrame.");

        m_avFrame->sample_rate = sampleRate;
        m_avFrame->format = static_cast<int>(sampleFormat);
        m_avFrame->nb_samples = samples;

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

    AudioFrame::~AudioFrame()
    {
        if (m_avFrame)
        {
            av_frame_free(&m_avFrame);
        }
        m_disposed = true;
    }

    void AudioFrame::CheckIfDisposed() const
    {
        if (m_disposed)
            throw std::runtime_error("The object has already been disposed.");
    }

    void AudioFrame::FillFrame(const uint8_t* src)
    {
        CheckIfDisposed();
        int bufferSize = av_samples_get_buffer_size(nullptr, m_channels, m_avFrame->nb_samples,
                                                    static_cast<AVSampleFormat>(m_avFrame->format), 0);
        if (bufferSize > 0)
        {
            std::memcpy(m_avFrame->data[0], src, bufferSize);
        }
    }

    void AudioFrame::ClearFrame()
    {
        CheckIfDisposed();
        int bufferSize = av_samples_get_buffer_size(nullptr, m_channels, m_avFrame->nb_samples,
                                                    static_cast<AVSampleFormat>(m_avFrame->format), 0);
        if (bufferSize > 0)
        {
            std::memset(m_avFrame->data[0], 0, bufferSize);
        }
    }

    int AudioFrame::SampleRate() const
    {
        CheckIfDisposed();
        return m_avFrame->sample_rate;
    }

    int AudioFrame::Channels() const
    {
        CheckIfDisposed();
        return m_channels;
    }

    int AudioFrame::Samples() const
    {
        CheckIfDisposed();
        return m_avFrame->nb_samples;
    }

    AVSampleFormat AudioFrame::SampleFormat() const
    {
        CheckIfDisposed();
        return static_cast<AVSampleFormat>(m_avFrame->format);
    }

    std::vector<int> AudioFrame::LineSize() const
    {
        CheckIfDisposed();
        std::vector<int> lineSizes(AV_NUM_DATA_POINTERS);
        for (int i = 0; i < AV_NUM_DATA_POINTERS; ++i)
        {
            lineSizes[i] = m_avFrame->linesize[i];
        }
        return lineSizes;
    }

    std::vector<uint8_t*> AudioFrame::DataPointer() const
    {
        CheckIfDisposed();
        std::vector<uint8_t*> dataPointers(AV_NUM_DATA_POINTERS);
        for (int i = 0; i < AV_NUM_DATA_POINTERS; ++i)
        {
            dataPointers[i] = m_avFrame->data[i];
        }
        return dataPointers;
    }

    AVFrame* AudioFrame::NativePointer() const
    {
        CheckIfDisposed();
        return m_avFrame;
    }
}