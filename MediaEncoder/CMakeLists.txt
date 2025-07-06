cmake_minimum_required(VERSION 3.10)
project(MediaEncoder VERSION 1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Find FFmpeg using pkg-config
find_package(PkgConfig REQUIRED)
pkg_check_modules(FFMPEG REQUIRED IMPORTED_TARGET
    libavcodec
    libavformat
    libavutil
    libswscale
    libswresample
)

# Include directories
include_directories(include)
include_directories(${FFMPEG_INCLUDE_DIRS})

# Source files
file(GLOB SOURCES "src/*.cpp")
file(GLOB HEADERS "include/*.h")

# Create shared library
add_library(mediaencoder SHARED ${SOURCES} ${HEADERS})

# Link FFmpeg
target_link_libraries(mediaencoder PkgConfig::FFMPEG)

# Set output name
set_target_properties(mediaencoder PROPERTIES OUTPUT_NAME "mediaencoder")

# Verbose build
set(CMAKE_VERBOSE_MAKEFILE ON)