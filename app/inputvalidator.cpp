/***********************************************************************

                          C I N E   E N C O D E R
                                JULY, 2020
                            COPYRIGHT (C) 2020

 FILE: inputvalidator.cpp
 COMMENT: Input validation and sanitization implementation
 LICENSE: GNU General Public License v3.0

***********************************************************************/

#include "inputvalidator.h"
#include "logger.h"
#include <QFileInfo>
#include <QDir>
#include <QRegularExpression>

const QStringList& InputValidator::getValidVideoExtensions()
{
    static const QStringList extensions = {
        "mp4", "avi", "mkv", "mov", "wmv", "flv", "webm", "m4v", "mpg", "mpeg",
        "3gp", "asf", "rm", "rmvb", "ts", "m2ts", "mts", "vob", "ogv", "divx"
    };
    return extensions;
}

const QStringList& InputValidator::getValidAudioExtensions()
{
    static const QStringList extensions = {
        "mp3", "aac", "ogg", "wav", "flac", "m4a", "wma", "ac3", "dts", "opus",
        "amr", "aiff", "au", "ra", "ape", "tak"
    };
    return extensions;
}

const QStringList& InputValidator::getValidSubtitleExtensions()
{
    static const QStringList extensions = {
        "srt", "ass", "ssa", "sub", "idx", "vtt", "sup", "pgs", "txt"
    };
    return extensions;
}

const QRegularExpression& InputValidator::getFileNameRegex()
{
    // Allow alphanumeric, spaces, dots, dashes, underscores, parentheses
    static const QRegularExpression regex(R"([a-zA-Z0-9\s\.\-_\(\)]+)");
    return regex;
}

const QRegularExpression& InputValidator::getNumberRegex()
{
    static const QRegularExpression regex(R"(^\d*\.?\d+$)");
    return regex;
}

const QRegularExpression& InputValidator::getTimeRegex()
{
    // Format: HH:MM:SS.mmm or MM:SS.mmm or SS.mmm
    static const QRegularExpression regex(R"(^(?:(\d{1,2}):)?(?:(\d{1,2}):)?(\d{1,2})(?:\.(\d{1,3}))?$)");
    return regex;
}

InputValidator::ValidationResult InputValidator::validateFilePath(const QString &path, bool mustExist)
{
    ValidationResult result;
    
    if (path.isEmpty()) {
        result.errorMessage = "File path cannot be empty";
        return result;
    }
    
    const QString sanitized = sanitizeFilePath(path);
    if (sanitized.isEmpty()) {
        result.errorMessage = "File path contains invalid characters";
        return result;
    }
    
    const QFileInfo fileInfo(sanitized);
    
    if (mustExist && !fileInfo.exists()) {
        result.errorMessage = "File does not exist: " + sanitized;
        return result;
    }
    
    if (mustExist && !fileInfo.isFile()) {
        result.errorMessage = "Path is not a file: " + sanitized;
        return result;
    }
    
    // Check if parent directory is writable (for output files)
    if (!mustExist) {
        const QDir parentDir = fileInfo.absoluteDir();
        if (!parentDir.exists()) {
            if (!QDir().mkpath(parentDir.absolutePath())) {
                result.errorMessage = "Cannot create directory: " + parentDir.absolutePath();
                return result;
            }
        }
    }
    
    result.isValid = true;
    result.sanitizedValue = sanitized;
    LOG_DEBUG(QString("Validated file path: %1").arg(sanitized));
    return result;
}

InputValidator::ValidationResult InputValidator::validateDirectoryPath(const QString &path, bool mustExist)
{
    ValidationResult result;
    
    if (path.isEmpty()) {
        result.errorMessage = "Directory path cannot be empty";
        return result;
    }
    
    const QString sanitized = sanitizeFilePath(path);
    if (sanitized.isEmpty()) {
        result.errorMessage = "Directory path contains invalid characters";
        return result;
    }
    
    const QDir dir(sanitized);
    
    if (mustExist && !dir.exists()) {
        result.errorMessage = "Directory does not exist: " + sanitized;
        return result;
    }
    
    if (!mustExist && !dir.exists()) {
        if (!QDir().mkpath(sanitized)) {
            result.errorMessage = "Cannot create directory: " + sanitized;
            return result;
        }
    }
    
    result.isValid = true;
    result.sanitizedValue = sanitized;
    LOG_DEBUG(QString("Validated directory path: %1").arg(sanitized));
    return result;
}

