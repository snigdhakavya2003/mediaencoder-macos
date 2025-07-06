#include "MediaWriter.h"
#include <libavutil/frame.h>
#include <libavutil/imgutils.h>
#include <cstdio>
#include <exception>
#include <string>
#include <memory>
#include <cstring>

using namespace MediaEncoder;

struct MediaWriterHandle {
    std::unique_ptr<MediaWriter> writer;
};

// C API function implementations

extern "C" {

MediaWriterHandle* MediaWriter_Create(
    const char* filename,
    const char* format,
    int width,
    int height,
    int fps,
    VideoCodec videoCodec,
    int videoBitrate,
    AudioCodec audioCodec,
    int audioBitrate)
{
    if (!filename || !format) return nullptr;

    try {
        auto handle = new MediaWriterHandle();
        handle->writer = std::make_unique<MediaWriter>(
            width, height,
            fps, 1,  // timebase: fps/1
            static_cast<AVCodecID>(videoCodec),
            videoBitrate,
            static_cast<AVCodecID>(audioCodec),
            audioBitrate
        );
        handle->writer->Open(filename, format, false);  // false = don't force software
        return handle;
    } catch (const std::exception& ex) {
        fprintf(stderr, "MediaWriter_Create failed: %s\n", ex.what());
        return nullptr;
    }
}

int MediaWriter_Open(MediaWriterHandle* handle) {
    if (!handle || !handle->writer) return -1;
    // Already opened in Create
    return 0;
}

int MediaWriter_EncodeVideoFrame(MediaWriterHandle* handle, VideoFrameHandle* frame) {
    if (!handle || !frame || !handle->writer) return -1;
    try {
        handle->writer->EncodeVideoFrame(reinterpret_cast<AVFrame*>(frame));
        return 0;
    } catch (const std::exception& e) {
        fprintf(stderr, "EncodeVideoFrame failed: %s\n", e.what());
        return -1;
    }
}

int MediaWriter_EncodeAudioFrame(MediaWriterHandle* handle, AudioFrameHandle* frame) {
    if (!handle || !frame || !handle->writer) return -1;
    try {
        handle->writer->EncodeAudioFrame(reinterpret_cast<AVFrame*>(frame));
        return 0;
    } catch (const std::exception& e) {
        fprintf(stderr, "EncodeAudioFrame failed: %s\n", e.what());
        return -1;
    }
}

int MediaWriter_Close(MediaWriterHandle* handle) {
    if (!handle || !handle->writer) return -1;
    try {
        handle->writer->Close();
        return 0;
    } catch (const std::exception& e) {
        fprintf(stderr, "MediaWriter_Close failed: %s\n", e.what());
        return -1;
    }
}

void MediaWriter_Destroy(MediaWriterHandle* handle) {
    if (!handle) return;
    delete handle;
}

} // extern "C"