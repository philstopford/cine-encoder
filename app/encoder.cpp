/***********************************************************************

                          C I N E   E N C O D E R
                                JULY, 2020
                            COPYRIGHT (C) 2020

 FILE: encoder.cpp
 COMMENT:
 LICENSE: GNU General Public License v3.0

***********************************************************************/

#include "encoder.h"
#include "tables.h"
#include "helper.h"
#include <QDir>
#include <QMap>
#include <iostream>
#include <cmath>
#include <ctime>
#include <algorithm>
#include <QColor>


#define rnd(num) static_cast<int>(round(num))


Encoder::Encoder(QObject *parent) :
    QObject(parent)
{
    processEncoding = new QProcess(this);
    processEncoding->setProcessChannelMode(QProcess::MergedChannels);
    processEncoding->setWorkingDirectory(QDir::homePath());
}

Encoder::~Encoder()
= default;

/************************************************
** Encoder
************************************************/

void Encoder::initEncoding(const QString  &temp_file,
                           const QString  &input_file,
                           const QString  &output_file,
                           QString  &_width,
                           QString  &_height,
                           const QString  &_fps,
                           const double   &_startTime,
                           const double   &_endTime,
                           const double   &_dur,
                           const QString  &container,
                           QString  globalTitle,
                           QVector<QString> _cur_param,
                           const QString  _hdr[],
                           Data           data,
                           int            *_fr_count,
                           int            streamCutting,
                           const QString &subtitle_font,
                           int subtitle_font_size,
                           const QString &subtitle_font_color,
                           const bool burn_background,
                           const QString &subtitle_background_color,
                           int subtitle_location,
                           int threads
                           )
{
    Tables t;
    int CE_CODEC;
    int CE_MODE;
    QString CE_BQR;
    QString CE_MINRATE;
    QString CE_MAXRATE;
    QString CE_BUFSIZE;
    int CE_LEVEL;
    int CE_FRAME_RATE;
    int CE_BLENDING;
    int CE_WIDTH;
    int CE_HEIGHT;
    int CE_PASS;
    int CE_PRESET;
    int CE_COLOR_RANGE;
    int CE_MATRIX;
    int CE_PRIMARY;
    int CE_TRC;
    QString CE_MIN_LUM;
    QString CE_MAX_LUM;
    QString CE_MAX_CLL;
    QString CE_MAX_FALL;
    int CE_MASTER_DISPLAY;
    QString CE_CHROMA_COORD;
    QString CE_WHITE_COORD;
    int CE_AUDIO_CODEC;
    int CE_AUDIO_BITRATE;
    int CE_AUDIO_SAMPLING;
    int CE_AUDIO_CHANNELS;
    int CE_REP_PRIM;
    int CE_REP_MATRIX;
    int CE_REP_TRC;
    int CE_USE_PRESET_SUBTITLES;
    QString CE_SUBTITLE_FONT;
    int CE_SUBTITLE_FONT_SIZE;
    QString CE_SUBTITLE_FONT_COLOR;
    int CE_SUBTITLE_BACKGROUND;
    QString CE_SUBTITLE_BACKGROUND_COLOR;
    int CE_SUBTITLE_LOCATION;
    initVariables(temp_file, input_file, output_file, _cur_param, _fr_count, t, CE_CODEC, CE_MODE, CE_BQR, CE_MINRATE, CE_MAXRATE,
                  CE_BUFSIZE,
                  CE_LEVEL, CE_FRAME_RATE, CE_BLENDING, CE_WIDTH, CE_HEIGHT, CE_PASS, CE_PRESET, CE_COLOR_RANGE, CE_MATRIX, CE_PRIMARY,
                  CE_TRC, CE_MIN_LUM,
                  CE_MAX_LUM, CE_MAX_CLL, CE_MAX_FALL, CE_MASTER_DISPLAY, CE_CHROMA_COORD, CE_WHITE_COORD, CE_AUDIO_CODEC,
                  CE_AUDIO_BITRATE,
                  CE_AUDIO_SAMPLING, CE_AUDIO_CHANNELS, CE_REP_PRIM, CE_REP_MATRIX, CE_REP_TRC,
                  CE_USE_PRESET_SUBTITLES, CE_SUBTITLE_FONT, CE_SUBTITLE_FONT_SIZE, CE_SUBTITLE_FONT_COLOR, CE_SUBTITLE_BACKGROUND,
                  CE_SUBTITLE_BACKGROUND_COLOR, CE_SUBTITLE_LOCATION);

    /****************************************** Resize ****************************************/
    QString resize_vf;
    resizeVF(_width, _height, CE_CODEC, CE_WIDTH, CE_HEIGHT, t, resize_vf);
    const QString width = _width;
    const QString height = _height;

    /******************************************* FPS *****************************************/
    QString fps_vf;
    double fps_dest;
    fpsVF(_fps, CE_CODEC, CE_FRAME_RATE, CE_BLENDING, t, fps_vf, fps_dest);

    /****************************************** Split ****************************************/
    QVector<double> extDurVect;
    foreach (auto dur, data.fields[Data::externAudioDuration])
        extDurVect.push_back(0.001*dur.toDouble());
    double minExtTime = 0;
    if (extDurVect.count() > 0) {
        minExtTime = *std::min_element(extDurVect.begin(), extDurVect.end());
    }
    Print("Min external time: " << minExtTime);

    QStringList _splitStartParam;
    QStringList _splitParam;
    split(_startTime, _endTime, _dur, streamCutting, t, CE_CODEC, fps_dest, minExtTime, _splitStartParam, _splitParam);

    /************************************** Video metadata ************************************/

    QVector<QString> videoMetadata(data.videoMetadata.size(), "");
    QStringList _videoMetadataParam;
    data = video(globalTitle, data, videoMetadata, _videoMetadataParam);

    /************************************** Audio streams ************************************/
    QStringList _audioMapParam;
    QStringList _audioMetadataParam;
    int audioNum;
    audio(data, _audioMapParam, _audioMetadataParam, audioNum);

    /********************************* External Audio streams ************************************/
    int extTrackNum = extAudio(data, _audioMapParam, _audioMetadataParam, audioNum);

    /**************************************** Subtitles **************************************/
    QStringList burn_subt_vf;
    QStringList _subtitleMapParam;
    QStringList _subtitleMetadataParam;
    QStringList _subtitleFormatParam;
    int subtNum;
    subtitles(input_file,
              CE_USE_PRESET_SUBTITLES == 1 ? CE_SUBTITLE_FONT : subtitle_font,
              CE_USE_PRESET_SUBTITLES == 1 ? CE_SUBTITLE_FONT_SIZE : subtitle_font_size,
              CE_USE_PRESET_SUBTITLES == 1 ? CE_SUBTITLE_FONT_COLOR : subtitle_font_color,
              CE_USE_PRESET_SUBTITLES == 1 ? CE_SUBTITLE_BACKGROUND : burn_background,
              CE_USE_PRESET_SUBTITLES == 1 ? CE_SUBTITLE_BACKGROUND_COLOR : subtitle_background_color,
              CE_USE_PRESET_SUBTITLES == 1 ? CE_SUBTITLE_LOCATION : subtitle_location,
              data, burn_subt_vf, const_cast<QString &>(width), const_cast<QString &>(height), _subtitleMapParam,
              _subtitleMetadataParam,
              _subtitleFormatParam,
              subtNum);



    /****************************** External Subtitle streams *********************************/
    extSub(data, extTrackNum, _subtitleMapParam, _subtitleMetadataParam, _subtitleFormatParam, subtNum);

    /************************************* Codec module ***************************************/
    QString hwaccel;
    QString hwaccel_filter_vf;
    codecModule(t, CE_CODEC, hwaccel, hwaccel_filter_vf);

    /************************************* Level module **************************************/
    QStringList level = levelModule(t, CE_CODEC, CE_LEVEL);

    /************************************* Mode module ***************************************/
    QStringList mode = modeModule(t, CE_CODEC, CE_MODE, CE_BQR, CE_MINRATE, CE_MAXRATE, CE_BUFSIZE);

    /************************************* Preset module ***************************************/
    QStringList preset = presetModule(t, CE_CODEC, CE_PRESET);

    /************************************* Pass module ***************************************/
    QStringList pass;
    QStringList pass1;
    passModule(t, CE_CODEC, CE_PASS, pass, pass1);

    /************************************* Audio module ***************************************/
    QStringList audio_param = audioModule(t, CE_CODEC, CE_AUDIO_CODEC, CE_AUDIO_BITRATE, CE_AUDIO_SAMPLING, CE_AUDIO_CHANNELS);

    /************************************ Subtitle module *************************************/
    // QStringList sub_param = subModule(container);

    /************************************* Color module ***************************************/

    // color primaries
    QStringList colorprim;
    QStringList colorprim_vf;
    colorPrimaries(_hdr, CE_PRIMARY, CE_REP_PRIM, colorprim, colorprim_vf);

    // color matrix
    QStringList colormatrix;
    QStringList colormatrix_vf;
    colorMatrix(_hdr, CE_MATRIX, CE_REP_MATRIX, colormatrix, colormatrix_vf);

    // transfer characteristics
    QStringList transfer;
    QStringList transfer_vf;
    colorTransfer(_hdr, CE_TRC, CE_REP_TRC, transfer, transfer_vf);

    QStringList codec = getCodec(t, CE_CODEC, resize_vf, fps_vf, _videoMetadataParam, _audioMapParam, _audioMetadataParam,
                                 burn_subt_vf, _subtitleMapParam, _subtitleMetadataParam, _subtitleFormatParam, hwaccel_filter_vf,
                                 colorprim_vf,
                                 colormatrix_vf, transfer_vf);

    /************************************* HDR module ***************************************/

    QStringList color_range;
    QStringList max_lum;
    QStringList min_lum;
    QStringList max_cll;
    QStringList max_fall;
    QStringList chroma_coord;
    QStringList white_coord;
    if (_flag_hdr) {

        /********************************* Color range module **********************************/
        hdrColorRange(_hdr, CE_COLOR_RANGE, color_range);

        /************************************* Lum module ***************************************/
        hdrLum(_hdr, CE_MIN_LUM, CE_MAX_LUM, CE_MAX_CLL, CE_MAX_FALL, max_lum, min_lum, max_cll, max_fall);

        /************************************* Display module ***************************************/
        hdrDisplay(_hdr, CE_MASTER_DISPLAY, CE_CHROMA_COORD, CE_WHITE_COORD, chroma_coord, white_coord);

    }

    /************************************* Result module ***************************************/
    getPresets(_splitStartParam, _splitParam, hwaccel, level, mode, preset, pass, pass1, audio_param,
               colorprim,
               colormatrix, transfer, codec, color_range, max_lum, min_lum, max_cll, max_fall, chroma_coord,
               white_coord);

    Print("Flag two-pass: " << _flag_two_pass);
    Print("Flag HDR: " << _flag_hdr);
    Print("preset_0: " << _preset_0.join(" ").toStdString());

    QString log = getLog();
    emit onEncodingLog(log);
    _threads = threads;
    encode();
}

