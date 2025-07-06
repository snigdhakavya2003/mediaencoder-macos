
#include "MediaWriter.h"
#include <cstdio>
#include <cstdint>
#include <exception>
#include <libavutil/frame.h>
#include <libavutil/imgutils.h>

typedef void* MediaWriterHandle;

// Helper function to create an AVFrame from raw data
AVFrame* MakeAVFrameFromRawData(const uint8_t* const* data, const int* linesize, int width, int height, AVPixelFormat pixelFormat) {
    AVFrame* frame = av_frame_alloc();
    if (!frame) {
        fprintf(stderr, "Failed to allocate AVFrame\n");
        return nullptr;
    }
    frame->format = pixelFormat;
    frame->width = width;
    frame->height = height;

    if (av_frame_get_buffer(frame, 32) < 0) {
        av_frame_free(&frame);
        fprintf(stderr, "Failed to allocate frame buffer\n");
        return nullptr;
    }

    // Copy data into the frame
    for (int i = 0; i < 4; i++) {
        frame->data[i] = const_cast<uint8_t*>(data[i]);
        frame->linesize[i] = linesize[i];
    }

    return frame;
}

/**
 * Creates a new MediaWriter instance.
 * @param width The width of the video.
 * @param height The height of the video.
 * @param fps_num The numerator of the frame rate.
 * @param fps_den The denominator of the frame rate.
 * @param video_bitrate The video bitrate.
 * @param audio_bitrate The audio bitrate.
 * @return A pointer to the MediaWriterHandle on success, NULL on failure.
 */
MediaWriterHandle MediaWriter_Create(int width, int height, int fps_num, int fps_den, int video_bitrate, int audio_bitrate) {
    try {
        MediaWriter* writer = new MediaWriter(width, height, fps_num, fps_den, video_bitrate, audio_bitrate);
        return reinterpret_cast<MediaWriterHandle>(writer);
    } catch (const std::exception& e) {
        fprintf(stderr, "Error creating MediaWriter: %s\n", e.what());
        return nullptr;
    }
}

/**
 * Opens the MediaWriter for writing.
 * @param handle Handle to the MediaWriter.
 * @param url The URL of the media file to create.
 * @param format The format of the media file.
 * @param forceSoftwareEncoder Whether to force software encoding.
 * @return 0 on success, non-zero on failure.
 */
bool MediaWriter_Open(MediaWriterHandle handle, const char* url, const char* format, bool forceSoftwareEncoder) {
    if (!handle || !url || !format) {
        fprintf(stderr, "Invalid parameters passed to MediaWriter_Open.\n");
        return false;
    }
    try {
        MediaWriter* writer = reinterpret_cast<MediaWriter*>(handle);
        writer->Open(url, format, forceSoftwareEncoder);
        return true;
    } catch (const std::exception& e) {
        fprintf(stderr, "Error opening MediaWriter: %s\n", e.what());
        return false;
    }
}

/**
 * Encodes a video frame.
 * @param handle Handle to the MediaWriter.
 * @param data Pointer to the raw video data.
 * @param linesize Pointer to the line sizes of the video data.
 */
void MediaWriter_EncodeVideoFrame(MediaWriterHandle handle, const uint8_t* const* data, const int* linesize) {
    if (!handle || !data || !linesize) {
        fprintf(stderr, "Invalid parameters passed to MediaWriter_EncodeVideoFrame.\n");
        return;
    }
    try {
        MediaWriter* writer = reinterpret_cast<MediaWriter*>(handle);
        AVFrame* frame = MakeAVFrameFromRawData(data, linesize, writer->GetWidth(), writer->GetHeight(), AV_PIX_FMT_YUV420P);
        if (frame) {
            writer->EncodeVideoFrame(frame);
            av_frame_free(&frame);
        }
    } catch (const std::exception& e) {
        fprintf(stderr, "Error encoding video frame: %s\n", e.what());
    }
}

/**
 * Encodes an audio frame.
 * @param handle Handle to the MediaWriter.
 * @param data Pointer to the raw audio data.
 * @param sampleCount The number of samples in the audio frame.
 */
void MediaWriter_EncodeAudioFrame(MediaWriterHandle handle, const uint8_t* data, int sampleCount) {
    if (!handle || !data) {
        fprintf(stderr, "Invalid parameters passed to MediaWriter_EncodeAudioFrame.\n");
        return;
    }
    try {
        MediaWriter* writer = reinterpret_cast<MediaWriter*>(handle);
        writer->EncodeAudioFrame(data, sampleCount);
    } catch (const std::exception& e) {
        fprintf(stderr, "Error encoding audio frame: %s\n", e.what());
    }
}

/**
 * Closes the MediaWriter and finalizes the file.
 * @param handle Handle to the MediaWriter.
 */
void MediaWriter_Close(MediaWriterHandle handle) {
    if (!handle) return;
    try {
        MediaWriter* writer = reinterpret_cast<MediaWriter*>(handle);
        writer->Close();
    } catch (const std::exception& e) {
        fprintf(stderr, "Error closing MediaWriter: %s\n", e.what());
    }
}

/**
 * Destroys the MediaWriter instance and frees resources.
 * @param handle Handle to the MediaWriter.
 */
void MediaWriter_Destroy(MediaWriterHandle handle) {
    if (!handle) return;
    try {
        MediaWriter* writer = reinterpret_cast<MediaWriter*>(handle);
        delete writer;
    } catch (const std::exception& e) {
        fprintf(stderr, "Error destroying MediaWriter: %s\n", e.what());
    }
}