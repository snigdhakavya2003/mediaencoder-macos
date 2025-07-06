#pragma once

#include <vector>
#include <memory>
#include <stdexcept>

extern "C" {
#include <libavutil/frame.h>
#include <libavutil/imgutils.h>
}

namespace MediaEncoder {

class VideoFrame {
public:
    VideoFrame(int width, int height, AVPixelFormat pixelFormat);
    VideoFrame(const VideoFrame& other);
    VideoFrame(VideoFrame&& other) noexcept;
    ~VideoFrame();

    void Dispose();

    void FillFrame(const uint8_t* srcData, int srcStride);
    AVFrame* NativePointer() const;
    int Width() const;
    int Height() const;
    AVPixelFormat PixelFormat() const;
    std::vector<int> LineSize() const;
    std::vector<uint8_t*> DataPointer() const;

    static std::shared_ptr<VideoFrame> Create(int width, int height, AVPixelFormat format) {
        return std::make_shared<VideoFrame>(width, height, format);
    }

private:
    AVFrame* m_frame;
    bool m_disposed;

    void CheckIfDisposed() const;
};

} // namespace MediaEncoder