void Encoder::initVariables(const QString &temp_file, const QString &input_file, const QString &output_file,
                            QVector<QString> &_cur_param, int *_fr_count, Tables &t, int &CE_CODEC, int &CE_MODE,
                            QString &CE_BQR, QString &CE_MINRATE, QString &CE_MAXRATE, QString &CE_BUFSIZE, int &CE_LEVEL,
                            int &CE_FRAME_RATE, int &CE_BLENDING, int &CE_WIDTH, int &CE_HEIGHT, int &CE_PASS, int &CE_PRESET,
                            int &CE_COLOR_RANGE, int &CE_MATRIX, int &CE_PRIMARY, int &CE_TRC, QString &CE_MIN_LUM,
                            QString &CE_MAX_LUM, QString &CE_MAX_CLL, QString &CE_MAX_FALL, int &CE_MASTER_DISPLAY,
                            QString &CE_CHROMA_COORD, QString &CE_WHITE_COORD, int &CE_AUDIO_CODEC, int &CE_AUDIO_BITRATE,
                            int &CE_AUDIO_SAMPLING, int &CE_AUDIO_CHANNELS, int &CE_REP_PRIM, int &CE_REP_MATRIX,
                            int &CE_REP_TRC,
                            int &CE_USE_PRESET_SUBTITLES, QString &CE_SUBTITLE_FONT, int &CE_SUBTITLE_FONT_SIZE,
                            QString &CE_SUBTITLE_FONT_COLOR, int &CE_SUBTITLE_BACKGROUND,
                            QString &CE_SUBTITLE_BACKGROUND_COLOR,
                            int &CE_SUBTITLE_LOCATION) {
    CE_CODEC= _cur_param[CODEC].toInt();
    CE_MODE= _cur_param[MODE].toInt();
    CE_BQR= _cur_param[BQR];
    CE_MINRATE= _cur_param[MINRATE];
    CE_MAXRATE= _cur_param[MAXRATE];
    CE_BUFSIZE= _cur_param[BUFSIZE];
    CE_LEVEL= _cur_param[LEVEL].toInt();
    CE_FRAME_RATE= _cur_param[FRAME_RATE].toInt();
    CE_BLENDING= _cur_param[BLENDING].toInt();
    CE_WIDTH= _cur_param[WIDTH].toInt();
    CE_HEIGHT= _cur_param[HEIGHT].toInt();
    CE_PASS= _cur_param[PASS].toInt();
    CE_PRESET= _cur_param[PRESET].toInt();
    CE_COLOR_RANGE= _cur_param[COLOR_RANGE].toInt();
    CE_MATRIX= _cur_param[MATRIX].toInt();
    CE_PRIMARY= _cur_param[PRIMARY].toInt();
    CE_TRC= _cur_param[TRC].toInt();
    CE_MIN_LUM= _cur_param[MIN_LUM].replace(",", ".");
    CE_MAX_LUM= _cur_param[MAX_LUM].replace(",", ".");
    CE_MAX_CLL= _cur_param[MAX_CLL].replace(",", ".");
    CE_MAX_FALL= _cur_param[MAX_FALL].replace(",", ".");
    CE_MASTER_DISPLAY= _cur_param[MASTER_DISPLAY].toInt();
    CE_CHROMA_COORD= _cur_param[CHROMA_COORD];
    CE_WHITE_COORD= _cur_param[WHITE_COORD];
    CE_AUDIO_CODEC= _cur_param[AUDIO_CODEC].toInt();
    CE_AUDIO_BITRATE= _cur_param[AUDIO_BITRATE].toInt();
    CE_AUDIO_SAMPLING= _cur_param[ASAMPLE_RATE].toInt();
    CE_AUDIO_CHANNELS= _cur_param[ACHANNELS].toInt();
    CE_REP_PRIM= _cur_param[REP_PRIM].toInt();
    CE_REP_MATRIX= _cur_param[REP_MATRIX].toInt();
    CE_REP_TRC= _cur_param[REP_TRC].toInt();
    CE_USE_PRESET_SUBTITLES = _cur_param[USE_PRESET_SUBTITLE_SETTINGS].toInt();
    CE_SUBTITLE_FONT = _cur_param[SUBTITLE_FONT];
    CE_SUBTITLE_FONT_SIZE = _cur_param[SUBTITLE_FONT_SIZE].toInt();
    CE_SUBTITLE_FONT_COLOR = _cur_param[SUBTITLE_FONT_COLOR];
    CE_SUBTITLE_BACKGROUND = _cur_param[SUBTITLE_BACKGROUND].toInt();
    QString bgCol = _cur_param[SUBTITLE_BACKGROUND_COLOR];
    int bgAlpha = _cur_param[SUBTITLE_BACKGROUND_ALPHA].toInt();
    // Now we need to get the alpha set into the color.
    auto bgColor = QColor(bgCol);
    bgColor.setAlpha(bgAlpha);
    CE_SUBTITLE_BACKGROUND_COLOR = bgColor.name();
    CE_SUBTITLE_LOCATION = _cur_param[SUBTITLE_LOCATION].toInt();
    Print("Make preset...");
    _temp_file = temp_file;
    _input_file = input_file;
    _output_file = output_file;
    fr_count = _fr_count;//int _CONTAINER = _cur_param[CurParamIndex::CONTAINER].toInt();
    _extAudioPaths = QStringList();
    _extSubPaths = QStringList();
    _preset_0 = QStringList();
    _preset_pass1.clear();
    _preset.clear();
    _preset_mkvmerge = "";
    _sub_mux_param.clear();
    _error_message = "";
    _flag_two_pass = false;
    _flag_hdr = false;
    _burn_subtitle = false;
    _mux_mode = false;
    *fr_count = 0;
    _threads = 0;
}

