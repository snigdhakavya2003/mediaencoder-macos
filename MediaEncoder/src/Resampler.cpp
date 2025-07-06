#include "Resampler.h"
#include "SampleFormat.h"

#include <stdexcept>
#include <cstdint>
#include <libswresample/swresample.h>
#include <libavutil/samplefmt.h>
#include <libavutil/channel_layout.h>

namespace MediaEncoder {

    Resampler::Resampler()
        : m_swrContext(nullptr),
          m_srcChannels(-1), m_destChannels(-1),
          m_srcSampleFormat(SampleFormat::NONE), m_destSampleFormat(SampleFormat::NONE),
          m_srcSampleRate(-1), m_destSampleRate(-1),
          m_resampledBuffer(nullptr),
          m_resampledBufferSize(-1),
          m_disposed(false)
    {
    }

    Resampler::~Resampler()
    {
        if (m_swrContext) {
            swr_free(&m_swrContext);
        }
        if (m_resampledBuffer) {
            av_free(m_resampledBuffer);
        }
    }

    void Resampler::SwrContextValidation(int srcChannels, SampleFormat srcSampleFormat, int srcSampleRate,
                                         int destChannels, SampleFormat destSampleFormat, int destSampleRate)
    {
        if (m_srcChannels != srcChannels || m_srcSampleFormat != srcSampleFormat || m_srcSampleRate != srcSampleRate ||
            m_destChannels != destChannels || m_destSampleFormat != destSampleFormat || m_destSampleRate != destSampleRate)
        {
            m_srcChannels = srcChannels;
            m_srcSampleFormat = srcSampleFormat;
            m_srcSampleRate = srcSampleRate;
            m_destChannels = destChannels;
            m_destSampleFormat = destSampleFormat;
            m_destSampleRate = destSampleRate;

            if (m_swrContext) {
                swr_free(&m_swrContext);
                m_swrContext = nullptr;
            }

            AVChannelLayout srcLayout, dstLayout;

            if (av_channel_layout_default(&srcLayout, srcChannels) < 0 ||
                av_channel_layout_default(&dstLayout, destChannels) < 0) {
                throw std::runtime_error("Failed to get default channel layout");
            }

            if (swr_alloc_set_opts2(&m_swrContext,
                                    &dstLayout, static_cast<AVSampleFormat>(destSampleFormat), destSampleRate,
                                    &srcLayout, static_cast<AVSampleFormat>(srcSampleFormat), srcSampleRate,
                                    0, nullptr) < 0 ||
                swr_init(m_swrContext) < 0) {
                throw std::runtime_error("Failed to initialize SwrContext");
            }

            av_channel_layout_uninit(&srcLayout);
            av_channel_layout_uninit(&dstLayout);
        }
    }

    void Resampler::Resampling(int srcChannels, SampleFormat srcSampleFormat, int srcSampleRate,
                               int destChannels, SampleFormat destSampleFormat, int destSampleRate,
                               uint8_t* srcData, int srcSamples, uint8_t** destData, int& destSamples)
    {
        SwrContextValidation(srcChannels, srcSampleFormat, srcSampleRate,
                             destChannels, destSampleFormat, destSampleRate);

        if (m_swrContext) {
            int requiredDestSamples = swr_get_out_samples(m_swrContext, srcSamples);
            int requiredDestBufferSize = av_samples_get_buffer_size(
                nullptr, destChannels, requiredDestSamples,
                static_cast<AVSampleFormat>(destSampleFormat), 1
            );

            if (m_resampledBufferSize < requiredDestBufferSize) {
                if (m_resampledBuffer) {
                    av_free(m_resampledBuffer);
                }

                m_resampledBufferSize = requiredDestBufferSize;
                m_resampledBuffer = static_cast<uint8_t*>(av_malloc(m_resampledBufferSize));
                if (!m_resampledBuffer) {
                    throw std::runtime_error("Failed to allocate resample buffer");
                }
            }

            const uint8_t* pSrcBuffer = srcData;
            uint8_t* pDestBuffer = m_resampledBuffer;

            int samples = swr_convert(
                m_swrContext, &pDestBuffer, requiredDestSamples,
                &pSrcBuffer, srcSamples
            );

            if (samples < 0) {
                throw std::runtime_error("swr_convert() failed");
            }

            *destData = m_resampledBuffer;
            destSamples = samples;
        } else {
            // No resampling needed
            *destData = srcData;
            destSamples = srcSamples;
        }
    }

    int Resampler::MeasureResamplingOutputSamples(int srcChannels, SampleFormat srcSampleFormat, int srcSampleRate,
                                                  int destChannels, SampleFormat destSampleFormat, int destSampleRate,
                                                  int srcSamples)
    {
        SwrContextValidation(srcChannels, srcSampleFormat, srcSampleRate,
                             destChannels, destSampleFormat, destSampleRate);

        if (!m_swrContext) {
            throw std::runtime_error("SwrContext is null");
        }

        return swr_get_out_samples(m_swrContext, srcSamples);
    }

} // namespace MediaEncoder