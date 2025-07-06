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
      m_srcSampleFormat(AV_SAMPLE_FMT_NONE), m_destSampleFormat(AV_SAMPLE_FMT_NONE),
      m_srcSampleRate(-1), m_destSampleRate(-1),
      m_resampledBuffer(nullptr),
      m_resampledBufferSize(-1)
{}

Resampler::~Resampler()
{
    if (m_swrContext) {
        swr_free(&m_swrContext);
    }
    if (m_resampledBuffer) {
        av_free(m_resampledBuffer);
    }
}

void Resampler::SwrContextValidation(int srcChannels, AVSampleFormat srcSampleFormat, int srcSampleRate,
                                     int destChannels, AVSampleFormat destSampleFormat, int destSampleRate)
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
        av_channel_layout_default(&srcLayout, srcChannels);
        av_channel_layout_default(&dstLayout, destChannels);

        if (swr_alloc_set_opts2(&m_swrContext,
                                &dstLayout, destSampleFormat, destSampleRate,
                                &srcLayout, srcSampleFormat, srcSampleRate,
                                0, nullptr) < 0 || swr_init(m_swrContext) < 0)
        {
            throw std::runtime_error("Failed to initialize SwrContext");
        }

        av_channel_layout_uninit(&srcLayout);
        av_channel_layout_uninit(&dstLayout);
    }
}

void Resampler::Initialize(int srcChannels, AVSampleFormat srcSampleFormat, int srcSampleRate,
                           int destChannels, AVSampleFormat destSampleFormat, int destSampleRate)
{
    SwrContextValidation(srcChannels, srcSampleFormat, srcSampleRate,
                         destChannels, destSampleFormat, destSampleRate);
}

uint8_t* Resampler::Resample(const uint8_t** srcData, int srcSamples, int& destSamples)
{
    if (!m_swrContext) {
        throw std::runtime_error("SwrContext is not initialized");
    }

    int maxDstSamples = swr_get_out_samples(m_swrContext, srcSamples);
    int bufferSize = av_samples_get_buffer_size(nullptr, m_destChannels, maxDstSamples, m_destSampleFormat, 1);

    if (bufferSize < 0) {
        throw std::runtime_error("Failed to get output buffer size");
    }

    if (m_resampledBufferSize < bufferSize) {
        if (m_resampledBuffer) {
            av_free(m_resampledBuffer);
        }
        m_resampledBuffer = static_cast<uint8_t*>(av_malloc(bufferSize));
        if (!m_resampledBuffer) {
            throw std::runtime_error("Failed to allocate resample buffer");
        }
        m_resampledBufferSize = bufferSize;
    }

    destSamples = swr_convert(
        m_swrContext, &m_resampledBuffer, maxDstSamples,
        srcData, srcSamples
    );

    if (destSamples < 0) {
        throw std::runtime_error("swr_convert() failed");
    }

    return m_resampledBuffer;
}

int Resampler::EstimateOutputSamples(int srcSamples) const
{
    if (!m_swrContext) {
        throw std::runtime_error("SwrContext is not initialized");
    }
    return swr_get_out_samples(m_swrContext, srcSamples);
}

} // namespace MediaEncoder