Data &Encoder::video(QString &globalTitle, Data &data, QVector<QString> &videoMetadata,
                     QStringList &_videoMetadataParam) {
    if (globalTitle != "") {
        videoMetadata[0] = QString("-metadata:s:v:0 title=\"%1\" ").arg(Helper::makeFileStringFFMPEGReady(globalTitle).replace(" ", "\u00A0"));
        _videoMetadataParam.append({"-metadata:s:v:0",QString("title=\""+Helper::makeFileStringFFMPEGReady(globalTitle)+"\"")});
    } else {
        if (data.videoMetadata[VIDEO_TITLE] != "") {
            videoMetadata[0] = QString("-metadata:s:v:0 title=\"%1\" ").arg(Helper::makeFileStringFFMPEGReady(data.videoMetadata[VIDEO_TITLE])
                                                                                .replace(" ", "\u00A0"));
            _videoMetadataParam.append({"-metadata:s:v:0",QString("title=\"" + Helper::makeFileStringFFMPEGReady(data.videoMetadata[VIDEO_TITLE]) + "\"")});
        } else {
            videoMetadata[0] = QString("-map_metadata:s:v:0 -1 ");
            _videoMetadataParam.append({"-map_metadata:s:v:0","-1"});
        }
    }
    if (data.videoMetadata[VIDEO_MOVIENAME] != "") {
        videoMetadata[1] = QString("-metadata title=\"%1\" ").arg(Helper::makeFileStringFFMPEGReady(data.videoMetadata[VIDEO_MOVIENAME])
                                                              .replace(" ", "\u00A0"));
        _videoMetadataParam.append({"-metadata",QString("title=\""+Helper::makeFileStringFFMPEGReady(data.videoMetadata[VIDEO_MOVIENAME])+"\"")});
    }
    if (data.videoMetadata[VIDEO_AUTHOR] != "") {
        videoMetadata[2] = QString("-metadata author=\"%1\" ").arg(Helper::makeFileStringFFMPEGReady(data.videoMetadata[VIDEO_AUTHOR])
                                                               .replace(" ", "\u00A0"));
        _videoMetadataParam.append({"-metadata",QString("author=\""+Helper::makeFileStringFFMPEGReady(data.videoMetadata[VIDEO_AUTHOR])+"\"")});
    }
    if (data.videoMetadata[VIDEO_DESCRIPTION] != "") {
        videoMetadata[3] = QString("-metadata description=\"%1\" ").arg(Helper::makeFileStringFFMPEGReady(data.videoMetadata[VIDEO_DESCRIPTION])
                                                                    .replace(" ", "\u00A0"));
        _videoMetadataParam.append({"-metadata",QString("description=\""+Helper::makeFileStringFFMPEGReady(data.videoMetadata[VIDEO_DESCRIPTION])+"\"")});
    }
    if (data.videoMetadata[VIDEO_YEAR] != "") {
        videoMetadata[4] = QString("-metadata year=%1 ").arg(Helper::makeFileStringFFMPEGReady(data.videoMetadata[VIDEO_YEAR]).replace(" ", ""));
        _videoMetadataParam.append({"-metadata",QString("year="+Helper::makeFileStringFFMPEGReady(data.videoMetadata[VIDEO_YEAR]))});
    }
    if (data.videoMetadata[VIDEO_PERFORMER] != "") {
        videoMetadata[5] = QString("-metadata author=\"%1\" ").arg(Helper::makeFileStringFFMPEGReady(data.videoMetadata[VIDEO_PERFORMER])
                                                               .replace(" ", "\u00A0"));
        _videoMetadataParam.append({"-metadata",QString("author=\""+Helper::makeFileStringFFMPEGReady(data.videoMetadata[VIDEO_PERFORMER])+"\"")});
    }
    return data;
}

void Encoder::getPresets(const QStringList &_splitStartParam, const QStringList &_splitParam, const QString &hwaccel,
                         const QStringList &level, const QStringList &mode, const QStringList &preset,
                         const QStringList &pass, const QStringList &pass1, const QStringList &audio_param,
                         const QStringList &colorprim, const QStringList &colormatrix,
                         const QStringList &transfer, const QStringList &codec, const QStringList &color_range,
                         const QStringList &max_lum, const QStringList &min_lum, const QStringList &max_cll,
                         const QStringList &max_fall, const QStringList &chroma_coord, const QStringList &white_coord) {
    _preset_0 = QStringList("-hide_banner");
    _preset_0.append("-probesize");
    _preset_0.append("100M");
    _preset_0.append("-analyzeduration");
    _preset_0.append("50M");
    _preset_0.append(hwaccel);
    // _preset_0.append(_splitStartParam.join(" "));
    _preset_0.append(_splitStartParam);
    _preset_pass1 = _splitParam + codec + level + preset + mode + pass1 + color_range
                    + colorprim + colormatrix + transfer + QStringList {"-an","-sn","-f","null", "/dev/null"};
    _preset = _splitParam + codec + level + preset + mode + pass + color_range
              + colorprim + colormatrix + transfer + audio_param;
    // DEBUG
/*
std::string _presetarray[_preset.length()];
for (int i = 0; i < _preset.length(); i++)
{
    _presetarray[i] = _preset[i].toStdString();
}
*/

    _preset_mkvmerge = max_cll.join(" ") + max_fall.join(" ") + max_lum.join(" ") + min_lum.join(" ") + chroma_coord.join(" ") + white_coord.join(" ");
}

QStringList Encoder::getCodec(const Tables &t, int CE_CODEC, const QString &resize_vf, const QString &fps_vf,
                              const QStringList &_videoMetadataParam, const QStringList &_audioMapParam,
                              const QStringList &_audioMetadataParam, const QStringList &burn_subt_vf,
                              const QStringList &_subtitleMapParam, const QStringList &_subtitleMetadataParam,
                              const QStringList &_subtitleFormatParam,
                              const QString &hwaccel_filter_vf, const QStringList &colorprim_vf,
                              const QStringList &colormatrix_vf, const QStringList &transfer_vf) const {
    QStringList codec;
    // Need to pay attention to whether the complex filter is being used. It seems to be incompatible with these arguments.
    if (!_burn_subtitle || (_burn_subtitle && (!burn_subt_vf[0].startsWith("-filter_complex"))))
    {
        codec.append("-map");
        codec.append("0:v:0?");
    }
    codec.append(_audioMapParam);
    codec.append(_subtitleMapParam);
    codec.append({"-map_metadata", "-1", "-map_chapters", "-1"});
    codec.append(_videoMetadataParam);
    codec.append(_audioMetadataParam);
    codec.append(_subtitleMetadataParam);
    codec.append(_subtitleFormatParam);
    if ((hwaccel_filter_vf != "") ||
        (fps_vf != "") ||
        (resize_vf != "") ||
        (colorprim_vf.count() != 0) ||
        (colormatrix_vf.count() != 0) ||
        (transfer_vf.count() != 0) ||
        (burn_subt_vf.count() != 0) ||
        _burn_subtitle)
    {
        // If the complex filter is used, we don't want the -vf switch
        if (!burn_subt_vf[0].startsWith("-filter_complex")) {
            codec.append("-vf");
        }
    }
    codec.append(hwaccel_filter_vf.split(" "));
    codec.append(fps_vf.split(" "));
    codec.append(resize_vf.split(" "));
    codec.append(colorprim_vf);
    codec.append(colormatrix_vf);
    codec.append(transfer_vf);
    codec.append(burn_subt_vf);
    codec.append(t.arr_params[CE_CODEC][0].split(" "));
    return codec;
}

void Encoder::subtitles(const QString &input_file, const QString &subtitle_font, int subtitle_font_size,
                        const QString &subtitle_font_color, const bool burn_background,
                        const QString &subtitle_background_color, int subtitle_location, Data &data,
                        QStringList &burn_subt_vf, QString& width, QString& height, QStringList &_subtitleMapParam, QStringList &_subtitleMetadataParam,
                        QStringList &_subtitleFormatParam,
                        int &subtNum) {
    subtNum= 0;
    std::string debugstr = burn_subt_vf.join(" ").toStdString();
    subtVF(input_file, subtitle_font, subtitle_font_size, subtitle_font_color, burn_background,
           subtitle_background_color, subtitle_location, data, burn_subt_vf, width, height);
    std::string debugstr2 = burn_subt_vf.join(" ").toStdString();

    auto length = data.checks[Data::subtChecks].size();
    if (!_burn_subtitle) {
        for (int k = 0; k < length; k++) {
            if (data.checks[Data::subtChecks][k]) {
                std::string subtitleFormat = data.fields[Data::subtFormats][k].toStdString();
                // This one has some subtleties. mov_text worked fine except for certain
                // files (mkv) that had a UTF-8 subtitle track which started complaining.
                // srt seems OK so far.
                // Largely this is by empirical testing.
                if (subtitleFormat == "UTF-8")
                {
                    // _subtitleFormatParam.append({"-c:s", "dvd_subtitle"});
                    _subtitleFormatParam.append({"-c:s", "mov_text"});
                }
                else if (subtitleFormat == "ASS")
                {
                    _subtitleFormatParam.append({"-c:s", "mov_text"});
                }
                else if (subtitleFormat == "D_WEBVTT/SUBTITLES")
                {
                    _subtitleFormatParam.append({"-c:s", "webvtt"});
                }
                else if (subtitleFormat == "Timed Text")
                {
                    _subtitleFormatParam.append({"-c:s", "ttml"});
                }
                else
                {
                    _subtitleFormatParam.append({"-c:s", "copy"});
                    // _subtitleFormatParam.append({"-c:s", "dvd_subtitle"});
                }
                _subtitleMapParam.append({"-map", "0:s:"+numToStr(k)+"?"});
                _subtitleMetadataParam.append({"-metadata:s:s:"+numToStr(subtNum), "language="+Helper::makeFileStringFFMPEGReady(data.fields[Data::subtLangs][k]) });
                _subtitleMetadataParam.append({"-metadata:s:s:"+numToStr(subtNum), "title="+Helper::makeFileStringFFMPEGReady(data.fields[Data::subtTitles][k]) });
                _subtitleMetadataParam.append({"-disposition:s:"+numToStr(subtNum), data.checks[Data::subtDef][k] ? "default" : "0" });
                subtNum++;
            }
        }
    }
}

