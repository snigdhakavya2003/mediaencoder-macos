#include "VideoFrame.h"

extern "C" {
#include <libavutil/imgutils.h>
#include <libavutil/frame.h>
}

namespace MediaEncoder {

VideoFrame::VideoFrame(int width, int height, AVPixelFormat pixelFormat)
    : m_disposed(false)
{
    m_frame = av_frame_alloc();
    if (!m_frame) {
        throw std::runtime_error("Failed to allocate AVFrame");
    }

    m_frame->width = width;
    m_frame->height = height;
    m_frame->format = pixelFormat;

    int ret = av_image_alloc(m_frame->data, m_frame->linesize, width, height, pixelFormat, 32);
    if (ret < 0) {
        av_frame_free(&m_frame);
        throw std::runtime_error("Failed to allocate raw picture buffer");
    }
}

VideoFrame::VideoFrame(const VideoFrame& other)
    : m_disposed(false)
{
    m_frame = av_frame_clone(other.m_frame);
    if (!m_frame) {
        throw std::runtime_error("Failed to clone AVFrame");
    }
}

VideoFrame::VideoFrame(VideoFrame&& other) noexcept
    : m_frame(other.m_frame), m_disposed(other.m_disposed)
{
    other.m_frame = nullptr;
    other.m_disposed = true;
}

VideoFrame::~VideoFrame()
{
    Dispose();
}

void VideoFrame::Dispose()
{
    if (!m_disposed) {
        if (m_frame) {
            av_freep(&m_frame->data[0]);
            av_frame_free(&m_frame);
        }
        m_disposed = true;
    }
}

void VideoFrame::FillFrame(const uint8_t* srcData, int srcStride)
{
    CheckIfDisposed();
    av_image_fill_arrays(m_frame->data, m_frame->linesize, srcData,
                         static_cast<AVPixelFormat>(m_frame->format),
                         m_frame->width, m_frame->height, 1);
}

AVFrame* VideoFrame::NativePointer() const {
    CheckIfDisposed();
    return m_frame;
}

int VideoFrame::Width() const {
    CheckIfDisposed();
    return m_frame->width;
}

int VideoFrame::Height() const {
    CheckIfDisposed();
    return m_frame->height;
}

AVPixelFormat VideoFrame::PixelFormat() const {
    CheckIfDisposed();
    return static_cast<AVPixelFormat>(m_frame->format);
}

std::vector<int> VideoFrame::LineSize() const {
    CheckIfDisposed();
    std::vector<int> result(AV_NUM_DATA_POINTERS);
    for (int i = 0; i < AV_NUM_DATA_POINTERS; ++i) {
        result[i] = m_frame->linesize[i];
    }
    return result;
}

std::vector<uint8_t*> VideoFrame::DataPointer() const {
    CheckIfDisposed();
    std::vector<uint8_t*> result(AV_NUM_DATA_POINTERS);
    for (int i = 0; i < AV_NUM_DATA_POINTERS; ++i) {
        result[i] = m_frame->data[i];
    }
    return result;
}

void VideoFrame::CheckIfDisposed() const {
    if (m_disposed || !m_frame) {
        throw std::runtime_error("The object was already disposed.");
    }
}

} // namespace MediaEncoder