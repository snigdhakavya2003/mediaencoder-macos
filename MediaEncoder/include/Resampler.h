
#pragma once

#include <stdexcept>
#include <cstdint>
#include <memory>
#include <libswresample/swresample.h>
#include <libavutil/samplefmt.h>

namespace MediaEncoder {

class Resampler {
private:
    SwrContext* m_swrContext;
    int m_srcChannels, m_destChannels;
    AVSampleFormat m_srcSampleFormat, m_destSampleFormat;
    int m_srcSampleRate, m_destSampleRate;
    uint8_t* m_resampledBuffer;
    int m_resampledBufferSampleSize, m_resampledBufferSize;

    void SwrContextValidation(int srcChannels, AVSampleFormat srcSampleFormat, int srcSampleRate,
                              int destChannels, AVSampleFormat destSampleFormat, int destSampleRate) {
        // Validate and reinitialize SwrContext if parameters change
        if (m_srcChannels != srcChannels || m_srcSampleFormat != srcSampleFormat || m_srcSampleRate != srcSampleRate ||
            m_destChannels != destChannels || m_destSampleFormat != destSampleFormat || m_destSampleRate != destSampleRate) {

            m_srcChannels = srcChannels;
            m_srcSampleFormat = srcSampleFormat;
            m_srcSampleRate = srcSampleRate;
            m_destChannels = destChannels;
            m_destSampleFormat = destSampleFormat;
            m_destSampleRate = destSampleRate;

            if (m_swrContext) {
                swr_free(&m_swrContext);
            }

            m_swrContext = swr_alloc_set_opts(nullptr,
                av_get_default_channel_layout(m_destChannels), m_destSampleFormat, m_destSampleRate,
                av_get_default_channel_layout(m_srcChannels), m_srcSampleFormat, m_srcSampleRate,
                0, nullptr);

            if (!m_swrContext || swr_init(m_swrContext) < 0) {
                throw std::runtime_error("Failed to initialize SwrContext");
            }
        }
    }

public:
    Resampler() 
        : m_swrContext(nullptr), m_srcChannels(0), m_destChannels(0),
          m_srcSampleFormat(AV_SAMPLE_FMT_NONE), m_destSampleFormat(AV_SAMPLE_FMT_NONE),
          m_srcSampleRate(0), m_destSampleRate(0), m_resampledBuffer(nullptr),
          m_resampledBufferSampleSize(0), m_resampledBufferSize(0) {}

    ~Resampler() {
        if (m_swrContext) {
            swr_free(&m_swrContext);
        }
        if (m_resampledBuffer) {
            av_free(m_resampledBuffer);
        }
    }

    void Initialize(int srcChannels, AVSampleFormat srcSampleFormat, int srcSampleRate,
                    int destChannels, AVSampleFormat destSampleFormat, int destSampleRate) {
        SwrContextValidation(srcChannels, srcSampleFormat, srcSampleRate, destChannels, destSampleFormat, destSampleRate);
    }

    void Resample(uint8_t** srcData, int srcSamples, uint8_t** destData, int& destSamples) {
        if (!m_swrContext) {
            throw std::runtime_error("SwrContext is not initialized");
        }

        int requiredDestSamples = swr_get_out_samples(m_swrContext, srcSamples);
        int requiredDestBufferSize = av_samples_get_buffer_size(nullptr, m_destChannels, requiredDestSamples,
                                                                m_destSampleFormat, 1);
        if (m_resampledBufferSize < requiredDestBufferSize) {
            if (m_resampledBuffer) {
                av_free(m_resampledBuffer);
            }
            m_resampledBuffer = static_cast<uint8_t*>(av_malloc(requiredDestBufferSize));
            m_resampledBufferSize = requiredDestBufferSize;
        }

        const uint8_t* pSrcBuffer = *srcData;
        uint8_t* pDestBuffer = m_resampledBuffer;
        destSamples = swr_convert(m_swrContext, &pDestBuffer, m_resampledBufferSampleSize, &pSrcBuffer, srcSamples);
        if (destSamples < 0) {
            throw std::runtime_error("swr_convert() failed");
        }
        *destData = m_resampledBuffer;
    }

    int EstimateOutputSamples(int srcSamples) {
        if (!m_swrContext) {
            throw std::runtime_error("SwrContext is not initialized");
        }
        return swr_get_out_samples(m_swrContext, srcSamples);
    }
};

} // namespace MediaEncoder