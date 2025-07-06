cmake_minimum_required(VERSION 3.18)
project(MediaEncoder CXX)

# Set C++ version
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Locate FFmpeg libraries manually (without pkg-config)
find_library(AVUTIL_LIB avutil REQUIRED)
find_library(AVCODEC_LIB avcodec REQUIRED)
find_library(AVFORMAT_LIB avformat REQUIRED)
find_library(SWSCALE_LIB swscale REQUIRED)
find_library(SWRESAMPLE_LIB swresample REQUIRED)

include_directories(
    ${CMAKE_SOURCE_DIR}
    /usr/local/include
    /opt/homebrew/include
)

link_directories(
    /usr/local/lib
    /opt/homebrew/lib
)

# Assuming sources are in MediaEncoder/
set(SOURCES
    MediaEncoder/src/AudioFrame.cpp
    MediaEncoder/src/MediaEncoder.cpp
    MediaEncoder/src/MediaFormat.cpp
    MediaEncoder/src/MediaWriter.cpp
    MediaEncoder/src/Resampler.cpp
    MediaEncoder/src/Scaler.cpp
    MediaEncoder/src/VideoFrame.cpp
    MediaEncoder/src/mediaencoder_c_api.cpp
)

set(HEADERS
    MediaEncoder/include/AudioFrame.h
    MediaEncoder/include/MediaEncoder.h
    MediaEncoder/include/MediaFormat.h
    MediaEncoder/include/MediaWriter.h
    MediaEncoder/include/Resampler.h
    MediaEncoder/include/Scaler.h
    MediaEncoder/include/SampleFormat.h
    MediaEncoder/include/VideoFrame.h
    MediaEncoder/include/AudioCodec.h
    MediaEncoder/include/VideoCodec.h
    MediaEncoder/include/PixelFormat.h
    MediaEncoder/include/mediaencoder_c_api.h
)
# Add library (static or shared)
add_library(mediaencoder STATIC ${SOURCES} ${HEADERS})

# Link FFmpeg libs
target_link_libraries(mediaencoder
    ${AVUTIL_LIB}
    ${AVCODEC_LIB}
    ${AVFORMAT_LIB}
    ${SWSCALE_LIB}
    ${SWRESAMPLE_LIB}
)

# If building a shared lib (optional alternative)
# add_library(mediaencoder SHARED ${SOURCES} ${HEADERS})

# Export headers if needed
target_include_directories(mediaencoder PUBLIC ${CMAKE_SOURCE_DIR})