/***********************************************************************

                          C I N E   E N C O D E R
                                JULY, 2020
                            COPYRIGHT (C) 2020

 FILE: encoderstream.cpp
 COMMENT:
 LICENSE: GNU General Public License v3.0

***********************************************************************/

#include "encoderstream.h"
#include "tables.h"
#include "helper.h"
#include <QDir>
#include <QMap>
#include <QOperatingSystemVersion>
#include <iostream>
#include <cmath>
#include <ctime>
#include <algorithm>

using namespace Constants;

#if defined(Q_OS_WIN64)
#include <windows.h>
#include <tlhelp32.h>
#endif

#define rnd(num) static_cast<int>(round(num))



EncoderStream::EncoderStream(QObject *parent) :
    QObject(parent),
    m_pData(nullptr)
{
    m_pProcessEncoding = new QProcess(this);
    m_pProcessEncoding->setProcessChannelMode(QProcess::MergedChannels);
    m_pProcessEncoding->setWorkingDirectory(QDir::homePath());
}

EncoderStream::~EncoderStream()
= default;

/************************************************
** Encoder
************************************************/

void EncoderStream::initEncoding(StreamData *data,
                                 EncoderAudioParam *aParam,
                                 EncoderSubtParam *sParam,
                                 int prio) {
    Print("Make preset...");
    Tables t;
    m_pData = data;
    int ACODEC = aParam->AUDIO_CODEC;
    int ABITRATE = aParam->AUDIO_BITRATE;
    int ASAMPLING = aParam->AUDIO_SAMPLING;
    int ACHANNELS = aParam->AUDIO_CHANNELS;
    QString ACONTAINER = aParam->AUDIO_CONTAINER;
    int SCODEC = sParam->SUBT_CODEC;
    QString SCONTAINER = sParam->SUBT_CONTAINER;
    _prio = prio;

    m_preset_0 = QStringList();
    m_preset = QStringList();
    m_error_message = "";
    m_error_lines.clear();  // Clear accumulated error lines for new encoding
    m_output_file = QFileInfo(data->output_file).absolutePath() + "/" +
                    QFileInfo(data->output_file).completeBaseName() +
                    "_" + numToStr(m_pData->stream) + ".";

    QStringList mapParam = QStringList();
    if (data->cont_type == ContentType::Audio) {
        m_output_file += ACONTAINER.toLower();
        mapParam.append("-map");
        mapParam.append(QString("0:a:%1?").arg(numToStr(m_pData->stream)));
    } else {
        m_output_file += SCONTAINER.toLower();
        mapParam.append(QString("-map"));
        mapParam.append(QString("0:s:%1?").arg(numToStr(m_pData->stream)));
    }

    /************************************* Audio module ***************************************/
    QStringList sampling("");
    {
        const QString selected_sampling = t.arr_sampling[ASAMPLING];
        if (selected_sampling != "Source") {
            sampling.append("-af");
            sampling.append(QString("aresample=%1:resampler=soxr").arg(selected_sampling));
        }
    }
    QStringList channels("");
    {
        const QString selected_channels = t.arr_channels[ACHANNELS];
        if (selected_channels != "Source") {
            channels.append("-ac");
            channels.append(QString("%1").arg(selected_channels));
        }
    }
    QStringList acodec("");
    {
        QString selected_bitrate("");
        const QString selected_acodec = t.arr_acodec_sep[ACODEC];
        if (selected_acodec == "Advanced Audio Coding") {
            selected_bitrate = t.arr_bitrate[0][ABITRATE];
            acodec.append("-strict");
            acodec.append("experimental");
            acodec.append("-c:a");
            acodec.append("aac");
            acodec.append("-b:a");
            acodec.append(QString("%1").arg(selected_bitrate));
        } else if (selected_acodec == "Dolby Digital") {
            selected_bitrate = t.arr_bitrate[1][ABITRATE];
            acodec.append("-c:a");
            acodec.append("ac3");
            acodec.append("-b:a");
            acodec.append(QString("%1").arg(selected_bitrate));
        } else if (selected_acodec == "Dolby TrueHD") {
            selected_bitrate = t.arr_bitrate[2][ABITRATE];
            acodec.append("-strict");
            acodec.append("-2");
            acodec.append("-c:a");
            acodec.append("dca");
            acodec.append("-b:a");
            acodec.append(QString("%1").arg(selected_bitrate));
        } else if (selected_acodec == "Vorbis") {
            selected_bitrate = t.arr_bitrate[3][ABITRATE];
            acodec.append("-c:a");
            acodec.append("libvorbis");
            acodec.append("-b:a");
            acodec.append(QString("%1").arg(selected_bitrate));
        } else if (selected_acodec == "Opus") {
            selected_bitrate = t.arr_bitrate[4][ABITRATE];
            acodec.append("-c:a");
            acodec.append("libopus");
            acodec.append("-b:a");
            acodec.append(QString("%1").arg(selected_bitrate));
        } else if (selected_acodec == "Pulse Code Modulation 16 bit") {
            acodec.append("-c:a");
            acodec.append("pcm_s16le");
        } else if (selected_acodec == "Pulse Code Modulation 24 bit") {
            acodec.append("-c:a");
            acodec.append("pcm_s24le");
        } else if (selected_acodec == "Pulse Code Modulation 32 bit") {
            acodec.append("-c:a");
            acodec.append("pcm_s32le");
        } else if (selected_acodec == tr("Source")) {
            acodec.append("-c:a");
            acodec.append(data->audioProcessing.isActive() ? "aac" : "copy");
        }
    }
    QStringList aparam = QStringList("-sn");
    if (data->cont_type == ContentType::Audio && data->audioProcessing.isActive()) {
        aparam.append("-af");
        aparam.append(data->audioProcessing.filterChain());
    }
    if (!sampling.empty()) {
        aparam.append(sampling);
    }
    if (!acodec.empty()) {
        aparam.append(acodec);
    }
    if (!channels.empty())
    {
        aparam.append(channels);
    }
    aparam.removeAll("");

    /************************************ Subtitle module *************************************/
    QStringList scodec("");
    {
        const QString selected_scodec = t.arr_scodec_sep[SCODEC];
        std::string selected_scodec_debug = selected_scodec.toStdString();
        if (selected_scodec == "SubRip") {
            scodec.append("-c:s");
            scodec.append("srt");
        }
        else
        if (selected_scodec == "WebVTT") {
            scodec.append("-c:s");
            scodec.append("webvtt");
        }
        else
        if (selected_scodec == "SubStation Alpha") {
            scodec.append("-c:s");
            scodec.append("ssa");
        }
        else
        if (selected_scodec == "Advanced SSA") {
            scodec.append("-c:s");
            scodec.append("ass");
        }
        else
        if (selected_scodec == "Timed Text") {
            scodec.append("-c:s");
            scodec.append("ttml");
        }
        else
        if (selected_scodec == "MOV text") {
            scodec.append("-c:s");
            scodec.append("mov_text");
        }
        else
        if (selected_scodec == tr("Source")) {
            scodec.append("-c:s");
            scodec.append("copy");
        }
    }
    QStringList sparam = QStringList("-an");
    sparam.append(scodec);
    sparam.removeAll("");

    /************************************* Result module ***************************************/
    m_preset_0.append("-hide_banner");
    m_preset_0.append("-probesize");
    m_preset_0.append("100M");
    m_preset_0.append("-analyzeduration");
    m_preset_0.append("50M");
    m_preset.append("-vn");
    
    // Handle metadata - include stream title if available
    if (!m_pData->title.isEmpty()) {
        // Export stream title as metadata for supported formats
        // Use stream-specific metadata which is more reliable for extraction
        if (data->cont_type == ContentType::Audio) {
            m_preset.append("-metadata:s:a:0");
            m_preset.append(QString("title=%1").arg(Helper::makeFileStringFFMPEGReady(m_pData->title)));
        } else {
            m_preset.append("-metadata:s:s:0");
            m_preset.append(QString("title=%1").arg(Helper::makeFileStringFFMPEGReady(m_pData->title)));
        }
        // Also set global title metadata as fallback
        m_preset.append("-metadata");
        m_preset.append(QString("title=%1").arg(Helper::makeFileStringFFMPEGReady(m_pData->title)));
        // Disable other global metadata but keep our custom metadata
        m_preset.append("-map_metadata");
        m_preset.append("-1");
    } else {
        // No title available, disable all metadata
        m_preset.append("-map_metadata");
        m_preset.append("-1");
    }
    
    m_preset.append("-map_chapters");
    m_preset.append("-1");
    m_preset.append(mapParam);
    if (data->cont_type == ContentType::Audio)
    {
        m_preset.append(aparam);
    }
    else
    {
        m_preset.append(sparam);
    }
    QString p0 = m_preset_0.join(" ");
    QString p = m_preset.join(" ");
    Print("preset_0: " << p0.toStdString());
    Print("preset: " << p.toStdString());
    QString log = QString("Preset: %1 -i <input file> %2 -y <output file>\n")
                    .arg(p0, p);
    std::cout << log.toStdString();
    emit onEncodingLog(log);
    encode();
}

