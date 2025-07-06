#include "VideoFrame.h"

extern "C" {
#include <libavutil/imgutils.h>
#include <libavutil/frame.h>
}

namespace MediaEncoder {

VideoFrame::VideoFrame(int width, int height, AVPixelFormat pixelFormat)
    : m_disposed(false)
{
    m_avFrame = av_frame_alloc();
    if (!m_avFrame) {
        throw std::runtime_error("Failed to allocate AVFrame");
    }

    m_avFrame->width = width;
    m_avFrame->height = height;
    m_avFrame->format = pixelFormat;

    int ret = av_image_alloc(m_avFrame->data, m_avFrame->linesize, width, height, pixelFormat, 32);
    if (ret < 0) {
        av_frame_free(&m_avFrame);
        throw std::runtime_error("Failed to allocate raw picture buffer");
    }
}

VideoFrame::VideoFrame(const VideoFrame& other)
    : m_disposed(false)
{
    m_avFrame = av_frame_clone(other.m_avFrame);
    if (!m_avFrame) {
        throw std::runtime_error("Failed to clone AVFrame");
    }
}

VideoFrame::VideoFrame(VideoFrame&& other) noexcept
    : m_avFrame(other.m_avFrame), m_disposed(other.m_disposed)
{
    other.m_avFrame = nullptr;
    other.m_disposed = true;
}

VideoFrame::~VideoFrame()
{
    Dispose();
}

void VideoFrame::Dispose()
{
    if (!m_disposed) {
        if (m_avFrame) {
            av_freep(&m_avFrame->data[0]);
            av_frame_free(&m_avFrame);
        }
        m_disposed = true;
    }
}

void VideoFrame::FillFrame(uint8_t* srcData, int srcStride)
{
    CheckIfDisposed();
    av_image_fill_arrays(m_avFrame->data, m_avFrame->linesize, srcData,
                         static_cast<AVPixelFormat>(m_avFrame->format),
                         m_avFrame->width, m_avFrame->height, 1);
}

AVFrame* VideoFrame::NativePointer() const {
    CheckIfDisposed();
    return m_avFrame;
}

int VideoFrame::Width() const {
    CheckIfDisposed();
    return m_avFrame->width;
}

int VideoFrame::Height() const {
    CheckIfDisposed();
    return m_avFrame->height;
}

AVPixelFormat VideoFrame::PixelFormat() const {
    CheckIfDisposed();
    return static_cast<AVPixelFormat>(m_avFrame->format);
}

std::vector<int> VideoFrame::LineSize() const {
    CheckIfDisposed();
    std::vector<int> result(AV_NUM_DATA_POINTERS);
    for (int i = 0; i < AV_NUM_DATA_POINTERS; ++i) {
        result[i] = m_avFrame->linesize[i];
    }
    return result;
}

std::vector<uint8_t*> VideoFrame::DataPointer() const {
    CheckIfDisposed();
    std::vector<uint8_t*> result(AV_NUM_DATA_POINTERS);
    for (int i = 0; i < AV_NUM_DATA_POINTERS; ++i) {
        result[i] = m_avFrame->data[i];
    }
    return result;
}

void VideoFrame::CheckIfDisposed() const {
    if (m_disposed || !m_avFrame) {
        throw std::runtime_error("The object was already disposed.");
    }
}

} // namespace MediaEncoder