QStringList Encoder::levelModule(const Tables &t, int CE_CODEC, int CE_LEVEL) {
    const QString selected_level = t.arr_level[CE_CODEC][CE_LEVEL];
    QStringList level;
    if (selected_level != "" && selected_level != tr("Auto"))
    {
        level.append({"-level:v",selected_level});
    }
    return level;
}

void Encoder::colorMatrix(const QString _hdr[], int CE_MATRIX, int CE_REP_MATRIX, QStringList &colormatrix,
                          QStringList &colormatrix_vf) {
    const QString arr_colormatrix[14] = {
        "Source", "bt470bg", "bt709", "bt2020nc", "bt2020c", "smpte170m", "smpte240m",
        "smpte2085", "chroma-derived-nc", "chroma-derived-c", "fcc", "GBR", "ICtCp", "YCgCo"
    };
    const QMap<QString, QString> curr_colormatrix = {
        {"BT709",                   "bt709"},
        {"BT2020nc",                "bt2020nc"},
        {"BT2020c",                 "bt2020c"},
        {"FCC73682",                "fcc"},
        {"BT470SystemB/G",          "bt470bg"},
        {"SMPTE240M",               "smpte240m"},
        {"YCgCo",                   "YCgCo"},
        {"Y'D'zD'x",                "smpte2085"},
        {"Chromaticity-derivednc",  "chroma-derived-nc"},
        {"Chromaticity-derivedc",   "chroma-derived-c"},
        {"ICtCp",                   "ICtCp"},
        {"BT601",                   "smpte170m"},
        {"Identity",                "GBR"},
        {"",                        ""}
    };
    const QString selected_colormatrix = arr_colormatrix[CE_MATRIX];
    if (!curr_colormatrix.contains(_hdr[CUR_COLOR_MATRIX])) {
        _message = tr("Can\'t find color matrix %1 in source map.").arg(_hdr[CUR_COLOR_MATRIX]);
        emit onEncodingInitError(_message);
        return;
    }
    if (selected_colormatrix == "Source") {
        if (_hdr[CUR_COLOR_MATRIX] != "") {
            colormatrix.append({"-colorspace", curr_colormatrix[_hdr[CUR_COLOR_MATRIX]]});
        }
    }
    else {
        colormatrix.append({"-colorspace", selected_colormatrix});
        if (CE_REP_MATRIX == 2) {
            colormatrix_vf.append(QString("zscale=m=%1").arg(selected_colormatrix));
        } else {

        }
    }
}

void Encoder::colorTransfer(const QString _hdr[], int CE_TRC, int CE_REP_TRC, QStringList &transfer,
                            QStringList &transfer_vf) {
    const QString arr_trc[17] = {
        "Source", "bt470m", "bt470bg", "bt709", "bt1361e", "bt2020-10", "bt2020-12", "smpte170m",
        "smpte240m", "smpte428", "smpte2084", "arib-std-b67", "linear", "log100", "log316",
        "iec61966-2-1", "iec61966-2-4"
    };
    const QMap<QString, QString> curr_transfer = {
        {"BT709",                    "bt709"},
        {"PQ",                       "smpte2084"},
        {"HLG",                      "arib-std-b67"},
        {"BT2020 (10-bit)",          "bt2020-10"},
        {"BT2020 (12-bit)",          "bt2020-12"},
        {"BT470 System M",           "bt470m"},
        {"BT470 System B/G",         "bt470bg"},
        {"SMPTE 240M",               "smpte240m"},
        {"Linear",                   "linear"},
        {"Logarithmic (100:1)",      "log100"},
        {"Logarithmic (31622777:1)", "log316"},
        {"xvYCC",                    "iec61966-2-4"},
        {"BT1361",                   "bt1361e"},
        {"sRGB/sYCC",                "iec61966-2-1"},
        {"SMPTE 428M",               "smpte428"},
        {"BT601",                    "smpte170m"},
        {"",                         ""}
    };
    const QString selected_transfer = arr_trc[CE_TRC];
    if (!curr_transfer.contains(_hdr[CUR_TRANSFER])) {
        _message = tr("Can\'t find transfer characteristics %1 in source map.").arg(_hdr[CUR_TRANSFER]);
        emit onEncodingInitError(_message);
        return;
    }
    if (selected_transfer == "Source") {
        if (_hdr[CUR_TRANSFER] != "") {
            transfer.append({"-color_trc",curr_transfer[_hdr[CUR_TRANSFER]]});
        }
    }
    else {
        transfer.append({"-color_trc", selected_transfer});
        if (CE_REP_TRC == 2) {
            transfer_vf.append(QString("zscale=t=%1").arg(selected_transfer));
        } else {

        }
    }
}

void
Encoder::extSub(Data &data, int extTrackNum, QStringList &_subtitleMapParam, QStringList &_subtitleMetadataParam,
                QStringList &_subtitleFormatParam,
                int subtNum) {
    auto length = data.checks[Data::externSubtChecks].size();
    if (!_burn_subtitle) {
        for (int k = 0; k < length; k++) {
            if (data.checks[Data::externSubtChecks][k]) {
                std::string subtitleFormat = data.fields[Data::externSubtFormats][k].toStdString();
                if (subtitleFormat == "UTF-8")
                {
                    _subtitleFormatParam.append({"-c:s", "mov_text"});
                }
                else if (subtitleFormat == "D_WEBVTT/SUBTITLES")
                {
                    _subtitleFormatParam.append({"-c:s", "webvtt"});
                }
                else
                {
                    _subtitleFormatParam.append({"-c:s", "dvd_subtitle"});
                }
                _extSubPaths << "-i" << Helper::makeFileStringFFMPEGReady(data.fields[Data::externSubtPath][k]);
                _subtitleMapParam.append({"-map", numToStr(extTrackNum)+":s?"});
                _subtitleMetadataParam.append({"-metadata:s:s:"+numToStr(subtNum), "language="+Helper::makeFileStringFFMPEGReady(data.fields[Data::externSubtLangs][k]) });
                _subtitleMetadataParam.append({"-metadata:s:s:"+numToStr(subtNum), "title="+Helper::makeFileStringFFMPEGReady(data.fields[Data::externSubtTitles][k]) });
                _subtitleMetadataParam.append({"-disposition:s:"+numToStr(subtNum), data.checks[Data::externSubtDef][k] ? "default" : "0" });
                subtNum++;
                extTrackNum++;
            }
        }
    }
}

QStringList Encoder::presetModule(const Tables &t, int CE_CODEC, int CE_PRESET) {
    QStringList preset;
    const QString selected_preset = t.getCurrentPreset(CE_CODEC, CE_PRESET);
    if (selected_preset != "" && selected_preset != tr("None")) {
        preset.append({"-preset", selected_preset.toLower() });
    }
    return preset;
}

void
Encoder::audio(Data &data, QStringList &_audioMapParam, QStringList &_audioMetadataParam, int &audioNum) {
    audioNum= 0;
    auto length = data.checks[Data::audioChecks].size();
    for (int k = 0; k < length; k++) {
        if (data.checks[Data::audioChecks][k]) {
            _audioMapParam.append({"-map", "0:a:"+numToStr(k)+"?" });
            _audioMetadataParam.append({"-metadata:s:a:"+numToStr(audioNum),"language="+Helper::makeFileStringFFMPEGReady(data.fields[Data::audioLangs][k])});
            _audioMetadataParam.append({"-metadata:s:a:"+numToStr(audioNum),"title="+Helper::makeFileStringFFMPEGReady(data.fields[Data::audioTitles][k])});
            _audioMetadataParam.append({"-disposition:a:"+numToStr(audioNum),data.checks[Data::audioDef][k] ? "default" : "0"});
            audioNum++;
        }
    }
}