void EncoderStream::encode()   // Encode
{
    Print("Encode ...");
    QStringList arguments;
    m_pProcessEncoding->disconnect();
    connect(m_pProcessEncoding, &QProcess::readyReadStandardOutput, this, &EncoderStream::progress);
    connect(m_pProcessEncoding, SIGNAL(finished(int)), this, SLOT(completed(int)));
    emit onEncodingProgress(0, 0.0f);

    if (m_pData->duration <= 0.f) {
        m_message = tr("The file does not contain duration information!\nSelect the correct input file!");
        emit onEncodingInitError(m_message);
        return;
    }
    emit onEncodingStarted();
    m_loop_start = time(nullptr);
    std::string debug1 = m_pData->input_file.toStdString();
    std::string debug2   = m_output_file.toStdString();
    arguments << m_preset_0 << "-i" << Helper::makeFileStringFFMPEGReady(m_pData->input_file)
              << m_preset << "-y" << Helper::makeFileStringFFMPEGReady(m_output_file);
    std::cout << arguments.join(" ").toStdString();
    //qDebug() << arguments;
    QString program;
    QOperatingSystemVersion ostype = QOperatingSystemVersion::current();
    if (ostype.type() == QOperatingSystemVersion::Windows)
    {
        program = "ffmpeg";
    }
    else
    {
        // Assume Linux - Qt doesn't report Linux directly.
        QString nicelevel;
        switch (_prio)
        {
            case Constants::lowest:
                nicelevel = "19";
                break;
            case Constants::low:
                nicelevel = "9";
                break;
            case Constants::normal:
            default:
                nicelevel = "0";
                break;
            case Constants::high:
                nicelevel = "-9";
                break;
            case Constants::highest:
                nicelevel = "-19";
                break;
        }
        program = "nice";
        QStringList new_args;
        new_args << "-n" << nicelevel << "ffmpeg";
        new_args.append(arguments);
        arguments = new_args;
    }
    m_pProcessEncoding->start(program, arguments);
    if (!m_pProcessEncoding->waitForStarted()) {
        Print("cmd command not found!!!");
        m_pProcessEncoding->disconnect();
        m_message = tr("An unknown error occurred!\n Possible FFMPEG not installed.\n");
        emit onEncodingInitError(m_message);
    }

#if defined(Q_OS_WIN64)
    set_process_prio_win();
#endif
}

