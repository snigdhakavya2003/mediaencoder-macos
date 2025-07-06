#pragma once

#include <string>
#include <vector>
#include <iostream> // Optional: for debug logging

extern "C" {
#include <libavformat/avformat.h>
}

namespace MediaEncoder {

class MediaFormat {
public:
    static std::vector<std::string> GetAllFormatLongNames();

    static std::string GetFormatExtensions(const std::string& format);

    static std::string GetFormatLongName(const std::string& format);

    static void GetFormatInfo(const std::string& format, std::string& longName, std::string& extensions);
};

} // namespace MediaEncoder