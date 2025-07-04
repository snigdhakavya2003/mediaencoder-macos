cpp
#pragma once

#include <string>
#include <vector>
#include <iostream> // For std::cout, if needed for debugging
#include <libavformat/avformat.h> // Include relevant FFmpeg headers

namespace MediaEncoder
{
    class MediaFormat
    {
    public:
        static std::vector<std::string> GetAllFormatLongNames()
        {
            // Dummy implementation - in a real scenario, this would query FFmpeg
            return {"MP4", "AVI", "MKV", "MOV"};
        }

        static std::string GetFormatExtensions(const std::string& format)
        {
            // Dummy implementation
            if (format == "MP4") return ".mp4";
            if (format == "AVI") return ".avi";
            if (format == "MKV") return ".mkv";
            if (format == "MOV") return ".mov";
            return "";
        }

        static std::string GetFormatLongName(const std::string& format)
        {
            // Dummy implementation
            if (format == "MP4") return "MPEG-4 Video";
            if (format == "AVI") return "Audio Video Interleave";
            if (format == "MKV") return "Matroska Video";
            if (format == "MOV") return "QuickTime Movie";
            return "Unknown Format";
        }
    }
}