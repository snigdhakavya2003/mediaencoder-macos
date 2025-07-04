cpp
#pragma once

extern "C" {
#include <libavutil/frame.h>
#include <libavutil/imgutils.h>
#include <libavutil/pixfmt.h>
}

#include <stdexcept>
#include <memory>
#include <cstdint>
#include <vector>

namespace MediaEncoder {

class VideoFrame {
private:
    AVFrame* m_avFrame;
    bool m_disposed;

    void CheckIfDisposed() const {
        if (m_disposed) {
            throw std::runtime_error("The object was already disposed.");
        }
    }

public:
    // Constructor from width/height and pixel format
    VideoFrame(int width, int height, AVPixelFormat pixelFormat) : m_disposed(false) {
        m_avFrame = av_frame_alloc();
        if (!m_avFrame) {
            throw std::runtime_error("Could not allocate AVFrame");
        }

        m_avFrame->format = pixelFormat;
        m_avFrame->width = width;
        m_avFrame->height = height;

        int ret = av_image_alloc(m_avFrame->data, m_avFrame->linesize, width, height, pixelFormat, 32);
        if (ret < 0) {
            av_frame_free(&m_avFrame);
            throw std::runtime_error("Could not allocate raw picture buffer");
        }
    }

    // Copy constructor
    VideoFrame(const VideoFrame& other) : m_disposed(false) {
        CheckIfDisposed();

        m_avFrame = av_frame_clone(other.m_avFrame);
        if (!m_avFrame) {
            throw std::runtime_error("Failed to clone AVFrame");
        }
    }

    // Move constructor
    VideoFrame(VideoFrame&& other) noexcept : m_avFrame(other.m_avFrame), m_disposed(other.m_disposed) {
        other.m_avFrame = nullptr;
        other.m_disposed = true;
    }

    ~VideoFrame() {
        Dispose();
    }

    void Dispose() {
        if (!m_disposed) {
            if (m_avFrame) {
                av_freep(&m_avFrame->data[0]);
                av_frame_free(&m_avFrame);
            }
            m_disposed = true;
        }
    }

    AVFrame* NativePointer() const {
        CheckIfDisposed();
        return m_avFrame;
    }

    int Width() const {
        CheckIfDisposed();
        return m_avFrame->width;
    }

    int Height() const {
        CheckIfDisposed();
        return m_avFrame->height;
    }

    AVPixelFormat PixelFormat() const {
        CheckIfDisposed();
        return static_cast<AVPixelFormat>(m_avFrame->format);
    }

    std::vector<int> LineSize() const {
        CheckIfDisposed();
        std::vector<int> result(AV_NUM_DATA_POINTERS);
        for (int i = 0; i < AV_NUM_DATA_POINTERS; ++i) {
            result[i] = m_avFrame->linesize[i];
        }
        return result;
    }

    std::vector<uint8_t*> DataPointer() const {
        CheckIfDisposed();
        std::vector<uint8_t*> result(AV_NUM_DATA_POINTERS);
        for (int i = 0; i < AV_NUM_DATA_POINTERS; ++i) {
            result[i] = m_avFrame->data[i];
        }
        return result;
    }

    // Fill frame from external buffer (interleaved)
    void FillFrame(uint8_t* srcData, int srcStride) {
        CheckIfDisposed();
        av_image_fill_arrays(m_avFrame->data, m_avFrame->linesize, srcData,
                             static_cast<AVPixelFormat>(m_avFrame->format),
                             m_avFrame->width, m_avFrame->height, 1);
    }
};

} // namespace MediaEncoder