InputValidator::ValidationResult InputValidator::validateFileName(const QString &fileName)
{
    ValidationResult result;
    
    if (fileName.isEmpty()) {
        result.errorMessage = "File name cannot be empty";
        return result;
    }
    
    const QString sanitized = sanitizeFileName(fileName);
    if (sanitized.isEmpty()) {
        result.errorMessage = "File name contains only invalid characters";
        return result;
    }
    
    if (sanitized.length() > 255) {
        result.errorMessage = "File name too long (maximum 255 characters)";
        return result;
    }
    
    // Check for reserved names on Windows
    const QStringList reserved = {"CON", "PRN", "AUX", "NUL", "COM1", "COM2", "COM3", "COM4",
                                  "COM5", "COM6", "COM7", "COM8", "COM9", "LPT1", "LPT2", 
                                  "LPT3", "LPT4", "LPT5", "LPT6", "LPT7", "LPT8", "LPT9"};
    
    const QString baseName = QFileInfo(sanitized).baseName().toUpper();
    if (reserved.contains(baseName)) {
        result.errorMessage = "File name is reserved: " + sanitized;
        return result;
    }
    
    result.isValid = true;
    result.sanitizedValue = sanitized;
    return result;
}

InputValidator::ValidationResult InputValidator::validateNumber(const QString &value, double min, double max)
{
    ValidationResult result;
    
    if (value.isEmpty()) {
        result.errorMessage = "Number value cannot be empty";
        return result;
    }
    
    if (!getNumberRegex().match(value).hasMatch()) {
        result.errorMessage = "Invalid number format: " + value;
        return result;
    }
    
    bool ok;
    const double number = value.toDouble(&ok);
    if (!ok) {
        result.errorMessage = "Cannot convert to number: " + value;
        return result;
    }
    
    if (number < min || number > max) {
        result.errorMessage = QString("Number out of range (%1-%2): %3").arg(min).arg(max).arg(number);
        return result;
    }
    
    result.isValid = true;
    result.sanitizedValue = QString::number(number);
    return result;
}

InputValidator::ValidationResult InputValidator::validateInteger(const QString &value, int min, int max)
{
    ValidationResult result;
    
    if (value.isEmpty()) {
        result.errorMessage = "Integer value cannot be empty";
        return result;
    }
    
    bool ok;
    const int number = value.toInt(&ok);
    if (!ok) {
        result.errorMessage = "Cannot convert to integer: " + value;
        return result;
    }
    
    if (number < min || number > max) {
        result.errorMessage = QString("Integer out of range (%1-%2): %3").arg(min).arg(max).arg(number);
        return result;
    }
    
    result.isValid = true;
    result.sanitizedValue = QString::number(number);
    return result;
}

InputValidator::ValidationResult InputValidator::validateResolution(const QString &width, const QString &height)
{
    ValidationResult result;
    
    const auto widthResult = validateInteger(width, 16, 7680); // Min 16px, max 8K width
    if (!widthResult.isValid) {
        result.errorMessage = "Invalid width: " + widthResult.errorMessage;
        return result;
    }
    
    const auto heightResult = validateInteger(height, 16, 4320); // Min 16px, max 8K height
    if (!heightResult.isValid) {
        result.errorMessage = "Invalid height: " + heightResult.errorMessage;
        return result;
    }
    
    result.isValid = true;
    result.sanitizedValue = QString("%1x%2").arg(widthResult.sanitizedValue, heightResult.sanitizedValue);
    return result;
}

