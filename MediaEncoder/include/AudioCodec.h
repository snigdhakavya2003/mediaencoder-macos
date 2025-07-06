#pragma once

extern "C" {
#include <libavcodec/avcodec.h>
}

#include <string>

namespace MediaEncoder {

    enum class AudioCodec {
        None = AV_CODEC_ID_NONE,

        // PCM formats
        PCM_S16LE = AV_CODEC_ID_PCM_S16LE,
        PCM_S16BE = AV_CODEC_ID_PCM_S16BE,
        PCM_U16LE = AV_CODEC_ID_PCM_U16LE,
        PCM_U16BE = AV_CODEC_ID_PCM_U16BE,
        PCM_S8 = AV_CODEC_ID_PCM_S8,
        PCM_U8 = AV_CODEC_ID_PCM_U8,
        PCM_MULAW = AV_CODEC_ID_PCM_MULAW,
        PCM_ALAW = AV_CODEC_ID_PCM_ALAW,
        PCM_S32LE = AV_CODEC_ID_PCM_S32LE,
        PCM_S32BE = AV_CODEC_ID_PCM_S32BE,
        PCM_U32LE = AV_CODEC_ID_PCM_U32LE,
        PCM_U32BE = AV_CODEC_ID_PCM_U32BE,
        PCM_S24LE = AV_CODEC_ID_PCM_S24LE,
        PCM_S24BE = AV_CODEC_ID_PCM_S24BE,
        PCM_U24LE = AV_CODEC_ID_PCM_U24LE,
        PCM_U24BE = AV_CODEC_ID_PCM_U24BE,
        PCM_S24DAUD = AV_CODEC_ID_PCM_S24DAUD,
        PCM_ZORK = AV_CODEC_ID_PCM_ZORK,
        PCM_S16LE_PLANAR = AV_CODEC_ID_PCM_S16LE_PLANAR,
        PCM_DVD = AV_CODEC_ID_PCM_DVD,
        PCM_F32BE = AV_CODEC_ID_PCM_F32BE,
        PCM_F32LE = AV_CODEC_ID_PCM_F32LE,
        PCM_F64BE = AV_CODEC_ID_PCM_F64BE,
        PCM_F64LE = AV_CODEC_ID_PCM_F64LE,
        PCM_BLURAY = AV_CODEC_ID_PCM_BLURAY,
        PCM_LXF = AV_CODEC_ID_PCM_LXF,
        PCM_S8_PLANAR = AV_CODEC_ID_PCM_S8_PLANAR,
        PCM_S24LE_PLANAR = AV_CODEC_ID_PCM_S24LE_PLANAR,
        PCM_S32LE_PLANAR = AV_CODEC_ID_PCM_S32LE_PLANAR,
        PCM_S16BE_PLANAR = AV_CODEC_ID_PCM_S16BE_PLANAR,

