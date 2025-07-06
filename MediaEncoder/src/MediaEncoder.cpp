#include "MediaEncoder.h"

#include <vector>
#include <string>
#include <stdexcept>
#include <cstring>
#include <memory>

extern "C" {
#include <libavformat/avformat.h>
}

namespace MediaEncoder {

std::vector<std::string> MediaFormat::GetAllFormatLongNames()
{
    std::vector<std::string> ret;
    const AVOutputFormat* fmt = nullptr;
    void* i = nullptr;

    while ((fmt = av_muxer_iterate(&i)))
    {
        if (fmt->long_name) ret.emplace_back(fmt->long_name);
        else if (fmt->name) ret.emplace_back(fmt->name);
    }

    return ret;
}

std::string MediaFormat::GetFormatExtensions(const std::string& format)
{
    const AVOutputFormat* fmt = av_guess_format(format.c_str(), nullptr, nullptr);
    return fmt && fmt->extensions ? fmt->extensions : "";
}

std::string MediaFormat::GetFormatLongName(const std::string& format)
{
    const AVOutputFormat* fmt = av_guess_format(format.c_str(), nullptr, nullptr);
    return fmt && fmt->long_name ? fmt->long_name : "";
}

void MediaFormat::GetFormatInfo(const std::string& format, std::string& longName, std::string& extensions)
{
    const AVOutputFormat* fmt = av_guess_format(format.c_str(), nullptr, nullptr);
    if (fmt)
    {
        longName = fmt->long_name ? fmt->long_name : "";
        extensions = fmt->extensions ? fmt->extensions : "";
    }
    else
    {
        longName.clear();
        extensions.clear();
    }
}

} // namespace MediaEncoder