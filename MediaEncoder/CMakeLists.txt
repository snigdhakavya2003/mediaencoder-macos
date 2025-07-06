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

# CMakeLists.txt

# Specify the source files
set(SOURCES
    MediaEncoder-src/MediaWriter.cpp
    MediaEncoder-src/MediaEncoder.cpp
    MediaEncoder-src/AudioFrame.cpp
    MediaEncoder-src/VideoFrame.cpp
    MediaEncoder-src/Resampler.cpp
    MediaEncoder-src/Scaler.cpp
)

# Specify the header files
set(HEADERS
    MediaEncoder-include/MediaWriter.h
    MediaEncoder-include/MediaEncoder.h
    MediaEncoder-include/AudioFrame.h
    MediaEncoder-include/VideoFrame.h
    MediaEncoder-include/Resampler.h
    MediaEncoder-include/Scaler.h
    MediaEncoder-include/PixelFormat.h
    MediaEncoder-inclue/SampleFormat.h
    MediaEncoder-include/AudioCodec.h
    MediaEncoder-include/VideoCodec.h
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