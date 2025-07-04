cpp
#include <stdexcept>
#include <cstdint>
#include <array>
#include <libswscale/swscale.h>
#include <libavutil/pixfmt.h>

namespace MediaEncoder {

class Scaler {
public:
    Scaler() : sws_ctx(nullptr), srcW(0), srcH(0), dstW(0), dstH(0), disposed(false) {}

    ~Scaler() {
        if (sws_ctx) {
            sws_freeContext(sws_ctx);
        }
    }

    bool Convert(int srcW, int srcH, AVPixelFormat srcFormat, int dstW, int dstH, AVPixelFormat dstFormat,
                 uint8_t* src, int srcStride, uint8_t* dst, int dstStride) {
        if (this->srcW != srcW || this->srcH != srcH || this->srcFormat != srcFormat ||
            this->dstW != dstW || this->dstH != dstH || this->dstFormat != dstFormat) {
            if (sws_ctx) {
                sws_freeContext(sws_ctx);
                sws_ctx = nullptr;
            }

            this->srcW = srcW;
            this->srcH = srcH;
            this->srcFormat = srcFormat;
            this->dstW = dstW;
            this->dstH = dstH;
            this->dstFormat = dstFormat;

            if (this->srcW > 0 && this->srcH > 0 && this->dstW > 0 && this->dstH > 0) {
                sws_ctx = sws_getContext(this->srcW, this->srcH, this->srcFormat,
                                          this->dstW, this->dstH, this->dstFormat,
                                          SWS_FAST_BILINEAR, nullptr, nullptr, nullptr);
                if (!sws_ctx) {
                    throw std::runtime_error("Failed to create SwsContext");
                }
            }
        }

        if (sws_ctx) {
            uint8_t* srcData[1] = { src };
            int srcLinesize[1] = { srcStride };
            uint8_t* dstData[1] = { dst };
            int dstLinesize[1] = { dstStride };

            sws_scale(sws_ctx, srcData, srcLinesize, 0, srcH, dstData, dstLinesize);
            return true;
        }
        return false;
    }

    bool Convert(int srcW, int srcH, AVPixelFormat srcFormat, int dstW, int dstH, AVPixelFormat dstFormat,
                 std::array<uint8_t*, 4> src, std::array<int, 4> srcStride,
                 std::array<uint8_t*, 4> dst, std::array<int, 4> dstStride) {
        if (this->srcW != srcW || this->srcH != srcH || this->srcFormat != srcFormat ||
            this->dstW != dstW || this->dstH != dstH || this->dstFormat != dstFormat) {
            if (sws_ctx) {
                sws_freeContext(sws_ctx);
                sws_ctx = nullptr;
            }

            this->srcW = srcW;
            this->srcH = srcH;
            this->srcFormat = srcFormat;
            this->dstW = dstW;
            this->dstH = dstH;
            this->dstFormat = dstFormat;

            if (this->srcW > 0 && this->srcH > 0 && this->dstW > 0 && this->dstH > 0) {
                sws_ctx = sws_getContext(this->srcW, this->srcH, this->srcFormat,
                                          this->dstW, this->dstH, this->dstFormat,
                                          SWS_FAST_BILINEAR, nullptr, nullptr, nullptr);
                if (!sws_ctx) {
                    throw std::runtime_error("Failed to create SwsContext");
                }
            }
        }

        if (sws_ctx) {
            uint8_t* srcData[4];
            int srcLinesize[4];
            uint8_t* dstData[4];
            int dstLinesize[4];

            for (int i = 0; i < 4; i++) {
                srcData[i] = (i < src.size()) ? src[i] : nullptr;
                srcLinesize[i] = (i < srcStride.size()) ? srcStride[i] : 0;

                dstData[i] = (i < dst.size()) ? dst[i] : nullptr;
                dstLinesize[i] = (i < dstStride.size()) ? dstStride[i] : 0;
            }

            sws_scale(sws_ctx, srcData, srcLinesize, 0, srcH, dstData, dstLinesize);
            return true;
        }
        return false;
    }

private:
    SwsContext* sws_ctx;
    int srcW, srcH, dstW, dstH;
    AVPixelFormat srcFormat, dstFormat;
    bool disposed;
};

} // namespace MediaEncoder