int Encoder::extAudio(Data &data, QStringList &_audioMapParam, QStringList &_audioMetadataParam, int audioNum) {
    auto length = data.checks[Data::externAudioChecks].size();
    int extTrackNum = 1;

    for (int k = 0; k < length; k++) {
        if (data.checks[Data::externAudioChecks][k]) {
            _extAudioPaths << "-i" << Helper::makeFileStringFFMPEGReady(data.fields[Data::externAudioPath][k]);
            _audioMapParam.append({"-map", numToStr(extTrackNum) + ":a?" });
            _audioMetadataParam.append({"-metadata:s:a:"+numToStr(audioNum),"language="+Helper::makeFileStringFFMPEGReady(data.fields[Data::externAudioLangs][k])});
            _audioMetadataParam.append({"-metadata:s:a:"+numToStr(audioNum),"title="+Helper::makeFileStringFFMPEGReady(data.fields[Data::externAudioTitles][k])});
            _audioMetadataParam.append({"-disposition:a:"+numToStr(audioNum),data.checks[Data::externAudioDef][k] ? "default" : "0"});
            audioNum++;
            extTrackNum++;
        }
    }
    return extTrackNum;
}

void Encoder::colorPrimaries(const QString _hdr[], int CE_PRIMARY, int CE_REP_PRIM, QStringList &colorprim,
                             QStringList &colorprim_vf) {
    const QString arr_colorprim[11] = {
        "Source",    "bt470m",   "bt470bg",  "bt709",    "bt2020", "smpte170m",
        "smpte240m", "smpte428", "smpte431", "smpte432", "film"
    };
    const QMap<QString, QString> curr_colorprim = {
        {"BT709",           "bt709"},
        {"BT2020",          "bt2020"},
        {"BT601 NTSC",      "smpte170m"},
        {"BT601 PAL",       "bt470bg"},
        {"BT470 System M",  "bt470m"},
        {"SMPTE 240M",      "smpte240m"},
        {"Generic film",    "film"},
        {"DCI P3",          "smpte431"},
        {"XYZ",             "smpte428"},
        {"Display P3",      "smpte432"},
        {"",                ""}
    };
    const QString selected_colorprim = arr_colorprim[CE_PRIMARY];
    if (!curr_colorprim.contains(_hdr[CUR_COLOR_PRIMARY])) {
        _message = tr("Can\'t find color primaries %1 in source map.").arg(_hdr[CUR_COLOR_PRIMARY]);
        emit onEncodingInitError(_message);
        return;
    }
    if (selected_colorprim == "Source") {
        if (_hdr[CUR_COLOR_PRIMARY] != "") {
            colorprim.append({"-color_primaries", curr_colorprim[_hdr[CUR_COLOR_PRIMARY]]});
        }
    }
    else {
        colorprim.append({"-color_primaries", selected_colorprim});
        if (CE_REP_PRIM == 2) {
            colorprim_vf.append(QString("zscale=p=%1").arg(selected_colorprim));
        } else {

        }
    }
}

// FIXME : I'm not really sure about this. The container plays a role in the supported subtitle
// streams, but the check here is naive. One can have different stream types (e.g. mov_text, dvd_subtitle)
// inside mp4 containers. It's not clear that this review is correct.
QStringList Encoder::subModule(const QString &container) {
    QStringList sub_param;

    if (_burn_subtitle) {
        _sub_mux_param.append("-sn");
        sub_param.append(_sub_mux_param);
    } else {
        if (container == "mkv") {
            _sub_mux_param.append({"-c:s", "ass"});
        } else
        if (container == "webm") {
            _sub_mux_param.append({"-c:s", "webvtt"});
        } else
        if (container == "mp4" || container == "mov") {
            // _sub_mux_param.append({"-c:s", "mov_text"});
            // This might be better for VobSub and srt support.
            _sub_mux_param.append({"-c:s", "dvd_subtitle"});
        } else {
            _sub_mux_param.append("-sn");
            emit onEncodingError(tr("Container \'%1\' will be transcoded without subtitles.")
                                         .arg(container), true);
        }

        if (_flag_hdr) {
            sub_param.append({"-c:s", "ass"});
        } else {
            sub_param.append(_sub_mux_param);
        }
    }
    return sub_param;
}

QStringList Encoder::modeModule(const Tables &t, int CE_CODEC, int CE_MODE, const QString &CE_BQR, const QString &CE_MINRATE,
                                const QString &CE_MAXRATE, const QString &CE_BUFSIZE) {
    QStringList mode;
    const QString bitrate = QString::number(1000000.0 * CE_BQR.toDouble(), 'f', 0);
    const QString minrate = QString::number(1000000.0 * CE_MINRATE.toDouble(), 'f', 0);
    const QString maxrate = QString::number(1000000.0 * CE_MAXRATE.toDouble(), 'f', 0);
    const QString bufsize = QString::number(1000000.0 * CE_BUFSIZE.toDouble(), 'f', 0);
    const QString selected_mode = t.arr_mode[CE_CODEC][CE_MODE];

    if (selected_mode == "CBR") {
        mode.append({"-b:v", bitrate, "-minrate", bitrate, "-maxrate", bitrate, "-bufsize", bufsize });
    }
    else
    if (selected_mode == "ABR") {
        mode.append({"-b:v", bitrate });
    }
    else
    if (selected_mode == "VBR") {
        mode.append({"-b:v", bitrate, "-minrate", minrate, "-maxrate", maxrate, "-bufsize", bufsize});
    }
    else
    if (selected_mode == "VBR_NV") {
        mode.append({"-b:v", bitrate, "-minrate", minrate, "-maxrate", maxrate, "-bufsize", bufsize, "-rc", "vbr"});
    }
    else
    if (selected_mode == "CRF") {
        mode.append({"-crf" , CE_BQR});
    }
    else
    if (selected_mode == "CQP") {
        mode.append({"-b:v", "0", "-cq", CE_BQR, "-qmin", CE_BQR, "-qmax", CE_BQR});
    }
    else
    if (selected_mode == "CQP_QS") {
        mode.append({"-global_quality", CE_BQR, "-look_ahead", "1"});
    }
    else
    if (selected_mode == "CQP_VA") {
        mode.append({"-qp", CE_BQR, "-rc_mode", "4"});
    }
    return mode;
}

void Encoder::hdrDisplay(const QString _hdr[11], int CE_MASTER_DISPLAY, const QString &CE_CHROMA_COORD,
                         const QString &CE_WHITE_COORD, QStringList &chroma_coord, QStringList &white_coord) {
    enum Display {Display_P3, Dci_P3, Bt_2020, Bt_709};
    enum Coord {red_x, red_y, green_x, green_y, blue_x, blue_y, white_x, white_y};
    const QString arr_coord[4][8] = {
        {"0.680", "0.320", "0.265", "0.690", "0.150", "0.060", "0.3127", "0.3290"}, // Display_P3
        {"0.680", "0.320", "0.265", "0.690", "0.150", "0.060", "0.314",  "0.3510"}, // DCI_P3
        {"0.708", "0.292", "0.170", "0.797", "0.131", "0.046", "0.3127", "0.3290"}, // BT.2020
        {"0.640", "0.330", "0.30",  "0.60",  "0.150", "0.060", "0.3127", "0.3290"}  // BT.709
    };
    QString current_coord[8] = {"", "", "", "", "", "", "", ""};

    auto fill_coord = [&current_coord, &arr_coord](int display){
        for (int i = red_x; i <= white_y; i++) {
            current_coord[i] = arr_coord[display][i];
        }
    };
    if (CE_MASTER_DISPLAY == SOURCE) {     // From source
        if (_hdr[CUR_MASTER_DISPLAY] == "Display P3") {
            fill_coord(Display::Display_P3);
        } else
        if (_hdr[CUR_MASTER_DISPLAY] == "DCI P3") {
            fill_coord(Display::Dci_P3);
        } else
        if (_hdr[CUR_MASTER_DISPLAY] == "BT.2020") {
            fill_coord(Display::Bt_2020);
        } else
        if (_hdr[CUR_MASTER_DISPLAY] == "BT.709") {
            fill_coord(Display::Bt_709);
        } else
        if (_hdr[CUR_MASTER_DISPLAY] == "Undefined") {
            const QStringList chr = _hdr[CUR_CHROMA_COORD].split(",");
            if (chr.size() == 6) {
                for (int i = red_x; i <= blue_y; i++) {
                    current_coord[i] = chr[i];
                }
            } else {
                _message = tr("Incorrect master display chroma coordinates source parameters!");
                emit onEncodingInitError(_message);
                return;
            }
            const QStringList wht = _hdr[CUR_WHITE_COORD].split(",");
            if (wht.size() == 2) {
                current_coord[white_x] = wht[0];
                current_coord[white_y] = wht[1];
            } else {
                _message = tr("Incorrect master display white point coordinates source parameters!");
                emit onEncodingInitError(_message);
                return;
            }
        }
    }
    if (CE_MASTER_DISPLAY == DISPLAY_P3) {     // Display P3
        fill_coord(Display::Display_P3);
    } else
    if (CE_MASTER_DISPLAY == DCI_P3) {     // DCI P3
        fill_coord(Display::Dci_P3);
    } else
    if (CE_MASTER_DISPLAY == BT_2020) {     // BT.2020
        fill_coord(Display::Bt_2020);
    } else
    if (CE_MASTER_DISPLAY == BT_709) {     // BT.709
        fill_coord(Display::Bt_709);
    } else
    if (CE_MASTER_DISPLAY == CUSTOM) {     // Custom
        QStringList chr = CE_CHROMA_COORD.split(",");
        if (chr.size() == 6) {
            for (int i = red_x; i <= blue_y; i++) {
                current_coord[i] = chr[i];
            }
        }
        QStringList wht = CE_WHITE_COORD.split(",");
        if (wht.size() == 2) {
            current_coord[white_x] = wht[0];
            current_coord[white_y] = wht[1];
        }
    }

    if (current_coord[red_x] == "") {
        chroma_coord.append({"-d", "chromaticity-coordinates-red-x", "-d", "chromaticity-coordinates-red-y",
                                "-d", "chromaticity-coordinates-green-x", "-d", "chromaticity-coordinates-green-y",
                                "-d", "chromaticity-coordinates-blue-x", "-d", "chromaticity-coordinates-blue-y"});
    } else {
        chroma_coord.append({"-s", "chromaticity-coordinates-red-x="+current_coord[red_x],
                             "-s", "chromaticity-coordinates-red-y="+current_coord[red_y],
                             "-s", "chromaticity-coordinates-green-x="+current_coord[green_x],
                             "-s", "chromaticity-coordinates-green-y="+current_coord[green_y],
                             "-s", "chromaticity-coordinates-blue-x="+current_coord[blue_x],
                             "-s", "chromaticity-coordinates-blue-y="+current_coord[blue_y]});
    }
    if (current_coord[white_x] == "") {
        white_coord.append({"-d", "white-coordinates-x", "-d","white-coordinates-y"});
    } else {
        white_coord.append({"-s", QString("white-coordinates-x=%1").arg(current_coord[white_x]), "-s", QString("white-coordinates-y=%2 ").arg(current_coord[white_y])});
    }
}