        // ADPCM
        ADPCM_IMA_QT = AV_CODEC_ID_ADPCM_IMA_QT,
        ADPCM_IMA_WAV = AV_CODEC_ID_ADPCM_IMA_WAV,
        ADPCM_IMA_DK3 = AV_CODEC_ID_ADPCM_IMA_DK3,
        ADPCM_IMA_DK4 = AV_CODEC_ID_ADPCM_IMA_DK4,
        ADPCM_IMA_WS = AV_CODEC_ID_ADPCM_IMA_WS,
        ADPCM_IMA_SMJPEG = AV_CODEC_ID_ADPCM_IMA_SMJPEG,
        ADPCM_MS = AV_CODEC_ID_ADPCM_MS,
        ADPCM_4XM = AV_CODEC_ID_ADPCM_4XM,
        ADPCM_XA = AV_CODEC_ID_ADPCM_XA,
        ADPCM_ADX = AV_CODEC_ID_ADPCM_ADX,
        ADPCM_EA = AV_CODEC_ID_ADPCM_EA,
        ADPCM_G726 = AV_CODEC_ID_ADPCM_G726,
        ADPCM_CT = AV_CODEC_ID_ADPCM_CT,
        ADPCM_SWF = AV_CODEC_ID_ADPCM_SWF,
        ADPCM_YAMAHA = AV_CODEC_ID_ADPCM_YAMAHA,
        ADPCM_SBPRO_4 = AV_CODEC_ID_ADPCM_SBPRO_4,
        ADPCM_SBPRO_3 = AV_CODEC_ID_ADPCM_SBPRO_3,
        ADPCM_SBPRO_2 = AV_CODEC_ID_ADPCM_SBPRO_2,
        ADPCM_THP = AV_CODEC_ID_ADPCM_THP,
        ADPCM_IMA_AMV = AV_CODEC_ID_ADPCM_IMA_AMV,
        ADPCM_EA_R1 = AV_CODEC_ID_ADPCM_EA_R1,
        ADPCM_EA_R3 = AV_CODEC_ID_ADPCM_EA_R3,
        ADPCM_EA_R2 = AV_CODEC_ID_ADPCM_EA_R2,
        ADPCM_IMA_EA_SEAD = AV_CODEC_ID_ADPCM_IMA_EA_SEAD,
        ADPCM_IMA_EA_EACS = AV_CODEC_ID_ADPCM_IMA_EA_EACS,
        ADPCM_EA_XAS = AV_CODEC_ID_ADPCM_EA_XAS,

        // Other codecs
        AMR_NB = AV_CODEC_ID_AMR_NB,
        AMR_WB = AV_CODEC_ID_AMR_WB,
        RA_144 = AV_CODEC_ID_RA_144,
        RA_288 = AV_CODEC_ID_RA_288,

        MP1 = AV_CODEC_ID_MP1,
        MP2 = AV_CODEC_ID_MP2,
        MP3 = AV_CODEC_ID_MP3,
        MP3ADU = AV_CODEC_ID_MP3ADU,
        MP3ON4 = AV_CODEC_ID_MP3ON4,

        AAC = AV_CODEC_ID_AAC,
        AC3 = AV_CODEC_ID_AC3,
        EAC3 = AV_CODEC_ID_EAC3,
        DCA = AV_CODEC_ID_DTS,
        DTS = AV_CODEC_ID_DTS,

        FLAC = AV_CODEC_ID_FLAC,
        ALAC = AV_CODEC_ID_ALAC,
        OPUS = AV_CODEC_ID_OPUS,
        SPEEX = AV_CODEC_ID_SPEEX,
        APE = AV_CODEC_ID_APE,
        ATRAC3 = AV_CODEC_ID_ATRAC3,
        TRUEHD = AV_CODEC_ID_TRUEHD,
        MP4ALS = AV_CODEC_ID_MP4ALS,

        VORBIS = AV_CODEC_ID_VORBIS
    };

    // Optional: return encoder name for FFmpeg
    inline const char* ToCodecName(AudioCodec codec) {
        switch (codec) {
            case AudioCodec::MP3: return "libmp3lame";
            case AudioCodec::AAC: return "aac";
            case AudioCodec::FLAC: return "flac";
            case AudioCodec::OPUS: return "libopus";
            case AudioCodec::VORBIS: return "libvorbis";
            case AudioCodec::AC3: return "ac3";
            case AudioCodec::EAC3: return "eac3";
            case AudioCodec::ALAC: return "alac";
            case AudioCodec::PCM_S16LE: return "pcm_s16le";
            case AudioCodec::PCM_F32LE: return "pcm_f32le";
            default: return nullptr;
        }
    }

    inline AVCodecID ToAVCodecID(AudioCodec codec) {
        return static_cast<AVCodecID>(codec);
    }

    namespace BackwardCompatibility {
        constexpr AudioCodec MP3 = AudioCodec::MP3;
        constexpr AudioCodec AAC = AudioCodec::AAC;
        constexpr AudioCodec FLAC = AudioCodec::FLAC;
    }

} // namespace MediaEncoder