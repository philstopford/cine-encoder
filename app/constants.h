/***********************************************************************

                          C I N E   E N C O D E R
                                JULY, 2020
                            COPYRIGHT (C) 2020

 FILE: constants.cpp
 COMMENT:
 LICENSE: GNU General Public License v3.0

***********************************************************************/

#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QSettings>
#include <QDir>
#include <QVector>
#include <iostream>

// Logging utility - prefer using Logger class for production code
#define Print(a) std::cout << a << std::endl

// Convert number to QString - using inline function for type safety
inline QString numToStr(int num) { return QString::number(num); }
inline QString numToStr(double num) { return QString::number(num); }
inline QString numToStr(float num) { return QString::number(num); }
inline QString numToStr(long num) { return QString::number(num); }

// Settings paths - consider using ConfigurationManager for better encapsulation
#define SETTINGSPATH QDir::homePath() + QString("/CineEncoder")
#define SETTINGSFILE (SETTINGSPATH + QString("/settings.ini"))
#define XMLSETTINGSFILE (SETTINGSPATH + QString("/settings.xml"))
#define SETTINGS(settings) QSettings settings(SETTINGSFILE, \
                            QSettings::IniFormat);

// ******************* Constants *****************************//
constexpr int PRESETS_VERSION = 202605;
constexpr int SETTINGS_VERSION = 202602;

constexpr int PARAMETERS_COUNT = 45;
constexpr int NUMBER_PRESETS = 31;

constexpr int CODEC_QSV_FIRST = 6;
constexpr int CODEC_QSV_LAST = 11;
constexpr int CODEC_VAAPI_FIRST = 12;
constexpr int CODEC_VAAPI_LAST = 12;

// Default values - using inline functions for QString construction
inline QString defaultPrefix() { return QString("output"); }
inline QString defaultSuffix() { return QString("_encoded_"); }
inline QString defaultSubtitleColor() { return QString("#00FFFFFF"); }
inline QString defaultSubtitleBackgroundColor() { return QString("#96000000"); }

constexpr int DEFAULTTIMER = 30;
constexpr int DOCKS_COUNT = 8;

constexpr int AMOUNT_HDR_PARAMS = 11;
constexpr int AMOUNT_VIDEO_METADATA = 6;
constexpr int MAX_AUDIO_STREAMS = 100;
constexpr int MAX_SUBTITLES = 100;

constexpr float MAXIMUM_ALLOWED_TIME = 359999.0f;
constexpr int FONTSIZE = 8;


namespace Constants {
    enum ffmpeg_prios {lowest, low, normal, high, highest};

    // ***************** Table Widget Columns *********************//
    enum ColumnIndex {
        WARNING,    FILENAME,   FORMAT,     RESOLUTION,    DURATION,     
        FPS,        AR,         STATUS,     PRESET_COL,    BITRATE,      
        SUBSAMPLING,BITDEPTH,   COLORSPACE, COLORRANGE,    COLORPRIM,    
        COLORMATRIX,TRANSFER,   MAXLUM,     MINLUM,        MAXCLL,       
        MAXFALL,    MASTERDISPLAY, PATH,    T_DUR,         T_CHROMACOORD, 
        T_WHITECOORD,T_STREAMSIZE,  T_WIDTH, T_HEIGHT,     T_STARTTIME,  
        T_ENDTIME,  T_ID
    };

    // **************** Geometry and Themes *********************//
    enum DockIndex {
        SOURCE_DOCK, PRESETS_DOCK,  OUTPUT_DOCK, STREAMS_DOCK,
        LOG_DOCK,    METADATA_DOCK, SPLIT_DOCK,  BROWSER_DOCK
    };

    enum Theme {
        LIGHT, DARK, WAVE, GRAY
    };

    enum PreviewDest {
        PREVIEW = 1, SPLITTER = 2
    };

    enum PreviewRes {
        RES_LOW, RES_HIGH
    };

    // ******************* Stream Data ***************************//
    enum class ContentType : uchar {
        Audio, Subtitle
    };

