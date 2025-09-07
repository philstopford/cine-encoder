/***********************************************************************

                          C I N E   E N C O D E R
                                JULY, 2020
                            COPYRIGHT (C) 2020

 FILE: inputvalidator.h
 COMMENT: Input validation and sanitization utilities
 LICENSE: GNU General Public License v3.0

***********************************************************************/

#ifndef INPUTVALIDATOR_H
#define INPUTVALIDATOR_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QRegularExpression>
#include <QFileInfo>
#include <QDir>

class InputValidator : public QObject
{
    Q_OBJECT

public:
    struct ValidationResult {
        bool isValid = false;
        QString errorMessage;
        QString sanitizedValue;
        
        explicit operator bool() const { return isValid; }
    };

    static ValidationResult validateFilePath(const QString &path, bool mustExist = false);
    static ValidationResult validateDirectoryPath(const QString &path, bool mustExist = false);
    static ValidationResult validateFileName(const QString &fileName);
    static ValidationResult validateNumber(const QString &value, double min = 0, double max = 999999);
    static ValidationResult validateInteger(const QString &value, int min = 0, int max = 999999);
    static ValidationResult validateResolution(const QString &width, const QString &height);
    static ValidationResult validateBitrate(const QString &bitrate);
    static ValidationResult validateFrameRate(const QString &fps);
    static ValidationResult validateTimeValue(const QString &time);
    
    // Sanitization methods
    static QString sanitizeFilePath(const QString &path);
    static QString sanitizeFileName(const QString &fileName);
    static QString sanitizeFFmpegParameter(const QString &param);
    static QStringList sanitizeFFmpegParameters(const QStringList &params);
    
    // File extension and codec validation
    static bool isValidVideoExtension(const QString &extension);
    static bool isValidAudioExtension(const QString &extension);
    static bool isValidSubtitleExtension(const QString &extension);
    static bool isCodecCompatibleWithContainer(const QString &codec, const QString &container);
    
private:
    static const QStringList& getValidVideoExtensions();
    static const QStringList& getValidAudioExtensions();
    static const QStringList& getValidSubtitleExtensions();
    static const QRegularExpression& getFileNameRegex();
    static const QRegularExpression& getNumberRegex();
    static const QRegularExpression& getTimeRegex();
};

// Convenience macros for validation
#define VALIDATE_FILE(path, mustExist) InputValidator::validateFilePath(path, mustExist)
#define VALIDATE_DIR(path, mustExist) InputValidator::validateDirectoryPath(path, mustExist)
#define VALIDATE_NUMBER(value, min, max) InputValidator::validateNumber(value, min, max)
#define SANITIZE_FFMPEG(param) InputValidator::sanitizeFFmpegParameter(param)

#endif // INPUTVALIDATOR_H