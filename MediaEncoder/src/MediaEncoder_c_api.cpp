#include "MediaWriter.h"
#include "VideoFrame.h"
#include "AudioFrame.h"
#include "VideoCodec.h"
#include "AudioCodec.h"

#include <memory>
#include <string>
#include <cstdio>
#include <exception>

using namespace MediaEncoder;

// Define handle
struct MediaWriterHandle {
    std::unique_ptr<MediaWriter> writer;
};

// Helper to convert enums to string names
std::string CodecToString(VideoCodec codec) {
    switch (codec) {
        case VideoCodec::H264: return "libx264";
        case VideoCodec::HEVC: return "libx265";
        case VideoCodec::MPEG4: return "mpeg4";
        default: return "libx264"; // fallback
    }
}

std::string CodecToString(AudioCodec codec) {
    const char* name = ToCodecName(codec); // use helper in AudioCodec.h
    return name ? name : "aac";  // default fallback
}

extern "C" {

MediaWriterHandle* MediaWriter_Create(
    int width,
    int height,
    int fps_num,
    int fps_den,
    int video_bitrate,
    VideoCodec videoCodec,
    int audio_bitrate,
    AudioCodec audioCodec
) {
    try {
        auto* handle = new MediaWriterHandle();
        handle->writer = std::make_unique<MediaWriter>(
            width,
            height,
            fps_num,
            fps_den,
            CodecToString(videoCodec),
            video_bitrate,
            CodecToString(audioCodec),
            audio_bitrate
        );
        return handle;
    } catch (const std::exception& ex) {
        fprintf(stderr, "MediaWriter_Create error: %s\n", ex.what());
        return nullptr;
    }
}

int MediaWriter_Open(MediaWriterHandle* handle, const char* filename, const char* format) {
    try {
        handle->writer->Open(filename, format);
        return 0;
    } catch (const std::exception& ex) {
        fprintf(stderr, "MediaWriter_Open error: %s\n", ex.what());
        return -1;
    }
}

int MediaWriter_EncodeVideoFrame(MediaWriterHandle* handle, VideoFrame* frame) {
    try {
        handle->writer->EncodeVideoFrame(frame);
        return 0;
    } catch (const std::exception& ex) {
        fprintf(stderr, "EncodeVideoFrame error: %s\n", ex.what());
        return -1;
    }
}

int MediaWriter_EncodeAudioFrame(MediaWriterHandle* handle, AudioFrame* frame) {
    try {
        handle->writer->EncodeAudioFrame(frame);
        return 0;
    } catch (const std::exception& ex) {
        fprintf(stderr, "EncodeAudioFrame error: %s\n", ex.what());
        return -1;
    }
}

int MediaWriter_Close(MediaWriterHandle* handle) {
    try {
        handle->writer->Close();
        return 0;
    } catch (const std::exception& ex) {
        fprintf(stderr, "MediaWriter_Close error: %s\n", ex.what());
        return -1;
    }
}

void MediaWriter_Destroy(MediaWriterHandle* handle) {
    delete handle;
}

} // extern "C"