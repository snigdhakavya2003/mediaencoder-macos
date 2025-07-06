cmake_minimum_required(VERSION 3.10)
project(MediaEncoder VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Set FFmpeg include and lib paths manually
set(FFMPEG_INCLUDE_DIR "/opt/homebrew/include")
set(FFMPEG_LIB_DIR "/opt/homebrew/lib")

include_directories(${FFMPEG_INCLUDE_DIR})
link_directories(${FFMPEG_LIB_DIR})

set(SOURCES
    MediaWriter.cpp
    MediaEncoder.cpp
    AudioFrame.cpp
    VideoFrame.cpp
    Resampler.cpp
    Scaler.cpp
)

set(HEADERS
    MediaWriter.h
    MediaEncoder.h
    AudioFrame.h
    VideoFrame.h
    Resampler.h
    Scaler.h
    PixelFormat.h
    SampleFormat.h
    AudioCodec.h
    VideoCodec.h
)

add_library(mediaencoder SHARED ${SOURCES} ${HEADERS})
set_target_properties(mediaencoder PROPERTIES OUTPUT_NAME "mediaencoder")

# Manually link FFmpeg libraries
target_link_libraries(mediaencoder
    avcodec
    avformat
    avutil
    swscale
    swresample
)

# Optional
set(CMAKE_VERBOSE_MAKEFILE ON)