    struct StreamData {
        ContentType cont_type;
        QString input_file;
        QString output_file;
        QString title;  // Stream title for metadata export
        float duration;
        int stream;
        struct AudioTrackProcessing {
            bool enabled = false;
            QStringList filters;
            double delayMs = 0.0;
            double tempo = 1.0;
            double cropStartSeconds = 0.0;
            double cropEndSeconds = 0.0;
            bool isActive() const { return enabled && (!filters.isEmpty() || delayMs != 0.0 || tempo != 1.0 || cropStartSeconds != 0.0 || cropEndSeconds != 0.0); }
            QString filterChain() const {
                QStringList out;
                if (delayMs > 0.0) out << QString("adelay=%1:all=1").arg(qRound(delayMs));
                else if (delayMs < 0.0) out << QString("atrim=start=%1").arg(QString::number(-delayMs / 1000.0, 'f', 3));
                if (cropStartSeconds > 0.0 || cropEndSeconds > 0.0) {
                    QString f = "atrim=";
                    if (cropStartSeconds > 0.0) f += QString("start=%1").arg(cropStartSeconds, 0, 'f', 3);
                    if (cropEndSeconds > 0.0) f += QString("%1end=%2").arg(cropStartSeconds > 0.0 ? ":" : "").arg(cropEndSeconds, 0, 'f', 3);
                    out << f << "asetpts=PTS-STARTPTS";
                }
                if (tempo > 0.0 && tempo != 1.0) {
                    double value = tempo;
                    while (value > 2.0) { out << "atempo=2.0"; value /= 2.0; }
                    while (value < 0.5) { out << "atempo=0.5"; value /= 0.5; }
                    out << QString("atempo=%1").arg(value, 0, 'f', 6);
                }
                out << filters;
                return out.join(',');
            }
        } audioProcessing;
        StreamData() :
            cont_type(ContentType::Audio),
            input_file(QString()),
            output_file(QString()),
            title(QString()),
            duration(0.f),
            stream(-1)
        {}
    };

    // ********************* Report ******************************//
    struct ReportLog {
        enum class Icon : uchar {
            Info, Warning, Critical
        };
        Icon    reportIcon;
        QString reportTime,
                reportMsg;
        ReportLog() :
            reportIcon(Icon::Info),
            reportTime(QString()),
            reportMsg(QString())
        {}
    };

    // ******************** Metadata *****************************//
    struct Data {
        enum StreamCheck {
            audioChecks, externAudioChecks, subtChecks, externSubtChecks,
            audioDef,    externAudioDef,    subtDef,    externSubtDef,
            subtBurn,    externSubtBurn
        };
        enum StreamField {
            audioFormats, audioChannels, audioChLayouts, audioDuration,
            audioLangs,   audioTitles,

            externAudioFormats,  externAudioChannels, externAudioChLayouts,
            externAudioDuration, externAudioLangs,    externAudioTitles,
            externAudioPath,

            subtFormats, subtDuration, subtLangs, subtTitles,

            externSubtFormats, externSubtDuration, externSubtLangs,
            externSubtTitles,  externSubtPath
        };

        static const int CHECKS_COUNT = 10;
        static const int FIELDS_COUNT = 22;

        QVector<QString> videoMetadata;
        QVector<bool>    checks[CHECKS_COUNT];
        QVector<QString> fields[FIELDS_COUNT];
        
        // Per-file preset parameters
        QVector<QString> presetParams;
        QString presetName;

        // User-selected FFmpeg video filters.  Stored per input so a batch can
        // contain files with different looks without changing the preset format.
        QStringList videoEffects;
        QVector<StreamData::AudioTrackProcessing> audioProcessing;
        QVector<StreamData::AudioTrackProcessing> externAudioProcessing;
        
        // Chapter metadata file path for import
        QString chaptersFile;
        
        void clear() {
            videoMetadata.clear();
            for (auto & check : checks)
                check.clear();
            for (auto & field : fields)
                field.clear();
            presetParams.clear();
            presetName.clear();
            videoEffects.clear();
            audioProcessing.clear();
            externAudioProcessing.clear();
            chaptersFile.clear();
        }
    };

    enum VideoMetadataIndex {
        VIDEO_TITLE,  VIDEO_MOVIENAME, VIDEO_YEAR,
        VIDEO_AUTHOR, VIDEO_PERFORMER, VIDEO_DESCRIPTION
    };

    // ***************** Encoder Data **************************//
    enum CurParamIndex {
        OUTPUT_PARAM,   CODEC,          MODE,
        CONTAINER,      BQR,            MAXRATE,
        BUFSIZE,        FRAME_RATE,     BLENDING,
        WIDTH,          HEIGHT,         PASS,
        PRESET,         COLOR_RANGE,    MIN_LUM,
        MAX_LUM,        MAX_CLL,        MAX_FALL,
        MASTER_DISPLAY, CHROMA_COORD,   WHITE_COORD,
        AUDIO_CODEC,    AUDIO_BITRATE,  MINRATE,
        LEVEL,          ASAMPLE_RATE,   ACHANNELS,
        MATRIX,         PRIMARY,        TRC,
        PRESET_NAME,    REP_PRIM,       REP_MATRIX,
        REP_TRC,
        USE_PRESET_SUBTITLE_SETTINGS,
        SUBTITLE_FONT, SUBTITLE_FONT_SIZE,
        SUBTITLE_FONT_COLOR,
        SUBTITLE_BACKGROUND, SUBTITLE_BACKGROUND_COLOR,
        SUBTITLE_BACKGROUND_ALPHA, SUBTITLE_LOCATION,
        USE_PRESET_DEINTERLACE_SETTINGS,
        DEINTERLACE_ENABLED,
        DEINTERLACE_FILTER
    };

