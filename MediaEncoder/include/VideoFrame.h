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
#include <cstring>

namespace MediaEncoder {

class VideoFrame {
private:
    AVFrame* m_avFrame;
    bool m_disposed;
    int m_bufferSize;

    void CheckIfDisposed() const {
        if (m_disposed || !m_avFrame)
            throw std::runtime_error("The object is disposed or invalid.");
    }

public:
    // Constructor
    VideoFrame(int width, int height, AVPixelFormat pixelFormat)
        : m_avFrame(av_frame_alloc()), m_disposed(false), m_bufferSize(0)
    {
        if (!m_avFrame)
            throw std::runtime_error("Could not allocate AVFrame");

        m_avFrame->format = pixelFormat;
        m_avFrame->width = width;
        m_avFrame->height = height;

        m_bufferSize = av_image_alloc(m_avFrame->data, m_avFrame->linesize,
                                      width, height, pixelFormat, 32);
        if (m_bufferSize < 0) {
            av_frame_free(&m_avFrame);
            throw std::runtime_error("Could not allocate raw picture buffer");
        }
    }

    // Shared pointer creator
    static std::shared_ptr<VideoFrame> CreateShared(int width, int height, AVPixelFormat format) {
        return std::make_shared<VideoFrame>(width, height, format);
    }

    // Destructor
    ~VideoFrame() {
        Dispose();
    }

    void Dispose() {
        if (!m_disposed) {
            if (m_avFrame) {
                av_freep(&m_avFrame->data[0]);  // Free buffer from av_image_alloc
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
        for (int i = 0; i < AV_NUM_DATA_POINTERS; ++i)
            result[i] = m_avFrame->linesize[i];
        return result;
    }

    std::vector<uint8_t*> DataPointer() const {
        CheckIfDisposed();
        std::vector<uint8_t*> result(AV_NUM_DATA_POINTERS);
        for (int i = 0; i < AV_NUM_DATA_POINTERS; ++i)
            result[i] = m_avFrame->data[i];
        return result;
    }

    void FillFrame(const uint8_t* srcData, int srcStride) {
        CheckIfDisposed();
        av_image_fill_arrays(m_avFrame->data, m_avFrame->linesize, srcData,
                             static_cast<AVPixelFormat>(m_avFrame->format),
                             m_avFrame->width, m_avFrame->height, 1);
    }
};

using VideoFrameHandle = std::shared_ptr<VideoFrame>;

} // namespace MediaEncoder