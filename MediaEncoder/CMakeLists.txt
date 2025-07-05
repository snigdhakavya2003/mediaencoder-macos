
cmake_minimum_required(VERSION 3.10)
project(MediaEncoder VERSION 1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Enable position-independent code for shared library
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Find FFmpeg using pkg-config
find_package(PkgConfig REQUIRED)
pkg_check_modules(FFMPEG REQUIRED IMPORTED_TARGET
    avcodec
    avformat
    avutil
    swscale
    swresample
)
target_link_libraries(MediaEncoder PkgConfig::FFMPEG)
# Specify the source files
set(SOURCES
    MediaWriter.cpp
    MediaEncoder.cpp
    AudioFrame.cpp
    VideoFrame.cpp
    Resampler.cpp
    Scaler.cpp
)

# Specify the header files
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

# Create the shared library
add_library(mediaencoder SHARED ${SOURCES} ${HEADERS})

# Set the output name
set_target_properties(mediaencoder PROPERTIES OUTPUT_NAME "mediaencoder")

# Link FFmpeg libraries
target_link_libraries(mediaencoder PkgConfig::FFMPEG)

# Include directories
target_include_directories(mediaencoder PRIVATE ${FFMPEG_INCLUDE_DIRS})

# Optional: Enable verbose output for debugging
set(CMAKE_VERBOSE_MAKEFILE ON)