// Completely untested.
#if defined(Q_OS_WIN64)
void EncoderStream::set_process_prio_win()
{
    // Get the process handle
    HANDLE hProcess = OpenProcess(PROCESS_SET_INFORMATION, FALSE, m_pProcessEncoding->processId());
    if (hProcess == nullptr) {
        qWarning("Failed to open process");
        return;
    }

    auto nicelevel = NORMAL_PRIORITY_CLASS;
    switch (_prio)
    {
        case Constants::lowest:
            nicelevel = IDLE_PRIORITY_CLASS;
            break;
        case Constants::low:
            nicelevel = BELOW_NORMAL_PRIORITY_CLASS;
            break;
        case Constants::normal:
        default:
            nicelevel = NORMAL_PRIORITY_CLASS;
            break;
        case Constants::high:
            nicelevel = ABOVE_NORMAL_PRIORITY_CLASS;
            break;
        case Constants::highest:
            nicelevel = HIGH_PRIORITY_CLASS;
            break;
    }

    // Set the priority class
    if (!SetPriorityClass(hProcess, nicelevel)) {
        qWarning("Failed to set process priority");
    }

    // Close the process handle
    CloseHandle(hProcess);
}
#endif

void EncoderStream::progress()   // Progress
{
    QString line = QString(m_pProcessEncoding->readAllStandardOutput());
    const QString line_mod = line.replace("   ", " ").replace("  ", " ").replace("  ", " ").replace("= ", "=");
    emit onEncodingLog(line_mod);
    
    // Accumulate potential error lines for better error reporting
    // Look for lines that indicate errors, warnings, or important status information
    if (line_mod.contains("error", Qt::CaseInsensitive) || 
        line_mod.contains("failed", Qt::CaseInsensitive) ||
        line_mod.contains("invalid", Qt::CaseInsensitive) ||
        line_mod.contains("unable", Qt::CaseInsensitive) ||
        line_mod.contains("cannot", Qt::CaseInsensitive) ||
        line_mod.contains("not found", Qt::CaseInsensitive) ||
        line_mod.contains("no such", Qt::CaseInsensitive) ||
        line_mod.contains("unrecognized", Qt::CaseInsensitive) ||
        line_mod.contains("unknown", Qt::CaseInsensitive) ||
        line_mod.contains("could not", Qt::CaseInsensitive)) {
        
        QString cleanLine = line_mod.trimmed();
        if (!cleanLine.isEmpty() && !m_error_lines.contains(cleanLine)) {
            m_error_lines.append(cleanLine);
            // Keep only the last 20 error lines to avoid memory bloat
            if (m_error_lines.size() > 20) {
                m_error_lines.removeFirst();
            }
        }
    }
    
    // Store the most recent line for backward compatibility
    m_error_message = line_mod;
    
    const int pos_st = line_mod.indexOf("time=");
    if (pos_st != -1) {
        const QString data = line_mod.split("time=").at(1);
        const QString data_mod = data.split(' ').at(0);
        const QStringList data_mod_2 = data_mod.split(':');
        const float h_cur = data_mod_2.at(0).toFloat();
        const float m_cur = data_mod_2.at(1).toFloat();
        const float s_cur = data_mod_2.at(2).toFloat();
        float dur = 3600.f*h_cur + 60.f*m_cur + s_cur;
        if (dur <= 0.0f)
            dur = 0.001;
        const time_t iter_start = time(nullptr);
        const int timer = static_cast<int>(iter_start - m_loop_start);
        const float full_time = static_cast<float>(timer * m_pData->duration) / dur;
        float rem_time = full_time - static_cast<float>(timer);
        if (rem_time < 0.0f)
            rem_time = 0.0f;
        if (rem_time > MAXIMUM_ALLOWED_TIME)
            rem_time = MAXIMUM_ALLOWED_TIME;

        float percent = static_cast<float>(dur * 100.f) / m_pData->duration;
        int percent_int = rnd(percent);
        if (percent_int > 100)
            percent_int = 100;
        emit onEncodingProgress(percent_int, rem_time);
    }
}

