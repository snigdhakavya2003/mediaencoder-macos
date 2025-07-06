#include "AudioFrame.h"
#include <stdexcept>
#include <cstring>
#include <memory>

namespace MediaEncoder
{
    AudioFrame::AudioFrame(int sampleRate, int channels, AVSampleFormat sampleFormat, int samples)
        : m_disposed(false), m_channels(channels), m_channelLayout(av_get_default_channel_layout(channels)), m_avFrame(av_frame_alloc())
    {
        if (!m_avFrame) {
            throw std::runtime_error("Failed to allocate AVFrame");
        }

        m_avFrame->format = static_cast<int>(sampleFormat);
        m_avFrame->sample_rate = sampleRate;
        m_avFrame->nb_samples = samples;

        // Set the new-style channel layout
        if (av_channel_layout_default(&m_avFrame->ch_layout, channels) < 0) {
            av_frame_free(&m_avFrame);
            throw std::runtime_error("Failed to set channel layout");
        }

        if (av_frame_get_buffer(m_avFrame, 0) < 0) {
            av_frame_free(&m_avFrame);
            throw std::runtime_error("Failed to allocate buffer for AVFrame");
        }
    }

    void AudioFrame::FillFrame(const uint8_t* src)
    {
        CheckIfDisposed();
        int bufferSize = av_samples_get_buffer_size(
            &m_avFrame->linesize,
            m_channels,
            m_avFrame->nb_samples,
            static_cast<AVSampleFormat>(m_avFrame->format),
            0);

        if (bufferSize > 0) {
            std::memcpy(m_avFrame->data[0], src, bufferSize);
        } else {
            throw std::runtime_error("Invalid buffer size returned from av_samples_get_buffer_size");
        }
    }

    void AudioFrame::ClearFrame()
    {
        CheckIfDisposed();
        int bufferSize = av_samples_get_buffer_size(
            &m_avFrame->linesize,
            m_channels,
            m_avFrame->nb_samples,
            static_cast<AVSampleFormat>(m_avFrame->format),
            0);

        if (bufferSize > 0) {
            std::memset(m_avFrame->data[0], 0, bufferSize);
        } else {
            throw std::runtime_error("Invalid buffer size returned from av_samples_get_buffer_size");
        }
    }

    AudioFrame::~AudioFrame()
    {
        if (m_avFrame) {
            av_frame_free(&m_avFrame);
        }
        m_disposed = true;
    }

    void AudioFrame::CheckIfDisposed() const
    {
        if (m_disposed) {
            throw std::runtime_error("The object has already been disposed.");
        }
    }

    const AVFrame* AudioFrame::GetAVFrame() const
    {
        return m_avFrame;
    }
}