InputValidator::ValidationResult InputValidator::validateBitrate(const QString &bitrate)
{
    return validateInteger(bitrate, 8, 100000); // 8 kbps to 100 Mbps
}

InputValidator::ValidationResult InputValidator::validateFrameRate(const QString &fps)
{
    return validateNumber(fps, 1.0, 240.0); // 1 fps to 240 fps
}

InputValidator::ValidationResult InputValidator::validateTimeValue(const QString &time)
{
    ValidationResult result;
    
    if (time.isEmpty()) {
        result.errorMessage = "Time value cannot be empty";
        return result;
    }
    
    const QRegularExpressionMatch match = getTimeRegex().match(time);
    if (!match.hasMatch()) {
        result.errorMessage = "Invalid time format (expected HH:MM:SS.mmm): " + time;
        return result;
    }
    
    result.isValid = true;
    result.sanitizedValue = time;
    return result;
}

QString InputValidator::sanitizeFilePath(const QString &path)
{
    QString sanitized = path.trimmed();
    
    // Remove null bytes and other control characters
    sanitized.remove(QRegularExpression("[\\x00-\\x1f\\x7f]"));
    
    // Replace backslashes with forward slashes for consistency
    sanitized.replace('\\', '/');
    
    // Remove duplicate slashes
    sanitized.replace(QRegularExpression("/+"), "/");
    
    return sanitized;
}

QString InputValidator::sanitizeFileName(const QString &fileName)
{
    QString sanitized = fileName.trimmed();
    
    // Remove invalid filename characters
    const QRegularExpression invalid(R"([<>:"/\\|?*\x00-\x1f])");
    sanitized.remove(invalid);
    
    // Remove leading and trailing dots and spaces
    sanitized = sanitized.remove(QRegularExpression("^[\\.\\s]+|[\\.\\s]+$"));
    
    return sanitized;
}

QString InputValidator::sanitizeFFmpegParameter(const QString &param)
{
    QString sanitized = param;
    
    // Escape special characters for FFmpeg
    sanitized.replace("\\", "\\\\\\\\");  // Escape backslashes
    sanitized.replace(":", "\\\\:");      // Escape colons
    sanitized.replace(",", "\\,");        // Escape commas
    sanitized.replace(";", "\\;");        // Escape semicolons
    sanitized.replace("'", "\\\\\\\\'");  // Escape single quotes
    sanitized.replace("[", "\\[");        // Escape square brackets
    sanitized.replace("]", "\\]");
    
    return sanitized;
}

QStringList InputValidator::sanitizeFFmpegParameters(const QStringList &params)
{
    QStringList sanitized;
    for (const QString &param : params) {
        sanitized.append(sanitizeFFmpegParameter(param));
    }
    return sanitized;
}

bool InputValidator::isValidVideoExtension(const QString &extension)
{
    return getValidVideoExtensions().contains(extension.toLower());
}

bool InputValidator::isValidAudioExtension(const QString &extension)
{
    return getValidAudioExtensions().contains(extension.toLower());
}

bool InputValidator::isValidSubtitleExtension(const QString &extension)
{
    return getValidSubtitleExtensions().contains(extension.toLower());
}

bool InputValidator::isCodecCompatibleWithContainer(const QString &codec, const QString &container)
{
    // Simplified compatibility check - can be expanded based on requirements
    const QString lowerCodec = codec.toLower();
    const QString lowerContainer = container.toLower();
    
    // MP4 container
    if (lowerContainer == "mp4" || lowerContainer == "m4v") {
        return lowerCodec == "h264" || lowerCodec == "h265" || lowerCodec == "hevc" || 
               lowerCodec == "mpeg4" || lowerCodec == "aac" || lowerCodec == "mp3";
    }
    
    // MKV container (supports almost everything)
    if (lowerContainer == "mkv") {
        return true;
    }
    
    // AVI container
    if (lowerContainer == "avi") {
        return lowerCodec != "hevc"; // AVI doesn't support HEVC well
    }
    
    // Default: assume compatible
    return true;
}