QProcess::ProcessState EncoderStream::getEncodingState()
{
    return m_pProcessEncoding->state();
}

void EncoderStream::pauseEncoding()
{
#ifdef Q_OS_WIN
    auto processID = m_pProcessEncoding->processId();
    HANDLE hProcess = OpenProcess(PROCESS_SUSPEND_RESUME, FALSE, processID);

    if (hProcess == NULL) {
        qDebug() << "Failed to open process for suspension";
        return;
    }

    // Iterate through the threads of the process
    HANDLE hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (hThreadSnap == INVALID_HANDLE_VALUE) {
        CloseHandle(hProcess);
        return;
    }

    THREADENTRY32 te;
    te.dwSize = sizeof(THREADENTRY32);

    if (Thread32First(hThreadSnap, &te)) {
        do {
            if (te.th32OwnerProcessID == processID) {
                HANDLE hThread = OpenThread(THREAD_SUSPEND_RESUME, FALSE, te.th32ThreadID);
                if (hThread) {
                    SuspendThread(hThread);
                    CloseHandle(hThread);
                }
            }
        } while (Thread32Next(hThreadSnap, &te));
    }

    CloseHandle(hThreadSnap);
    CloseHandle(hProcess);
#else
    kill(pid_t(m_pProcessEncoding->processId()), SIGSTOP);  // pause for Unix
#endif
}