    static QString param_names[] = {
            "OUTPUT_PARAM",
            "CODEC", "MODE",
            "CONTAINER", "BQR", "MAXRATE",
             "BUFSIZE", "FRAME_RATE", "BLENDING",
            "WIDTH", "HEIGHT", "PASS",
            "PRESET", "COLOR_RANGE", "MIN_LUM",
            "MAX_LUM", "MAX_CLL", "MAX_FALL",
            "MASTER_DISPLAY", "CHROMA_COORD", "WHITE_COORD",
            "AUDIO_CODEC", "AUDIO_BITRATE", "MINRATE",
            "LEVEL", "ASAMPLE_RATE", "ACHANNELS",
            "MATRIX", "PRIMARY", "TRC",
            "PRESET_NAME", "REP_PRIM", "REP_MATRIX",
            "REP_TRC",
            "USE_PRESET_SUBTITLE_SETTINGS",
            "SUBTITLE_FONT", "SUBTITLE_FONT_SIZE",
            "SUBTITLE_FONT_COLOR",
            "SUBTITLE_BACKGROUND", "SUBTITLE_BACKGROUND_COLOR",
            "SUBTITLE_BACKGROUND_ALPHA", "SUBTITLE_LOCATION",
            "USE_PRESET_DEINTERLACE_SETTINGS",
            "DEINTERLACE_ENABLED",
            "DEINTERLACE_FILTER"
    };

    static QList<QString> default_preset = {
            "ProRes HQ, Res: Source, Fps: Source, YUV, 4:2:2, 10 bit, HDR: Enabled, Audio: PCM 16 bit, MOV",
            "19", "0",
            "0", "Auto", "Auto",
            "Auto", "0", "0",
            "0", "0", "0",
            "0", "0", "",
            "", "", "",
            "0", "From source", "From source",
            "0", "0", "Auto",
            "0", "0", "0",
            "0", "0", "0",
            "ProRes HQ", "0", "0",
            "0",
            "0", "Arial", numToStr(FONTSIZE), defaultSubtitleColor(),
            "0", defaultSubtitleBackgroundColor(), "150", "0",
            "0", "0", "0"
    };

    struct EncoderAudioParam {
        int AUDIO_CODEC;
        int AUDIO_BITRATE;
        int AUDIO_SAMPLING;
        int AUDIO_CHANNELS;
        QString AUDIO_CONTAINER;
    };

    struct EncoderSubtParam {
        int SUBT_CODEC;
        QString SUBT_CONTAINER;
    };

    // ******************* Presets *****************************//
    enum Profile {
        HIGH,           MAIN,           MAIN10,
        MAIN12,         PROFILE_0,      PROFILE_1,
        PROFILE_2,      PROFILE_3,      PROFILE_4,
        PROFILE_5,      DNXHR_LB,       DNXHR_SQ,
        DNXHR_HQ,       DNXHR_HQX,      DNXHR_444,
        PROFILE_AUTO
    };
    enum Pixformat {
        YUV420P12LE,    YUV444P10LE,    YUV422P10LE,
        YUV420P10LE,    YUV422p,        YUV420P,
        P010LE,         PIXFORMAT_AUTO
    };

    enum CurHDRIndex {
        CUR_COLOR_RANGE,    CUR_COLOR_PRIMARY,  CUR_COLOR_MATRIX,
        CUR_TRANSFER,       CUR_MAX_LUM,        CUR_MIN_LUM,
        CUR_MAX_CLL,        CUR_MAX_FALL,       CUR_MASTER_DISPLAY,
        CUR_CHROMA_COORD,   CUR_WHITE_COORD
    };

    enum MasterDisplay {
        SOURCE, DISPLAY_P3, DCI_P3, BT_2020, BT_709, CUSTOM
    };   

    enum DeinterlaceFilter {
        DEINTERLACE_NONE, DEINTERLACE_YADIF, DEINTERLACE_BWDIF, DEINTERLACE_ESTDIF
    };

    enum EncodingStatus {
        START, PAUSE, RESUME
    };
}

#endif // CONSTANTS_H