void
Encoder::hdrLum(const QString _hdr[], const QString &CE_MIN_LUM, const QString &CE_MAX_LUM, const QString &CE_MAX_CLL,
                const QString &CE_MAX_FALL, QStringList &max_lum, QStringList &min_lum, QStringList &max_cll,
                QStringList &max_fall) {
    if (CE_MAX_LUM != "") {                           // max lum
        max_lum.append({"-s", QString("max-luminance=%1").arg(CE_MAX_LUM)});
    } else {
        if (_hdr[CUR_MAX_LUM] != "") {
            max_lum.append({"-s", QString("max-luminance=%1").arg(_hdr[CUR_MAX_LUM])});
        } else {
            max_lum.append({"-d", "max-luminance"});
        }
    }

    if (CE_MIN_LUM != "") {                           // min lum
        min_lum.append({"-s", QString("min-luminance=%1").arg(CE_MIN_LUM)});
    } else {
        if (_hdr[CUR_MIN_LUM] != "") {
            min_lum.append({"-s", QString("min-luminance=%1").arg(_hdr[CUR_MIN_LUM])});
        } else {
            min_lum.append({"-d", "min-luminance"});
        }
    }

    if (CE_MAX_CLL != "") {                           // max cll
        max_cll.append({"-s", QString("max-content-light=%1").arg(CE_MAX_CLL)});
    } else {
        if (_hdr[CUR_MAX_CLL] != "") {
            max_cll.append({"-s", QString("max-content-light=%1").arg(_hdr[CUR_MAX_CLL])});
        } else {
            max_cll.append({"-d", "max-content-light"});
        }
    }

    if (CE_MAX_FALL != "") {                           // max fall
        max_fall.append({"-s", QString("max-frame-light=%1").arg(CE_MAX_FALL)});
    } else {
        if (_hdr[CUR_MAX_FALL] != "") {
            max_fall.append({"-s", QString("max-frame-light=%1").arg(_hdr[CUR_MAX_FALL])});
        } else {
            max_fall.append({"-d", "max-frame-light"});
        }
    }
}

void Encoder::hdrColorRange(const QString _hdr[], int CE_COLOR_RANGE, QStringList &color_range) {
    if (CE_COLOR_RANGE == 0) {                             // color range
        if (_hdr[CUR_COLOR_RANGE] == "Limited")
            color_range.append({"-color_range","tv"});
        else
        if (_hdr[CUR_COLOR_RANGE] == "Full")
            color_range.append({"-color_range","pc"});
    }
    else
    if (CE_COLOR_RANGE == 1) {
        color_range.append({"-color_range","pc"});
    }
    else
    if (CE_COLOR_RANGE == 2) {
        color_range.append({"-color_range","tv"});
    }
}

QString Encoder::getLog() const {
    QString log("");
    if (_flag_two_pass && _flag_hdr) {
        Print("preset_pass1: " << _preset_pass1.join(" ").toStdString());
        Print("preset: " << _preset.join(" ").toStdString());
        Print("preset_mkvpropedit: " << _preset_mkvmerge.toStdString());
        log = QString("Preset pass 1: %1 -i <input file> %2\n"
                      "Preset pass 2: %3 -i <input file> %4 -y <output file>\n"
                      "Preset mkvpropedit: %5\n")
                .arg(_preset_0.join(" "), _preset_pass1.join(" "), _preset_0.join(" "), _preset.join(" "), _preset_mkvmerge);
    }
    else
    if (_flag_two_pass && !_flag_hdr) {
        Print("preset_pass1: " << _preset_pass1.join(" ").toStdString());
        Print("preset: " << _preset.join(" ").toStdString());
        log = QString("Preset pass 1: %1 -i <input file> %2\n"
                      "Preset pass 2: %3 -i <input file> %4 -y <output file>\n")
                .arg(_preset_0.join(" "), _preset_pass1.join(" "), _preset_0.join(" "), _preset.join(" "));
    }
    else
    if (!_flag_two_pass && _flag_hdr) {
        Print("preset: " << _preset.join(" ").toStdString());
        Print("preset_mkvpropedit: " << _preset_mkvmerge.toStdString());
        log = QString("Preset: %1 -i <input file> %2 -y <output file>\n"
                      "Preset mkvpropedit: %3\n")
                .arg(_preset_0.join(" "), _preset.join(" "), _preset_mkvmerge);
    }
    else
    if (!_flag_two_pass && !_flag_hdr) {
        Print("preset: " << _preset.join(" ").toStdString());
        log = QString("Preset: %1 -i <input file> %2 -y <output file>\n")
                .arg(_preset_0.join(" "), _preset.join(" "));
    }
    return log;
}

void
Encoder::split(const double &_startTime, const double &_endTime, const double &_dur, int streamCutting, const Tables &t,
               int CE_CODEC, double fps_dest, double minExtTime, QStringList &_splitStartParam,
               QStringList &_splitParam) const {
    double duration = _endTime - _startTime;
    if (streamCutting == 1) {
        if (minExtTime > 0) {
            if (minExtTime < duration || minExtTime < _dur)
                duration = minExtTime;
            else
            if (_dur < minExtTime)
                duration = _dur;
        }
    }
    Print("Cut duration: " << duration);
    if (duration > 0) {
        *fr_count = rnd(duration * fps_dest);
        _splitStartParam.append(t.arr_codec[CE_CODEC][0] == tr("Source") ? QStringList{"-copyts"} : QStringList{"-copytb", "0"}); //-copytb 0
        _splitStartParam.append(QStringList{"-ss", QString::number(_startTime, 'f', 3)});
        _splitParam.append(QStringList{"-vframes", numToStr(*fr_count)});
    } else {
        *fr_count = rnd(_dur * fps_dest);
    }
}

void Encoder::codecModule(const Tables &t, int CE_CODEC, QString &hwaccel, QString &hwaccel_filter_vf) {
    hwaccel= t.arr_params[CE_CODEC][1];
    hwaccel_filter_vf= t.arr_params[CE_CODEC][3];
    _flag_hdr = static_cast<bool>(t.arr_params[CE_CODEC][2].toInt());
}

void Encoder::passModule(const Tables &t, int CE_CODEC, int CE_PASS, QStringList &pass, QStringList &pass1) {
    const QString selected_pass = t.arr_pass[CE_CODEC][CE_PASS];
    if (selected_pass == tr("2 Pass_x265")) {
        pass.append({"-x265-params","pass=2"});
        pass1.append({"-x265-params","pass=1"});
        _flag_two_pass = true;
    }
    else
    if (selected_pass == tr("2 Pass")) {
        pass.append({"-pass","2"});
        pass1.append({"-pass","1"});
        _flag_two_pass = true;
    }
    else
    if (selected_pass == tr("2 Pass Optimisation")) {
        pass.append({"-2pass","1"});
    }
}

