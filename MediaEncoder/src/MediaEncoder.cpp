
#include "MediaEncoder.h"
#include <vector>
#include <string>
#include <stdexcept>
#include <cstring>
#include <memory>
extern "C" {
    #include <libavformat/avformat.h>
    #include <libavcodec/avcodec.h>
    #include <libswscale/swscale.h>
    #include <libswresample/swresample.h>
    #include <libavutil/opt.h>
    #include <libavutil/imgutils.h>
    #include <libavutil/channel_layout.h>
}

namespace MediaEncoder
{
    std::vector<std::string> MediaFormat::GetAllFormatLongNames()
    {
        std::vector<std::string> ret;
        const AVOutputFormat* fmt = nullptr;
        void* i = nullptr;

        while ((fmt = av_muxer_iterate(&i)))
        {
            ret.emplace_back(fmt->long_name);
            ret.emplace_back(fmt->name);
        }
        return ret;
    }

    std::string MediaFormat::GetFormatExtensions(const std::string& format)
    {
        const AVOutputFormat* fmt = av_guess_format(format.c_str(), nullptr, nullptr);
        return fmt ? fmt->extensions : "";
    }

    std::string MediaFormat::GetFormatLongName(const std::string& format)
    {
        const AVOutputFormat* fmt = av_guess_format(format.c_str(), nullptr, nullptr);
        return fmt ? fmt->long_name : "";
    }

    void MediaFormat::GetFormatInfo(const std::string& format, std::string& longName, std::string& extensions)
    {
        const AVOutputFormat* fmt = av_guess_format(format.c_str(), nullptr, nullptr);
        if (fmt)
        {
            longName = fmt->long_name;
            extensions = fmt->extensions;
        }
        else
        {
            longName.clear();
            extensions.clear();
        }
    }
} // namespace MediaEncoder