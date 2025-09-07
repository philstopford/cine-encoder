/***********************************************************************

                          C I N E   E N C O D E R
                                JULY, 2020
                            COPYRIGHT (C) 2020

 FILE: logger.h
 COMMENT: Centralized logging system for debugging and maintenance
 LICENSE: GNU General Public License v3.0

***********************************************************************/

#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QString>
#include <QDebug>
#include <QDateTime>
#include <QTextStream>
#include <QFile>
#include <QDir>

class Logger : public QObject
{
    Q_OBJECT

public:
    enum class Level {
        Debug,
        Info,
        Warning,
        Error,
        Critical
    };

    static Logger& instance();
    
    static void debug(const QString &message);
    static void info(const QString &message);
    static void warning(const QString &message);
    static void error(const QString &message);
    static void critical(const QString &message);
    
    void enableFileLogging(const QString &filePath = "");
    void disableFileLogging();
    void setLevel(Level level);

private:
    explicit Logger(QObject *parent = nullptr);
    ~Logger() override;
    
    void log(Level level, const QString &message);
    QString levelToString(Level level) const;
    QString formatMessage(Level level, const QString &message) const;
    
    Level m_level = Level::Info;
    bool m_fileLoggingEnabled = false;
    QString m_logFilePath;
    QFile m_logFile;
    QTextStream m_logStream;
};

// Convenience macros for logging
#define LOG_DEBUG(msg) Logger::debug(msg)
#define LOG_INFO(msg) Logger::info(msg)
#define LOG_WARNING(msg) Logger::warning(msg)
#define LOG_ERROR(msg) Logger::error(msg)
#define LOG_CRITICAL(msg) Logger::critical(msg)

#endif // LOGGER_H