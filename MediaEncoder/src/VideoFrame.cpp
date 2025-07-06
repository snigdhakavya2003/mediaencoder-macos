
#include <stdexcept>
#include <cstdint>
#include <array>
#include <memory>
#include <iostream>
extern "C" {
#include <libavutil/frame.h>
#include <libavutil/imgutils.h>
}

namespace MediaEncoder {

class VideoFrame {
public:
    // Constructor to clone another VideoFrame
    VideoFrame(const VideoFrame& videoFrame) {
        m_avFrame = av_frame_clone(videoFrame.m_avFrame);
        if (!m_avFrame) {
            throw std::runtime_error("Failed to clone AVFrame");
        }
    }

    // Constructor to create a VideoFrame with specified width, height, and pixel format
    VideoFrame(int width, int height, AVPixelFormat pixelFormat) : m_disposed(false) {
        m_avFrame = av_frame_alloc();
        if (!m_avFrame) {
            throw std::runtime_error("Failed to allocate AVFrame");
        }
        m_avFrame->width = width;
        m_avFrame->height = height;
        m_avFrame->format = pixelFormat;
        if (av_frame_get_buffer(m_avFrame, 32) < 0) {
            av_frame_free(&m_avFrame);
            throw std::runtime_error("Failed to allocate frame buffer");
        }
    }

    // Stub for Windows-specific BitmapSource constructor
    // VideoFrame(Windows::Media::Imaging::BitmapSource^ bitmapSource) : m_disposed(false) {
    //     // Implementation for bitmap source would go here
    // }

    // Destructor to free the AVFrame
    ~VideoFrame() {
        if (m_avFrame) {
            av_frame_free(&m_avFrame);
        }
    }

    // Fill the frame with pixel data from a single source
    void FillFrame(uint8_t* src, int srcStride) {
        CheckIfDisposed();

        const uint8_t* src_data[4] = {src, nullptr, nullptr, nullptr};
        int src_linesize[4] = {srcStride, 0, 0, 0};

        av_image_copy(m_avFrame->data, m_avFrame->linesize, src_data, src_linesize,
                      static_cast<AVPixelFormat>(m_avFrame->format), m_avFrame->width, m_avFrame->height);
    }

    // Fill the frame with pixel data from multiple sources
    void FillFrame(uint8_t** src, int* srcStride, int numPlanes) {
        CheckIfDisposed();

        const uint8_t* src_data[4];
        int src_linesize[4];

        for (int i = 0; i < 4; i++) {
            src_data[i] = (i < numPlanes) ? src[i] : nullptr;
            src_linesize[i] = (i < numPlanes) ? srcStride[i] : 0;
        }

        av_image_copy(m_avFrame->data, m_avFrame->linesize, src_data, src_linesize,
                      static_cast<AVPixelFormat>(m_avFrame->format), m_avFrame->width, m_avFrame->height);
    }

private:
    AVFrame* m_avFrame = nullptr;
    bool m_disposed = false;

    void CheckIfDisposed() {
        if (m_disposed) {
            throw std::runtime_error("The object was already disposed.");
        }
    }
};

} // namespace MediaEncoder