
#pragma once

extern "C" {
#include <libavutil/pixfmt.h> // Include FFmpeg pixel format definitions
}

namespace MediaEncoder
{
    enum class PixelFormat
    {
        None = AV_PIX_FMT_NONE,
        YUV420P = AV_PIX_FMT_YUV420P,
        YUYV422 = AV_PIX_FMT_YUYV422,
        RGB24 = AV_PIX_FMT_RGB24,
        BGR24 = AV_PIX_FMT_BGR24,
        YUV422P = AV_PIX_FMT_YUV422P,
        YUV444P = AV_PIX_FMT_YUV444P,
        YUV410P = AV_PIX_FMT_YUV410P,
        YUV411P = AV_PIX_FMT_YUV411P,
        UYVY422 = AV_PIX_FMT_UYVY422,
        YVYU422 = AV_PIX_FMT_YVYU422,
        UYYVYY411 = AV_PIX_FMT_UYYVYY411,
        ARGB = AV_PIX_FMT_ARGB,
        RGBA = AV_PIX_FMT_RGBA,
        ABGR = AV_PIX_FMT_ABGR,
        BGRA = AV_PIX_FMT_BGRA,
        NV12 = AV_PIX_FMT_NV12,
        NV21 = AV_PIX_FMT_NV21,
        D3D11VA_VLD = AV_PIX_FMT_D3D11VA_VLD,
        D3D11 = AV_PIX_FMT_D3D11,
    };
}