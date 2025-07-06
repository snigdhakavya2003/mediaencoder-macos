#pragma once

#include <stdexcept>
#include <cstdint>
#include <memory>

extern "C" {
#include <libswresample/swresample.h>
#include <libavutil/samplefmt.h>
#include <libavutil/channel_layout.h>
#include <libavutil/mem.h>
}

namespace MediaEncoder {

class Resampler {
private:
    SwrContext* m_swrContext;
    int m_srcChannels, m_destChannels;
    AVSampleFormat m_srcSampleFormat, m_destSampleFormat;
    int m_srcSampleRate, m_destSampleRate;
    uint8_t* m_resampledBuffer;
    int m_resampledBufferSize;

    void SwrContextValidation(int srcChannels, AVSampleFormat srcSampleFormat, int srcSampleRate,
                              int destChannels, AVSampleFormat destSampleFormat, int destSampleRate);

public:
    Resampler();
    ~Resampler();

    void Initialize(int srcChannels, AVSampleFormat srcSampleFormat, int srcSampleRate,
                    int destChannels, AVSampleFormat destSampleFormat, int destSampleRate);

    uint8_t* Resample(const uint8_t** srcData, int srcSamples, int& destSamples);

    int EstimateOutputSamples(int srcSamples) const;
};

} // namespace MediaEncoder