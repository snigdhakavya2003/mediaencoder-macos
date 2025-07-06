
#pragma once

extern "C" {
#include <libavcodec/avcodec.h> // Include FFmpeg codec definitions
}

namespace MediaEncoder {

    // Enum class for audio codecs
    enum class AudioCodec {
        None = AV_CODEC_ID_NONE,                  ///< No codec
        Default = AV_CODEC_ID_DEFAULT,            ///< Default codec
        Pcm16 = AV_CODEC_ID_PCM_S16LE,            ///< Pulse Code Modulation (16-bit)
        AmrNarrowBand = AV_CODEC_ID_AMR_NB,       ///< Adaptive Multi-Rate Narrowband
        AmrWideBand = AV_CODEC_ID_AMR_WB,         ///< Adaptive Multi-Rate Wideband
        Ra144 = AV_CODEC_ID_RA_144,               ///< RealAudio 1.0
        Ra288 = AV_CODEC_ID_RA_288,               ///< RealAudio 2.0
        Mp2 = AV_CODEC_ID_MP2,                     ///< MPEG Audio Layer II
        Mp3 = AV_CODEC_ID_MP3,                     ///< MPEG Audio Layer III
        Aac = AV_CODEC_ID_AAC,                     ///< Advanced Audio Coding
        Ac3 = AV_CODEC_ID_AC3,                     ///< Dolby Digital AC-3
        Dts = AV_CODEC_ID_DTS,                     ///< Digital Theater Systems
        Vorbis = AV_CODEC_ID_VORBIS,               ///< Vorbis Audio Codec
        DvAudio = AV_CODEC_ID_DVAUDIO,             ///< DV Audio
        WmaV1 = AV_CODEC_ID_WMAV1,                 ///< Windows Media Audio 1
        WmaV2 = AV_CODEC_ID_WMAV2,                 ///< Windows Media Audio 2
        Mace3 = AV_CODEC_ID_MACE3,                 ///< MACE 3:1
        Mace6 = AV_CODEC_ID_MACE6,                 ///< MACE 6:1
        VMdAudio = AV_CODEC_ID_VMDAUDIO,           ///< VMD Audio
        Flac = AV_CODEC_ID_FLAC,                   ///< Free Lossless Audio Codec
        Mp3Adu = AV_CODEC_ID_MP3ADU,               ///< MP3 ADU
        Mp3On4 = AV_CODEC_ID_MP3ON4,               ///< MP3 on 4
        Shorten = AV_CODEC_ID_SHORTEN,             ///< Shorten
        Alac = AV_CODEC_ID_ALAC,                   ///< Apple Lossless Audio Codec
        WestwoodSnd1 = AV_CODEC_ID_WESTWOOD_SND1, ///< Westwood Audio 1
        Gsm = AV_CODEC_ID_GSM,                     ///< GSM
        Qdm2 = AV_CODEC_ID_QDM2,                   ///< QDM2
        Cook = AV_CODEC_ID_COOK,                   ///< Cook
        Truespeech = AV_CODEC_ID_TRUESPEECH,      ///< TrueSpeech
        Tta = AV_CODEC_ID_TTA,                     ///< TTA
        SmackAudio = AV_CODEC_ID_SMACKAUDIO,      ///< Smack Audio
        Qcelp = AV_CODEC_ID_QCELP,                 ///< QCELP
        Wavpack = AV_CODEC_ID_WAVPACK,             ///< WavPack
        DsiCinAudio = AV_CODEC_ID_DSICINAUDIO,     ///< DSI Cinema Audio
        Imc = AV_CODEC_ID_IMC,                     ///< IMC
        Musepack7 = AV_CODEC_ID_MUSEPACK7,         ///< Musepack 7
        Mlp = AV_CODEC_ID_MLP,                     ///< MLP
        GsmMs = AV_CODEC_ID_GSM_MS,                ///< GSM MS
        Atrac3 = AV_CODEC_ID_ATRAC3,               ///< ATRAC3
        Ape = AV_CODEC_ID_APE,                     ///< APE
        Nellymoser = AV_CODEC_ID_NELLYMOSER,       ///< Nellymoser
        Musepack8 = AV_CODEC_ID_MUSEPACK8,         ///< Musepack 8
        Speex = AV_CODEC_ID_SPEEX,                 ///< Speex
        Wmavoice = AV_CODEC_ID_WMAVOICE,           ///< Windows Media Audio Voice
        WmaPro = AV_CODEC_ID_WMAPRO,               ///< Windows Media Audio Pro
        WmaLossless = AV_CODEC_ID_WMALOSSLESS,     ///< Windows Media Audio Lossless
        Trac3p = AV_CODEC_ID_TRAC3P,               ///< ATRAC3+
        Eac3 = AV_CODEC_ID_EAC3,                   ///< Enhanced AC-3
        Sipr = AV_CODEC_ID_SIPR,                   ///< SIPR
        Mp1 = AV_CODEC_ID_MP1,                     ///< MPEG Audio Layer I
        TwinVQ = AV_CODEC_ID_TWINVQ,               ///< TwinVQ
        TrueHD = AV_CODEC_ID_TRUEHD,               ///< TrueHD
        MP4Als = AV_CODEC_ID_MP4ALS,               ///< MP4 ALS
        Atrac1 = AV_CODEC_ID_ATRAC1,               ///< ATRAC1
        BinkAudioRdft = AV_CODEC_ID_BINKAUDIO_RDFT,///< Bink Audio RDFT
        BinkAudioDct = AV_CODEC_ID_BINKAUDIO_DCT,  ///< Bink Audio DCT
        AacLatm = AV_CODEC_ID_AAC_LATM,            ///< AAC LATM
        Qdmc = AV_CODEC_ID_QDMC,                   ///< QDMC
        Celt = AV_CODEC_ID_CELT,                   ///< CELT
        G723_1 = AV_CODEC_ID_G723_1,               ///< G723.1
        G729 = AV_CODEC_ID_G729,                   ///< G.729
        EightSvxExp = AV_CODEC_ID_EIGHTSVX_EXP,   ///< 8SVX EXP
        EightSvxFib = AV_CODEC_ID_EIGHTSVX_FIB,   ///< 8SVX FIB
        BmvAudio = AV_CODEC_ID_BMV_AUDIO,          ///< BMV Audio
        Ralf = AV_CODEC_ID_RALF,                   ///< RALF
        Iac = AV_CODEC_ID_IAC,                     ///< IAC
        Ilbc = AV_CODEC_ID_ILBC,                   ///< iLBC
        Opus = AV_CODEC_ID_OPUS,                   ///< Opus
        ComfortNoise = AV_CODEC_ID_COMFORT_NOISE,  ///< Comfort Noise
        Tak = AV_CODEC_ID_TAK,                     ///< TAK
        MetaSound = AV_CODEC_ID_METASOUND,         ///< MetaSound
        PafAudio = AV_CODEC_ID_PAF_AUDIO,          ///< PAF Audio
        On2Avc = AV_CODEC_ID_ON2AVC,               ///< On2 AVC
        DsSsp = AV_CODEC_ID_DSSSP,                 ///< DSSS
        FfwaveSynth = AV_CODEC_ID_FFWAVESYNTH,     ///< FF Wave Synth
        Sonic = AV_CODEC_ID_SONIC,                 ///< Sonic
        SonicLossless = AV_CODEC_ID_SONIC_LOSSLESS,///< Sonic Lossless
        Evrc = AV_CODEC_ID_EVRC,                   ///< EVRC
        Smv = AV_CODEC_ID_SMV,                     ///< SMV
        DsdLsbf = AV_CODEC_ID_DSD_LSBF,            ///< DSD LSBF
        DsdMsbf = AV_CODEC_ID_DSD_MSBF,            ///< DSD MSBF
        DsdLsbfPlanar = AV_CODEC_ID_DSD_LSBF_PLANAR, ///< DSD LSBF Planar
        DsdMsbfPlanner = AV_CODEC_ID_DSD_MSBF_PLANAR, ///< DSD MSBF Planar
        FourGv = AV_CODEC_ID_FOURGV,               ///< 4GV
        InterplayAcm = AV_CODEC_ID_INTERPLAY_ACM,  ///< Interplay ACM
        Xma1 = AV_CODEC_ID_XMA1,                   ///< XMA1
        Xma2 = AV_CODEC_ID_XMA2,                   ///< XMA2
        Dst = AV_CODEC_ID_DST                       ///< DST
    };

#pragma region Backward Compatibility
    // Aliases for backward compatibility
    enum class BackwardCompatibility {
        MP3 = AudioCodec::Mp3,
        AAC = AudioCodec::Aac,
        M4A = AudioCodec::MP4Als,
        MP4ALS = AudioCodec::MP4Als
        // Add more aliases as needed
    };
#pragma endregion

} // namespace MediaEncoder