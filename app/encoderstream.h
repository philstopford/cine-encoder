/***********************************************************************

                          C I N E   E N C O D E R
                                JULY, 2020
                            COPYRIGHT (C) 2020

 FILE: encoderstream.h
 COMMENT:
 LICENSE: GNU General Public License v3.0

***********************************************************************/

#ifndef ENCODERSTREAM_H
#define ENCODERSTREAM_H

#include <QObject>
#include <QProcess>
#include <QVector>
#include "constants.h"


#if defined (__linux__)
    #include <unistd.h>
    #include <csignal>
#elif defined(__WIN64__)
    #include <windows.h>
#endif

using namespace Constants;

class EncoderStream : public QObject
{
    Q_OBJECT
public:
    explicit EncoderStream(QObject *parent = nullptr);
    ~EncoderStream() override;

    void initEncoding(StreamData *data,
                      EncoderAudioParam *aParam,
                      EncoderSubtParam *sParam,
                      int prio);

    QProcess::ProcessState getEncodingState();
    void pauseEncoding();
    void resumeEncoding();
    void stopEncoding();
    void killEncoding();
    long long getPid();

    signals:
    void onEncodingStarted();
    void onEncodingInitError(const QString &_message);
    void onEncodingProgress(int percent, float rem_time);
    void onEncodingLog(const QString &log);
    void onEncodingAborted();
    void onEncodingError(const QString &_error_message, bool popup = false);
    void onEncodingCompleted();

private:
    int _prio;
    time_t  m_loop_start;
    QStringList m_preset_0,
            m_preset;
    QString m_output_file,
            m_message,
            m_error_message;

    StreamData *m_pData;
    QProcess   *m_pProcessEncoding;

private slots:
    void encode();
    void progress();
    void completed(int);
    void abort();
#if defined(Q_OS_WIN64)
    static void set_process_prio_win();
#endif
};

#endif // ENCODERSTREAM_H
