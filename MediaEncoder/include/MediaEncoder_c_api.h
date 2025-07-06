
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// Forward declarations of opaque structs
typedef struct MediaWriterHandle MediaWriterHandle;
typedef struct VideoFrameHandle VideoFrameHandle;
typedef struct AudioFrameHandle AudioFrameHandle;

// Enum definitions for video codecs
typedef enum {
    VIDEO_CODEC_H264,
    VIDEO_CODEC_H265,
    // Add more codecs as needed
} VideoCodec;

// Enum definitions for audio codecs
typedef enum {
    AUDIO_CODEC_AAC,
    AUDIO_CODEC_MP3,
    // Add more codecs as needed
} AudioCodec;

/**
 * Creates a new MediaWriter instance.
 * @param filename The name of the file to create.
 * @param videoCodec The codec to use for video encoding.
 * @param audioCodec The codec to use for audio encoding.
 * @return A pointer to the MediaWriterHandle on success, NULL on failure.
 */
MediaWriterHandle* MediaWriter_Create(const char* filename, VideoCodec videoCodec, AudioCodec audioCodec);

/**
 * Opens the MediaWriter for writing.
 * @param writer Handle to the MediaWriter.
 * @return 0 on success, non-zero on failure.
 */
int MediaWriter_Open(MediaWriterHandle* writer);

/**
 * Closes the MediaWriter and finalizes the file.
 * @param writer Handle to the MediaWriter.
 * @return 0 on success, non-zero on failure.
 */
int MediaWriter_Close(MediaWriterHandle* writer);

/**
 * Encodes a video frame.
 * @param writer Handle to the MediaWriter.
 * @param frame Handle to the VideoFrame.
 * @return 0 on success, non-zero on failure.
 */
int MediaWriter_EncodeVideoFrame(MediaWriterHandle* writer, VideoFrameHandle* frame);

/**
 * Encodes an audio frame.
 * @param writer Handle to the MediaWriter.
 * @param frame Handle to the AudioFrame.
 * @return 0 on success, non-zero on failure.
 */
int MediaWriter_EncodeAudioFrame(MediaWriterHandle* writer, AudioFrameHandle* frame);

/**
 * Destroys the MediaWriter instance and frees resources.
 * @param writer Handle to the MediaWriter.
 */
void MediaWriter_Destroy(MediaWriterHandle* writer);

#ifdef __cplusplus
}
#endif