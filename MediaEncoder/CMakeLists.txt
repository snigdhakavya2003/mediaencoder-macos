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

# Include and link directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    /usr/local/include
    /opt/homebrew/include
)

link_directories(
    /usr/local/lib
    /opt/homebrew/lib
)

# Collect all source and header files from src and include
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "include/*.h")

# Create the static library
add_library(mediaencoder STATIC ${SOURCES} ${HEADERS})

# Link FFmpeg libraries
target_link_libraries(mediaencoder
    ${AVUTIL_LIB}
    ${AVCODEC_LIB}
    ${AVFORMAT_LIB}
    ${SWSCALE_LIB}
    ${SWRESAMPLE_LIB}
)

# Set include directories for this target
target_include_directories(mediaencoder PUBLIC ${CMAKE_SOURCE_DIR}/include)

# Optional: enable verbose output during compilation
set(CMAKE_VERBOSE_MAKEFILE ON)