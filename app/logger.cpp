/***********************************************************************

                          C I N E   E N C O D E R
                                JULY, 2020
                            COPYRIGHT (C) 2020

 FILE: logger.cpp
 COMMENT: Centralized logging system implementation
 LICENSE: GNU General Public License v3.0

***********************************************************************/

#include "logger.h"
#include "constants.h"
#include <QStandardPaths>

using namespace Constants;

Logger::Logger(QObject *parent) : QObject(parent)
{
    // Set up default log file path
    const QString logDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(logDir);
    m_logFilePath = logDir + "/cine-encoder.log";
}

Logger::~Logger()
{
    if (m_logFile.isOpen()) {
        m_logFile.close();
    }
}

Logger& Logger::instance()
{
    static Logger logger;
    return logger;
}

void Logger::debug(const QString &message)
{
    instance().log(Level::Debug, message);
}

void Logger::info(const QString &message)
{
    instance().log(Level::Info, message);
}

void Logger::warning(const QString &message)
{
    instance().log(Level::Warning, message);
}

void Logger::error(const QString &message)
{
    instance().log(Level::Error, message);
}

void Logger::critical(const QString &message)
{
    instance().log(Level::Critical, message);
}

void Logger::log(Level level, const QString &message)
{
    // Check if this level should be logged
    if (level < m_level) {
        return;
    }
    
    const QString formattedMessage = formatMessage(level, message);
    
    // Always output to console in debug builds
    #ifdef QT_DEBUG
        qDebug().noquote() << formattedMessage;
    #else
        // In release builds, only show warnings and errors in console
        if (level >= Level::Warning) {
            qDebug().noquote() << formattedMessage;
        }
    #endif
    
    // Write to file if enabled
    if (m_fileLoggingEnabled && !m_logFilePath.isEmpty()) {
        if (!m_logFile.isOpen()) {
            m_logFile.setFileName(m_logFilePath);
            if (m_logFile.open(QIODevice::WriteOnly | QIODevice::Append)) {
                m_logStream.setDevice(&m_logFile);
            }
        }
        
        if (m_logFile.isOpen()) {
            m_logStream << formattedMessage << Qt::endl;
            m_logStream.flush();
        }
    }
}

QString Logger::levelToString(Level level) const
{
    switch (level) {
        case Level::Debug:    return "DEBUG";
        case Level::Info:     return "INFO";
        case Level::Warning:  return "WARN";
        case Level::Error:    return "ERROR";
        case Level::Critical: return "CRITICAL";
        default:              return "UNKNOWN";
    }
}

QString Logger::formatMessage(Level level, const QString &message) const
{
    const QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
    return QString("[%1] [%2] %3").arg(timestamp, levelToString(level), message);
}

void Logger::enableFileLogging(const QString &filePath)
{
    if (!filePath.isEmpty()) {
        m_logFilePath = filePath;
    }
    m_fileLoggingEnabled = true;
    
    // Close existing file if open
    if (m_logFile.isOpen()) {
        m_logFile.close();
    }
    
    LOG_INFO("File logging enabled: " + m_logFilePath);
}

void Logger::disableFileLogging()
{
    m_fileLoggingEnabled = false;
    if (m_logFile.isOpen()) {
        m_logFile.close();
    }
    LOG_INFO("File logging disabled");
}

void Logger::setLevel(Level level)
{
    m_level = level;
    LOG_INFO(QString("Log level set to: %1").arg(levelToString(level)));
}