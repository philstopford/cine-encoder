/***********************************************************************

                          C I N E   E N C O D E R
                                JULY, 2020
                            COPYRIGHT (C) 2020

 FILE: encoder.h
 COMMENT:
 LICENSE: GNU General Public License v3.0

***********************************************************************/

#ifndef ENCODER_H
#define ENCODER_H

#include <QObject>
#include <QProcess>
#include <QVector>
#include "constants.h"
#include "tables.h"


#if defined (Q_OS_UNIX)
    #include <unistd.h>
    #include <csignal>
#elif defined(Q_OS_WIN64)
    #include <windows.h>
#endif

class Encoder : public QObject
{
    Q_OBJECT
public:
    explicit Encoder(QObject *parent = nullptr);
    ~Encoder() override;

    void initEncoding(const QString  &temp_file,
                      const QString  &input_file,
                      const QString  &output_file,
                      QString  &_width,
                      QString  &_height,
                      const QString  &_fps,
                      const double   &_startTime,
                      const double   &_endTime,
                      const double   &_dur,
                      const QString  &container,
                      QString        globalTitle,
                      QVector<QString>  _cur_param,
                      const QString  _hdr[],
                      Constants::Data           data,
                      int            *_fr_count,
                      int            streamCutting,
                      const QString  &subtitle_font,
                      int            subtitle_font_size,
                      const QString  &subtitle_font_color,
                      bool     burn_background,
                      const QString  &subtitle_background_color,
                      int            subtitle_location,
                      int threads,
                      int prio
                      );

    QProcess::ProcessState getEncodingState();
    void pauseEncoding();
    void resumeEncoding();
    void stopEncoding();
    void killEncoding();
    long long getPid();

    signals:
    void onEncodingMode(const QString &mode);
    void onEncodingStarted();
    void onEncodingInitError(const QString &_message);
    void onEncodingProgress(int percent, float rem_time);
    void onEncodingLog(const QString &log);
    void onEncodingAborted();
    void onEncodingError(const QString &_error_message, bool popup = false);
    void onEncodingCompleted();

private:
    bool    _flag_two_pass,
            _flag_hdr,
            _burn_subtitle,
            _mux_mode;

    int     *fr_count,
            _threads,
            _prio;

    int     frames_processed, tpf_slot;

    // Two slot array for moving average computation.
    float time_per_frame_history[2];

    time_t  _loop_start, _last_update;

    QStringList _extAudioPaths,
                _extSubPaths,
                _chaptersInput;
    QStringList _preset_0;
    QString _temp_file,
            _input_file,
            _output_file,
            _preset_mkvmerge,
            _message,
            _error_message,
            _encoding_mode;
    QStringList _error_lines;  // Accumulate error lines for better error reporting

    QStringList _preset, _preset_pass1, _sub_mux_param;

    QProcess *processEncoding;

private slots:
    void encode();
    void add_metadata();
    void progress_1();
    void progress_2();
    void completed(int exit_code);
    void abort();
#if defined(Q_OS_WIN64)
    void set_process_prio_win();
#endif
#if defined(Q_OS_MACOS)
    void set_process_prio_macos();
#endif

    static void
    resizeVF(QString &_width, QString &_height, int CE_CODEC, int CE_WIDTH, int CE_HEIGHT, Tables &t,
             QString &resize_vf) ;
    static void
    fpsVF(const QString &_fps, int CE_CODEC, int CE_FRAME_RATE, int CE_BLENDING, Tables &t, QString &fps_vf,
          double &fps_dest) ;

    void subtVF(const QString &input_file, const QString &subtitle_font, int subtitle_font_size,
                const QString &subtitle_font_color, bool burn_background,
                const QString &subtitle_background_color,
                int subtitle_location, Constants::Data &data, QStringList &burn_subt_vf, const QString& width, const QString& height);

    static QStringList
    audioModule(const Tables &t, int CE_CODEC, int CE_AUDIO_CODEC, int CE_AUDIO_BITRATE, int CE_AUDIO_SAMPLING,
                int CE_AUDIO_CHANNELS) ;

    void passModule(const Tables &t, int CE_CODEC, int CE_PASS, QStringList &pass, QStringList &pass1);

    void codecModule(const Tables &t, int CE_CODEC, QString &hwaccel, QString &hwaccel_filter_vf);

    void split(const double &_startTime, const double &_endTime, const double &_dur, int streamCutting, const Tables &t,
               int CE_CODEC, double fps_dest, double minExtTime, QStringList &_splitStartParam,
               QStringList &_splitParam) const;

    [[nodiscard]] QString getLog() const;

    static void hdrColorRange(const QString _hdr[], int CE_COLOR_RANGE, QStringList &color_range) ;

    static void hdrLum(const QString _hdr[], const QString &CE_MIN_LUM, const QString &CE_MAX_LUM, const QString &CE_MAX_CLL,
                const QString &CE_MAX_FALL, QStringList &max_lum, QStringList &min_lum, QStringList &max_cll,
                QStringList &max_fall) ;

