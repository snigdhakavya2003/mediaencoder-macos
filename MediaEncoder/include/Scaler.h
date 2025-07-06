
#pragma once

extern "C" {
#include <libswscale/swscale.h>
#include <libavutil/pixfmt.h>
#include <libavutil/imgutils.h>
}

#include <cstdint>
#include <stdexcept>
#include <vector>

class Scaler {
public:
    Scaler() : sws_ctx(nullptr), currentSrcW(0), currentSrcH(0), currentDstW(0), currentDstH(0),
               currentSrcFmt(AV_PIX_FMT_NONE), currentDstFmt(AV_PIX_FMT_NONE) {}

    ~Scaler() {
        ResetContext();
    }

    bool Convert(int srcW, int srcH, AVPixelFormat srcFormat,
                 int dstW, int dstH, AVPixelFormat dstFormat,
                 uint8_t* src, int srcStride,
                 uint8_t* dst, int dstStride) {
        if (!EnsureContext(srcW, srcH, srcFormat, dstW, dstH, dstFormat)) {
            throw std::runtime_error("Failed to ensure scaling context.");
        }

        // Perform the scaling operation
        sws_scale(sws_ctx, &src, &srcStride, 0, srcH, &dst, &dstStride);
        return true;
    }

    bool Convert(int srcW, int srcH, AVPixelFormat srcFormat,
                 int dstW, int dstH, AVPixelFormat dstFormat,
                 uint8_t* const srcData[4], const int srcStride[4],
                 uint8_t* const dstData[4], const int dstStride[4]) {
        if (!EnsureContext(srcW, srcH, srcFormat, dstW, dstH, dstFormat)) {
            throw std::runtime_error("Failed to ensure scaling context.");
        }

        // Perform the scaling operation
        sws_scale(sws_ctx, srcData, srcStride, 0, srcH, dstData, dstStride);
        return true;
    }

private:
    SwsContext* sws_ctx;
    int currentSrcW, currentSrcH, currentDstW, currentDstH;
    AVPixelFormat currentSrcFmt, currentDstFmt;

    void ResetContext() {
        if (sws_ctx) {
            sws_freeContext(sws_ctx);
            sws_ctx = nullptr;
        }
    }

    bool EnsureContext(int srcW, int srcH, AVPixelFormat srcFmt,
                       int dstW, int dstH, AVPixelFormat dstFmt) {
        if (currentSrcW != srcW || currentSrcH != srcH ||
            currentSrcFmt != srcFmt || currentDstW != dstW ||
            currentDstH != dstH || currentDstFmt != dstFmt) {

            ResetContext(); // Free existing context if parameters have changed

            currentSrcW = srcW;
            currentSrcH = srcH;
            currentSrcFmt = srcFmt;
            currentDstW = dstW;
            currentDstH = dstH;
            currentDstFmt = dstFmt;

            sws_ctx = sws_getContext(srcW, srcH, srcFmt,
                                      dstW, dstH, dstFmt,
                                      SWS_BILINEAR, nullptr, nullptr, nullptr);
            if (!sws_ctx) {
                throw std::runtime_error("Failed to create SwsContext.");
            }
        }
        return true;
    }
};