#pragma once

#include <vector>
#include <stdexcept>
#include <cstdint>

extern "C" {
#include <libavutil/frame.h>
#include <libavutil/samplefmt.h>
#include <libavutil/channel_layout.h>
}

namespace MediaEncoder
{
    class AudioFrame
    {
    private:
        AVFrame* m_avFrame;
        bool m_disposed;
        int m_channels;

        void CheckIfDisposed() const;

    public:
        AudioFrame(int sampleRate, int channels, AVSampleFormat sampleFormat, int samples);
        ~AudioFrame();

        void FillFrame(const uint8_t* src);
        void ClearFrame();

        int SampleRate() const;
        int Channels() const;
        int Samples() const;
        AVSampleFormat SampleFormat() const;

        std::vector<int> LineSize() const;
        std::vector<uint8_t*> DataPointer() const;

        AVFrame* NativePointer() const;
    };
}