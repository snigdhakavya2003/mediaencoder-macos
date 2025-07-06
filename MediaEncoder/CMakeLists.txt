cmake_minimum_required(VERSION 3.16)

project(MediaEncoder)

# Require C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Optional: verbose output during build
set(CMAKE_VERBOSE_MAKEFILE ON)

# Include directories
include_directories(
    ${PROJECT_SOURCE_DIR}/include
    /usr/local/include
    /opt/homebrew/include
)

# Source files
file(GLOB_RECURSE SOURCES
    ${PROJECT_SOURCE_DIR}/src/*.cpp
)

# Create the library
add_library(mediaencoder SHARED ${SOURCES})

# FFmpeg libraries to link
find_package(PkgConfig REQUIRED)
pkg_check_modules(FFMPEG REQUIRED IMPORTED_TARGET
    libavcodec
    libavformat
    libavutil
    libswscale
    libswresample
)

# Link FFmpeg
target_link_libraries(mediaencoder PRIVATE
    PkgConfig::FFMPEG
)

# If pkg-config is not reliable, fall back to explicit linking:
# target_link_libraries(mediaencoder PRIVATE
#     avcodec
#     avformat
#     avutil
#     swscale
#     swresample
# )

# Output to /build
set_target_properties(mediaencoder PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)