    void
    hdrDisplay(const QString _hdr[], int CE_MASTER_DISPLAY, const QString &CE_CHROMA_COORD,
               const QString &CE_WHITE_COORD,
               QStringList &chroma_coord, QStringList &white_coord);

    static QStringList modeModule(const Tables &t, int CE_CODEC, int _CE_MODE, const QString &CE_BQR, const QString &CE_MINRATE,
                           const QString &CE_MAXRATE, const QString &CE_BUFSIZE) ;

    QStringList subModule(const QString &container);

    void colorPrimaries(const QString _hdr[], int CE_PRIMARY, int CE_REP_PRIM, QStringList &colorprim,
                        QStringList &colorprim_vf);

    int extAudio(Constants::Data &data, QStringList &_audioMapParam, QStringList &_audioMetadataParam, int audioNum);

    static void audio(Constants::Data &data, QStringList &_audioMapParam, QStringList &_audioMetadataParam, int &audioNum) ;

    static QStringList presetModule(const Tables &t, int CE_CODEC, int CE_PRESET) ;

    void extSub(Constants::Data &data, int extTrackNum, QStringList &_subtitleMapParam, QStringList &_subtitleMetadataParam,
                QStringList &_subtitleFormatParam,
                int subtNum);

    void
    colorTransfer(const QString _hdr[], int CE_TRC, int CE_REP_TRC, QStringList &transfer, QStringList &transfer_vf);

    void colorMatrix(const QString hdr[], int CE_MATRIX, int CE_REP_MATRIX, QStringList &colormatrix,
                     QStringList &colormatrix_vf);

    static QStringList levelModule(const Tables &t, int CE_CODEC, int CE_LEVEL) ;

    void subtitles(const QString &input_file, const QString &subtitle_font, int subtitle_font_size,
                   const QString &subtitle_font_color, bool burn_background,
                   const QString &subtitle_background_color,
                   int subtitle_location, Constants::Data &data, QStringList &burn_subt_vf, QString& width, QString& height, QStringList &_subtitleMapParam,
                   QStringList &_subtitleMetadataParam,
                   QStringList &_subtitleFormatParam, int &subtNum);

    [[nodiscard]] QStringList getCodec(const Tables &t, int CE_CODEC, const QString &resize_vf, const QString &fps_vf,
                         const QStringList &_videoMetadataParam, const QStringList &_audioMapParam,
                         const QStringList &_audioMetadataParam, const QStringList &burn_subt_vf,
                         const QStringList &_subtitleMapParam, const QStringList &_subtitleMetadataParam,
                         const QStringList &_subtitleFormatParam,
                         const QString &hwaccel_filter_vf, const QStringList &colorprim_vf,
                         const QStringList &colormatrix_vf, const QStringList &transfer_vf, 
                         const QString &chaptersFile, int chaptersInputIndex) const;

    void getPresets(const QStringList &_splitStartParam, const QStringList &_splitParam, const QString &hwaccel,
                    const QStringList &level, const QStringList &mode, const QStringList &preset,
                    const QStringList &pass,
                    const QStringList &pass1, const QStringList &audio_param,
                    const QStringList &colorprim, const QStringList &colormatrix, const QStringList &transfer,
                    const QStringList &codec, const QStringList &color_range, const QStringList &max_lum,
                    const QStringList &min_lum, const QStringList &max_cll, const QStringList &max_fall,
                    const QStringList &chroma_coord, const QStringList &white_coord);

    static Constants::Data &video(QString &globalTitle, Constants::Data &data, QVector<QString> &videoMetadata,
                QStringList &_videoMetadataParam) ;

    void initVariables(const QString &temp_file, const QString &input_file, const QString &output_file,
                       QVector<QString> &_cur_param, int *_fr_count, Tables &t, int &CE_CODEC, int &_CE_MODE,
                       QString &CE_BQR, QString &CE_MINRATE, QString &CE_MAXRATE, QString &CE_BUFSIZE, int &CE_LEVEL,
                       int &CE_FRAME_RATE, int &CE_BLENDING, int &CE_WIDTH, int &CE_HEIGHT, int &CE_PASS, int &CE_PRESET,
                       int &CE_COLOR_RANGE, int &CE_MATRIX, int &CE_PRIMARY, int &CE_TRC, QString &CE_MIN_LUM, QString &CE_MAX_LUM,
                       QString &CE_MAX_CLL, QString &CE_MAX_FALL, int &CE_MASTER_DISPLAY, QString &CE_CHROMA_COORD,
                       QString &CE_WHITE_COORD, int &CE_AUDIO_CODEC, int &CE_AUDIO_BITRATE, int &CE_AUDIO_SAMPLING,
                       int &CE_AUDIO_CHANNELS, int &CE_REP_PRIM, int &CE_REP_MATRIX, int &CE_REP_TRC,
                       int &CE_USE_PRESET_SUBTITLES, QString &CE_SUBTITLE_FONT, int &CE_SUBTITLE_FONT_SIZE,
                       QString &CE_SUBTITLE_FONT_COLOR, int &CE_SUBTITLE_BACKGROUND,
                       QString &CE_SUBTITLE_BACKGROUND_COLOR, int &CE_SUBTITLE_LOCATION);

};

#endif // ENCODER_H
