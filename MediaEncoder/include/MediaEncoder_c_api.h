#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// Forward declarations of opaque structs
typedef struct MediaWriterHandle MediaWriterHandle;
typedef struct VideoFrameHandle VideoFrameHandle;
typedef struct AudioFrameHandle AudioFrameHandle;

// Video codec enumeration
typedef enum {
    VIDEO_CODEC_NONE = 0,
    VIDEO_CODEC_H264,
    VIDEO_CODEC_H265,
    VIDEO_CODEC_MPEG4,
    VIDEO_CODEC_VP8,
    VIDEO_CODEC_VP9
    // Add more if needed (matches MediaEncoder::VideoCodec)
} VideoCodec;

// Audio codec enumeration
typedef enum {
    AUDIO_CODEC_NONE = 0,
    AUDIO_CODEC_AAC,
    AUDIO_CODEC_MP3,
    AUDIO_CODEC_OPUS,
    AUDIO_CODEC_PCM_S16LE
    // Add more if needed (matches MediaEncoder::AudioCodec)
} AudioCodec;

/**
 * Creates and initializes a new MediaWriter instance.
 * 
 * @param filename        Output file path.
 * @param format          Format name (e.g., "mp4", "mov").
 * @param width           Video width.
 * @param height          Video height.
 * @param fps             Frames per second.
 * @param videoCodec      Video codec to use.
 * @param videoBitrate    Video bitrate in bps.
 * @param audioCodec      Audio codec to use.
 * @param audioBitrate    Audio bitrate in bps.
 * @return                Handle to the MediaWriter, or NULL on failure.
 */
MediaWriterHandle* MediaWriter_Create(
    const char* filename,
    const char* format,
    int width,
    int height,
    int fps,
    VideoCodec videoCodec,
    int videoBitrate,
    AudioCodec audioCodec,
    int audioBitrate
);

/**
 * Opens the MediaWriter. Must be called before encoding frames.
 * 
 * @param writer    MediaWriter handle.
 * @return          0 on success, non-zero on error.
 */
int MediaWriter_Open(MediaWriterHandle* writer);

/**
 * Encodes a video frame.
 * 
 * @param writer    MediaWriter handle.
 * @param frame     VideoFrame handle.
 * @return          0 on success, non-zero on error.
 */
int MediaWriter_EncodeVideoFrame(MediaWriterHandle* writer, VideoFrameHandle* frame);

/**
 * Encodes an audio frame.
 * 
 * @param writer    MediaWriter handle.
 * @param frame     AudioFrame handle.
 * @return          0 on success, non-zero on error.
 */
int MediaWriter_EncodeAudioFrame(MediaWriterHandle* writer, AudioFrameHandle* frame);

/**
 * Closes and finalizes the media file.
 * 
 * @param writer    MediaWriter handle.
 * @return          0 on success, non-zero on error.
 */
int MediaWriter_Close(MediaWriterHandle* writer);

/**
 * Frees the MediaWriter instance and associated resources.
 * 
 * @param writer    MediaWriter handle to free.
 */
void MediaWriter_Destroy(MediaWriterHandle* writer);

#ifdef __cplusplus
}
#endif