QStringList Encoder::audioModule(const Tables &t, int CE_CODEC, int CE_AUDIO_CODEC, int CE_AUDIO_BITRATE, int CE_AUDIO_SAMPLING,
                                 int CE_AUDIO_CHANNELS) {
    QStringList acodec;
    const QString selected_acodec = t.arr_acodec[CE_CODEC][CE_AUDIO_CODEC];
    QString selected_bitrate = "";

    QStringList sampling;
    const QString selected_sampling = t.arr_sampling[CE_AUDIO_SAMPLING];
    if (selected_sampling != "Source") {
        sampling.append({"-af",QString("aresample=%1:resampler=soxr").arg(selected_sampling)});
    }

    QStringList channels;
    const QString selected_channels = t.arr_channels[CE_AUDIO_CHANNELS];
    if (selected_channels != "Source") {
        channels.append({"-ac", selected_channels});
    }

    if (selected_acodec == "AAC") {
        selected_bitrate = t.arr_bitrate[0][CE_AUDIO_BITRATE];
        acodec.append({"-c:a","aac", "-b:a", selected_bitrate});
    }
    else
    if (selected_acodec == "AC3") {
        selected_bitrate = t.arr_bitrate[1][CE_AUDIO_BITRATE];
        acodec.append({"-c:a", "ac3", "-b:a", selected_bitrate});
    }
    else
    if (selected_acodec == "DTS") {
        selected_bitrate = t.arr_bitrate[2][CE_AUDIO_BITRATE];
        acodec.append({"-strict", "-2", "-c:a", "dca", "-b:a", selected_bitrate});
    }
    else
    if (selected_acodec == "Vorbis") {
        selected_bitrate = t.arr_bitrate[3][CE_AUDIO_BITRATE];
        acodec.append({"-c:a", "libvorbis", "-b:a", selected_bitrate});
    }
    else
    if (selected_acodec == "Opus") {
        selected_bitrate = t.arr_bitrate[4][CE_AUDIO_BITRATE];
        acodec.append({"-c:a", "libopus", "-b:a", selected_bitrate});
    }
    else
    if (selected_acodec == "PCM 16 bit") {
        acodec.append({"-c:a", "pcm_s16le"});
    }
    else
    if (selected_acodec == "PCM 24 bit") {
        acodec.append({"-c:a", "pcm_s24le"});
    }
    else
    if (selected_acodec == "PCM 32 bit") {
        acodec.append({"-c:a", "pcm_s32le"});
    }
    else
    if (selected_acodec == tr("Source")) {
        acodec.append({"-c:a", "copy"});
    }
    const QStringList audio_param = sampling + acodec + channels;
    return audio_param;
}

void Encoder::subtVF(const QString &input_file, const QString &subtitle_font, int subtitle_font_size,
                     const QString &subtitle_font_color, const bool burn_background,
                     const QString &subtitle_background_color, int subtitle_location, Data &data,
                     QStringList &burn_subt_vf, const QString& width, const QString& height) {
    // Keep with a QString here as there are no spaces in the parameters.
    QString  burn_string;
    // DEBUG
    std::string fontcol = subtitle_font_color.toStdString();
    std::string bgcol = subtitle_background_color.toStdString();
    // Hard-coding UTF-8. Not very elegant....
    burn_string = "charenc=utf-8:force_style=\"'FontName='" + subtitle_font +
                          "',Fontsize=" + numToStr(subtitle_font_size) +
                          ",PrimaryColour=&H" + subtitle_font_color +
                          ",BorderStyle=4";
    if (burn_background) {
        burn_string += QString(",BackColour=&H" + subtitle_background_color);
    }

    // Location mapping to ffmpeg alignment
    int location = subtitle_location;
    switch (location)
    {
        case 0: // default
        case 1: // bottom left
        case 2: // bottom center
        case 3: // bottom right
            break;
        case 4: // top left
        case 5: // top center
        case 6: // top right
            location += 1;
            break;
        case 7: // middle left
        case 8: // middle center
        case 9: // middle right
            location += 2;
            break;
    }
    if (location > 0)
    {
        burn_string += QString(",Alignment="+ numToStr(location));
    }

    burn_string += QString("'\"");
    for (int k = 0; k < data.checks[Data::subtBurn].size(); k++) {
        if (data.checks[Data::subtBurn][k]) {
            std::string subtitleFormat = data.fields[Data::subtFormats][k].toStdString();
            // FIXME Hard-coded specific check for investigation.
            if ((subtitleFormat == "PGS") || (subtitleFormat == "VobSub"))
            {
                burn_subt_vf.append("-filter_complex");
                burn_subt_vf.append(QString("[0:v]scale="+width+":"+height+"[video];[0:s:%1]scale="+width+":"+height+"[sub];[video][sub]overlay[v]").arg(numToStr(k)));
                burn_subt_vf.append("-map");
                burn_subt_vf.append("[v]");
            }
            else
            {
                burn_subt_vf.append(QString("subtitles='%1':%2:stream_index=%3").arg(input_file, burn_string, numToStr(k)));
            }
            _burn_subtitle = true;
            break;
        }
    }
    for (int k = 0; k < data.checks[Data::externSubtBurn].size(); k++) {
        if (data.checks[Data::externSubtBurn][k]) {
            std::string subtitleFormat = data.fields[Data::externSubtFormats][k].toStdString();
            _burn_subtitle = true;
            burn_subt_vf.append(QString("subtitles='%1':%2").arg(data.fields[Data::externSubtPath][k], burn_string));
            break;
        }
    }
}

void Encoder::fpsVF(const QString &_fps, int CE_CODEC, int CE_FRAME_RATE, int CE_BLENDING, Tables &t, QString &fps_vf,
                    double &fps_dest) {
    // Keep with a QString here as there are no spaces in the parameters.
    fps_vf= "";
    if (t.frame_rate[CE_FRAME_RATE] != "Source") {
        fps_dest = t.frame_rate[CE_FRAME_RATE].toDouble();
        if (CE_BLENDING == t.Blending::Simple) {
            if (CE_CODEC >= CODEC_QSV_FIRST && CE_CODEC <= CODEC_QSV_LAST) // QSV
                fps_vf = QString("vpp_qsv=framerate=%1").arg(t.frame_rate[CE_FRAME_RATE]);
            else
                fps_vf = QString("fps=fps=%1").arg(t.frame_rate[CE_FRAME_RATE]);
        }
        else
        if (CE_BLENDING == Tables::Interpolated)
            fps_vf = QString("framerate=fps=%1").arg(t.frame_rate[CE_FRAME_RATE]);
        else
        if (CE_BLENDING == Tables::MCI)
            fps_vf = QString("minterpolate=fps=%1:mi_mode=mci:mc_mode=aobmc:me_mode=bidir:vsbmc=1")
                    .arg(t.frame_rate[CE_FRAME_RATE]);
        else
        if (CE_BLENDING == Tables::Blend)
            fps_vf = QString("minterpolate=fps=%1:mi_mode=blend").arg(t.frame_rate[CE_FRAME_RATE]);
    } else
        fps_dest = _fps.toDouble();
}

void Encoder::resizeVF(QString &_width, QString &_height, int CE_CODEC, int CE_WIDTH, int CE_HEIGHT, Tables &t,
                       QString &resize_vf) {
    // Keep with a QString here as there are no spaces in the parameters.
    resize_vf= "";
    const QString new_width = (t.arr_width[CE_WIDTH] != "Source") ? t.arr_width[CE_WIDTH] : _width;
    const QString new_height = (t.arr_height[CE_HEIGHT] != "Source") ? t.arr_height[CE_HEIGHT] : _height;
    if ((t.arr_width[CE_WIDTH] != "Source") || (t.arr_height[CE_HEIGHT] != "Source")) {
        if (CE_CODEC >= CODEC_QSV_FIRST && CE_CODEC <= CODEC_QSV_LAST) // QSV
            resize_vf = QString("scale_qsv=w=%1:h=%2,setsar=1:1").arg(new_width, new_height);
        else
        if (CE_CODEC >= CODEC_VAAPI_FIRST && CE_CODEC <= CODEC_VAAPI_LAST) // VAAPI
            resize_vf = QString("scale_vaapi=w=%1:h=%2,setsar=1:1").arg(new_width, new_height);
        else
            resize_vf = QString("scale=%1:%2,setsar=1:1").arg(new_width, new_height);
    }
    _width = new_width;
    _height = new_height;
}