void EncoderStream::resumeEncoding()
{
#ifdef Q_OS_WIN
    auto processID = m_pProcessEncoding->processId();
    HANDLE hProcess = OpenProcess(PROCESS_SUSPEND_RESUME, FALSE, processID);
    if (hProcess == NULL) {
        qDebug() << "Failed to open process for resumption";
        return;
    }

    // Iterate through the threads of the process
    HANDLE hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (hThreadSnap == INVALID_HANDLE_VALUE) {
        CloseHandle(hProcess);
        return;
    }

    THREADENTRY32 te;
    te.dwSize = sizeof(THREADENTRY32);

    if (Thread32First(hThreadSnap, &te)) {
        do {
            if (te.th32OwnerProcessID == processID) {
                HANDLE hThread = OpenThread(THREAD_SUSPEND_RESUME, FALSE, te.th32ThreadID);
                if (hThread) {
                    ResumeThread(hThread);
                    CloseHandle(hThread);
                }
            }
        } while (Thread32Next(hThreadSnap, &te));
    }

    CloseHandle(hThreadSnap);
    CloseHandle(hProcess);
#else
    kill(pid_t(m_pProcessEncoding->processId()), SIGCONT); // resume for Unix
#endif
}

void EncoderStream::stopEncoding()
{
    m_pProcessEncoding->disconnect();
    connect(m_pProcessEncoding, SIGNAL(finished(int)), this, SLOT(abort()));
    m_pProcessEncoding->kill();
}

void EncoderStream::killEncoding()
{
    if (m_pProcessEncoding->state() == QProcess::Running)
        m_pProcessEncoding->kill();
}

void EncoderStream::completed(int exit_code)
{
    m_pProcessEncoding->disconnect();
    if (exit_code == 0) {
        emit onEncodingProgress(100, 0.0f);
        emit onEncodingCompleted();
    } else {
        // Provide a comprehensive error message with accumulated error information
        QString detailedError;
        if (!m_error_lines.isEmpty()) {
            detailedError = tr("FFmpeg encoding failed with exit code %1:\n\n").arg(exit_code);
            detailedError += tr("Error details:\n");
            detailedError += m_error_lines.join("\n");
        } else if (!m_error_message.trimmed().isEmpty()) {
            // Fallback to last message if no specific errors were captured
            detailedError = tr("FFmpeg encoding failed with exit code %1:\n\n").arg(exit_code);
            detailedError += m_error_message;
        } else {
            // Generic error message
            detailedError = tr("FFmpeg encoding failed with exit code %1.\n\nNo detailed error information available.").arg(exit_code);
        }
        
        emit onEncodingError(detailedError);
    }
}

void EncoderStream::abort()
{
    m_pProcessEncoding->disconnect();
    emit onEncodingAborted();
}

long long EncoderStream::getPid()
{
    if (m_pProcessEncoding->state() != QProcess::NotRunning) {
        return -1;
    }
    return m_pProcessEncoding->processId();
}
