/***********************************************************************

                          C I N E   E N C O D E R
                                JULY, 2020
                            COPYRIGHT (C) 2020

 FILE: interfaces.h
 COMMENT: Interface definitions for dependency injection and testability
 LICENSE: GNU General Public License v3.0

***********************************************************************/

#ifndef INTERFACES_H
#define INTERFACES_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariant>

// Forward declarations for Qt classes
class QWidget;

/**
 * @brief Interface for logging operations
 * 
 * This interface allows for easy mocking and testing of logging functionality.
 */
class ILogger
{
public:
    virtual ~ILogger() = default;
    
    virtual void debug(const QString &message) = 0;
    virtual void info(const QString &message) = 0;
    virtual void warning(const QString &message) = 0;
    virtual void error(const QString &message) = 0;
    virtual void critical(const QString &message) = 0;
};

/**
 * @brief Interface for configuration management
 * 
 * Provides type-safe configuration access with validation.
 */
class IConfigurationManager
{
public:
    virtual ~IConfigurationManager() = default;
    
    virtual bool getBool(const QString &key, bool defaultValue = false) const = 0;
    virtual int getInt(const QString &key, int defaultValue = 0) const = 0;
    virtual QString getString(const QString &key, const QString &defaultValue = QString()) const = 0;
    
    virtual void setBool(const QString &key, bool value) = 0;
    virtual void setInt(const QString &key, int value) = 0;
    virtual void setString(const QString &key, const QString &value) = 0;
    
    virtual bool isValidKey(const QString &key) const = 0;
    virtual bool validateValue(const QString &key, const QVariant &value) const = 0;
};

/**
 * @brief Interface for input validation
 * 
 * Provides validation and sanitization services for user input.
 */
class IInputValidator
{
public:
    struct ValidationResult {
        bool isValid = false;
        QString errorMessage;
        QString sanitizedValue;
        
        explicit operator bool() const { return isValid; }
    };
    
    virtual ~IInputValidator() = default;
    
    virtual ValidationResult validateFilePath(const QString &path, bool mustExist = false) = 0;
    virtual ValidationResult validateNumber(const QString &value, double min = 0, double max = 999999) = 0;
    virtual ValidationResult validateResolution(const QString &width, const QString &height) = 0;
    
    virtual QString sanitizeFilePath(const QString &path) = 0;
    virtual QString sanitizeFileName(const QString &fileName) = 0;
    virtual QStringList sanitizeFFmpegParameters(const QStringList &params) = 0;
};

/**
 * @brief Interface for encoding operations
 * 
 * Manages video/audio encoding processes.
 */
class IEncodingManager : public QObject
{
    Q_OBJECT

public:
    enum class Status {
        Idle, Initializing, Running, Paused, Completed, Aborted, Error
    };
    
    virtual ~IEncodingManager() = default;
    
    virtual void startEncoding(const QString &inputFile, const QString &outputFile, 
                              const QStringList &parameters) = 0;
    virtual void pauseEncoding() = 0;
    virtual void resumeEncoding() = 0;
    virtual void stopEncoding() = 0;
    
    virtual Status getStatus() const = 0;
    virtual int getProgress() const = 0;
    virtual QString getCurrentFile() const = 0;

signals:
    void statusChanged(Status status);
    void progressChanged(int progress, float remainingTime);
    void encodingCompleted(const QString &outputFile);
    void encodingError(const QString &errorMessage);
};

/**
 * @brief Interface for error handling
 * 
 * Provides consistent error handling across the application.
 */
class IErrorHandler
{
public:
    virtual ~IErrorHandler() = default;
    
    virtual void handleError(const QString &message, QWidget *parent = nullptr) = 0;
    virtual void handleWarning(const QString &message, QWidget *parent = nullptr) = 0;
    virtual void handleInfo(const QString &message, QWidget *parent = nullptr) = 0;
    virtual bool askConfirmation(const QString &message, QWidget *parent = nullptr) = 0;
};

/**
 * @brief Interface for UI state management
 * 
 * Manages UI state independently of the specific UI implementation.
 */
class IUIStateManager
{
public:
    virtual ~IUIStateManager() = default;
    
    virtual void setEnabled(bool enabled) = 0;
    virtual void setProgress(int progress) = 0;
    virtual void setStatus(const QString &status) = 0;
    virtual void showMessage(const QString &message) = 0;
    
    virtual QString getOutputPath() const = 0;
    virtual QStringList getSelectedFiles() const = 0;
    virtual QStringList getEncodingParameters() const = 0;
};

#endif // INTERFACES_H