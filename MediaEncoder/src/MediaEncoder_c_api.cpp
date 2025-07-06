#include "MediaWriter.h"
#include "VideoFrame.h"
#include "AudioFrame.h"
#include "VideoCodec.h"
#include "AudioCodec.h"

#include <memory>
#include <string>
#include <cstdio>
#include <exception>

// Define handle
struct MediaWriterHandle {
    std::unique_ptr<MediaEncoder::MediaWriter> writer;
};

// Helper to convert enums to string names
std::string CodecToString(MediaEncoder::VideoCodec codec) {
    switch (codec) {
        case MediaEncoder::VideoCodec::H264: return "libx264";
        case MediaEncoder::VideoCodec::Hevc: return "libx265";
        case MediaEncoder::VideoCodec::Mpeg4: return "mpeg4";
        default: return "libx264"; // default fallback
    }
}

std::string CodecToString(MediaEncoder::AudioCodec codec) {
    switch (codec) {
        case MediaEncoder::AudioCodec::Aac: return "aac";
        case MediaEncoder::AudioCodec::Mp3: return "libmp3lame";
        default: return "aac"; // default fallback
    }
}

extern "C" {

MediaWriterHandle* MediaWriter_Create(
    int width,
    int height,
    int fps_num,
    int fps_den,
    int video_bitrate,
    MediaEncoder::VideoCodec videoCodec,
    int audio_bitrate,
    MediaEncoder::AudioCodec audioCodec
) {
    try {
        auto* handle = new MediaWriterHandle();
        handle->writer = std::make_unique<MediaEncoder::MediaWriter>(
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

int MediaWriter_EncodeVideoFrame(MediaWriterHandle* handle, MediaEncoder::VideoFrame* frame) {
    try {
        handle->writer->EncodeVideoFrame(frame);
        return 0;
    } catch (const std::exception& ex) {
        fprintf(stderr, "EncodeVideoFrame error: %s\n", ex.what());
        return -1;
    }
}

int MediaWriter_EncodeAudioFrame(MediaWriterHandle* handle, MediaEncoder::AudioFrame* frame) {
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