void Encoder::encode()   // Encode
{
    Print("Encode ...");
    QStringList arguments;
    frames_processed = 0;
    tpf_slot = 0;
    time_per_frame_history[0] = -1;
    time_per_frame_history[1] = -1;
    _last_update = time(nullptr);
    processEncoding->disconnect();
    connect(processEncoding, SIGNAL(readyReadStandardOutput()), this, SLOT(progress_1()));
    connect(processEncoding, SIGNAL(finished(int)), this, SLOT(completed(int)));
    emit onEncodingProgress(0, 0.0f);

    QString escaped_file_in;
    std::string debug1 = _input_file.toStdString();
    std::string debug3 = _output_file.toStdString();
    QString escaped_file_out = Helper::makeFileStringFFMPEGReady(_output_file);
    std::string debug4 = escaped_file_out.toStdString();

    if (_mux_mode) {
        Print("Muxing mode ...");
        _encoding_mode = tr("Muxing:");
        emit onEncodingMode(_encoding_mode);
        escaped_file_in = Helper::makeFileStringFFMPEGReady(_temp_file);
        arguments << "-hide_banner" << "-i" << escaped_file_in << "-map" << "0:v:0?" << "-map" << "0:a?"
                  << "-map" << "0:s?" << "-movflags" << "+write_colr"
                  << "-c:v" << "copy" << "-c:a" << "copy" << _sub_mux_param
                  << "-threads" << numToStr(_threads)
                  << "-y" << escaped_file_out;
    } else {
        if (*fr_count == 0) {
            _message = tr("The file does not contain FPS information!\nSelect the correct input file!");
            emit onEncodingInitError(_message);
            return;
        }
        emit onEncodingStarted();

        escaped_file_in = Helper::makeFileStringFFMPEGReady(_input_file);
        std::string debug2 = escaped_file_in.toStdString();

        _loop_start = time(nullptr);
        if (!_flag_two_pass && !_flag_hdr) {
            Print("Encode non HDR...");
            _encoding_mode = tr("Encoding:");
            emit onEncodingMode(_encoding_mode);
            arguments << _preset_0 << "-i" << escaped_file_in
                      << _extAudioPaths
                      << _extSubPaths << _preset
                     << "-threads" << numToStr(_threads)
                      << "-y" << escaped_file_out;
        }
        else
        if (!_flag_two_pass && _flag_hdr) {
            Print("Encode HDR...");
            escaped_file_out = Helper::makeFileStringFFMPEGReady(_temp_file);
            _encoding_mode = tr("Encoding:");
            emit onEncodingMode(_encoding_mode);
            arguments << _preset_0 << "-i" << escaped_file_in
                      << _extAudioPaths
                      << _extSubPaths << _preset
                      << "-threads" << numToStr(_threads)
                      << "-y" << escaped_file_out;
        }
        else
        if (_flag_two_pass) {
            Print("Encode 1-st pass...");
            _encoding_mode = tr("1-st pass:");
            emit onEncodingMode(_encoding_mode);
            arguments << _preset_0 << "-y" << "-i" << escaped_file_in
                      << _extAudioPaths
                      << _extSubPaths
                      << "-threads" << numToStr(_threads)
                      << _preset_pass1;
        }
    }

    // Clean up empty slots.
    arguments.removeAll("");
    arguments.removeAll(" ");

    // Debug
    std::string args = arguments.join(" ").toStdString();
    std::cout << args;

    processEncoding->start("ffmpeg", arguments);
    if (!processEncoding->waitForStarted()) {
        Print("cmd command not found!!!");
        processEncoding->disconnect();
        _message = tr("An unknown error occurred!\n Possible FFMPEG not installed.\n");
            emit onEncodingInitError(_message);
    }
}

void Encoder::add_metadata() // Add metedata
{
    Print("Add metadata ...");
    _mux_mode = true;
    processEncoding->disconnect();
    connect(processEncoding, SIGNAL(readyReadStandardOutput()), this, SLOT(progress_2()));
    connect(processEncoding, SIGNAL(finished(int)), this, SLOT(encode()));
    _encoding_mode = tr("Add data:");
    emit onEncodingMode(_encoding_mode);
    emit onEncodingProgress(0, 0.0f);
    QStringList arguments;
    arguments << "--edit" << "track:1" << _preset_mkvmerge.split(" ") << _temp_file;
    processEncoding->start("mkvpropedit", arguments);
    if (!processEncoding->waitForStarted()) {
        Print("cmd command not found!!!");
        processEncoding->disconnect();
        _message = tr("An unknown error occurred!\n Possibly mkvtoolnix is not installed.\n");
        emit onEncodingInitError(_message);
    }
}

void Encoder::progress_1()   // Progress
{
    QString line = QString(processEncoding->readAllStandardOutput());
    const QString line_mod = line.replace("   ", " ").replace("  ", " ").replace("  ", " ").replace("= ", "=");
    emit onEncodingLog(line_mod);
    _error_message = line_mod;
    const int pos_st = line_mod.indexOf("frame=");
    if (pos_st == 0) {
        QStringList data = line_mod.split(" ");
        const QString frame_qstr = data[0].replace("frame=", "");
        int frame = frame_qstr.toInt();
        if (frame == 0)
            frame = 1;

        // Delta in time since we last reported progress.
        const time_t iter_start = time(nullptr);
        const int time_from_last_update = static_cast<int>(iter_start - _last_update);

        // Number of frames processed since last update.
        const int frames_done_in_last_period = frame - frames_processed;

        // Try to avoid reporting progress too frequently, which messes up the estimate.
        if (frames_done_in_last_period < 120)
        {
            return;
        }

        _last_update = iter_start;
        frames_processed = frame;

        // Time per frame, multiplied by remaining frames;
        const float last_interval_time_per_frame = (float)time_from_last_update / frames_done_in_last_period;
        time_per_frame_history[tpf_slot % 2] = last_interval_time_per_frame;
        float time_per_frame = last_interval_time_per_frame;
        if (tpf_slot >= 2)
        {
            time_per_frame = (time_per_frame_history[0] + time_per_frame_history[1]) * 0.5;
        }
        tpf_slot++;
        const int frames_remaining = *fr_count - frame;
        float rem_time = time_per_frame * frames_remaining;


        if (rem_time <= 0.0f)
            rem_time = 0.0f;
        if (rem_time > MAXIMUM_ALLOWED_TIME)
            rem_time = MAXIMUM_ALLOWED_TIME;

        float percent = static_cast<float>(frame * 100) / (*fr_count);
        int percent_int = rnd(percent);
        if (percent_int > 100)
            percent_int = 100;
        emit onEncodingProgress(percent_int, rem_time);
    }
}

void Encoder::progress_2()   // Progress mkvpropedit
{
    const QString line = QString(processEncoding->readAllStandardOutput());
    emit onEncodingLog(line);
    _error_message = line;
    const int pos_st = line.indexOf("Done.");
    const int pos_nf = line.indexOf("Nothing to do.");
    static bool lock = false;
    if ((pos_st != -1) || (pos_nf != -1)) {
        emit onEncodingProgress(100, 0.0f);
        if (!lock) {
            lock = true;
            _loop_start = time(nullptr);
        }
    }
}

QProcess::ProcessState Encoder::getEncodingState()
{
    return processEncoding->state();
}

void Encoder::pauseEncoding()
{
#ifdef Q_OS_WIN
    _PROCESS_INFORMATION *pi = processEncoding->pid();
    SuspendThread(pi->hThread);  // pause for Windows
#else
    kill(pid_t(processEncoding->processId()), SIGSTOP);  // pause for Unix
#endif
}

void Encoder::resumeEncoding()
{
#ifdef Q_OS_WIN
    _PROCESS_INFORMATION *pi = processEncoding->pid();
    ResumeThread(pi->hThread);  // resume for Windows
#else
    kill(pid_t(processEncoding->processId()), SIGCONT); // resume for Unix
#endif
}

void Encoder::stopEncoding()
{
    processEncoding->disconnect();
    connect(processEncoding, SIGNAL(finished(int)), this, SLOT(abort()));
    processEncoding->kill();
}

void Encoder::killEncoding()
{
    if (processEncoding->state() == QProcess::Running)
        processEncoding->kill();
}

void Encoder::completed(int exit_code)
{
    processEncoding->disconnect();
    if (exit_code == 0) {
        if (_mux_mode) {
            if (_flag_hdr)
                QDir().remove(_temp_file);
            emit onEncodingProgress(100, 0.0f);
            emit onEncodingCompleted();
        } else {
            if (!_flag_two_pass && _flag_hdr) {
                add_metadata();
            } else
            if (!_flag_two_pass && !_flag_hdr) {
                emit onEncodingProgress(100, 0.0f);
                emit onEncodingCompleted();
            } else
            if (_flag_two_pass) {
                _flag_two_pass = false;
                encode();
            }
        }
    } else {
        if (_flag_hdr)
            QDir().remove(_temp_file);
        emit onEncodingError(_error_message, false);
    }
}

void Encoder::abort()
{
    processEncoding->disconnect();
    if (_flag_hdr)
        QDir().remove(_temp_file);
    emit onEncodingAborted();
}
