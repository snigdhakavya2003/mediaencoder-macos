#pragma once

extern "C" {
#include <libswscale/swscale.h>
#include <libavutil/pixfmt.h>
#include <libavutil/imgutils.h>
}

#include <cstdint>
#include <stdexcept>
#include <vector>

namespace MediaEncoder {

class Scaler {
public:
    Scaler();
    ~Scaler();

    bool Convert(int srcW, int srcH, AVPixelFormat srcFormat,
                 int dstW, int dstH, AVPixelFormat dstFormat,
                 uint8_t* src, int srcStride,
                 uint8_t* dst, int dstStride);

    bool Convert(int srcW, int srcH, AVPixelFormat srcFormat,
                 int dstW, int dstH, AVPixelFormat dstFormat,
                 uint8_t* const srcData[4], const int srcStride[4],
                 uint8_t* const dstData[4], const int dstStride[4]);

private:
    SwsContext* sws_ctx;
    int currentSrcW, currentSrcH, currentDstW, currentDstH;
    AVPixelFormat currentSrcFmt, currentDstFmt;

    void ResetContext();
    bool EnsureContext(int srcW, int srcH, AVPixelFormat srcFmt,
                       int dstW, int dstH, AVPixelFormat dstFmt);
};

} // namespace MediaEncoder