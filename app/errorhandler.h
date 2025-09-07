/***********************************************************************

                          C I N E   E N C O D E R
                                JULY, 2020
                            COPYRIGHT (C) 2020

 FILE: errorhandler.h
 COMMENT: Centralized error handling and user feedback system
 LICENSE: GNU General Public License v3.0

***********************************************************************/

#ifndef ERRORHANDLER_H
#define ERRORHANDLER_H

#include <QObject>
#include <QString>
#include <QWidget>
#include <functional>

class PopupMessage;

class ErrorHandler : public QObject
{
    Q_OBJECT

public:
    enum class Severity {
        Info,
        Warning,
        Error,
        Critical
    };

    enum class Action {
        None,
        Retry,
        Cancel,
        Ignore,
        SaveData
    };

    struct ErrorInfo {
        Severity severity = Severity::Error;
        QString title;
        QString message;
        QString details;
        QString context; // Where the error occurred
        Action suggestedAction = Action::None;
        std::function<void()> onRetry;
        std::function<void()> onCancel;
    };

    static ErrorHandler& instance();

    // Main error reporting methods
    void handleError(const ErrorInfo &errorInfo, QWidget *parent = nullptr);
    void handleException(const std::exception &e, const QString &context = "", QWidget *parent = nullptr);
    
    // Convenience methods
    void showInfo(const QString &message, const QString &title = "", QWidget *parent = nullptr);
    void showWarning(const QString &message, const QString &title = "", QWidget *parent = nullptr);  
    void showError(const QString &message, const QString &title = "", QWidget *parent = nullptr);
    void showCritical(const QString &message, const QString &title = "", QWidget *parent = nullptr);
    
    // File operation error helpers
    void handleFileError(const QString &filePath, const QString &operation, const QString &errorDetails, QWidget *parent = nullptr);
    void handleNetworkError(const QString &url, const QString &errorDetails, QWidget *parent = nullptr);
    void handleEncodingError(const QString &inputFile, const QString &errorDetails, QWidget *parent = nullptr);
    
    // User confirmation dialogs
    bool askUser(const QString &question, const QString &title = "", QWidget *parent = nullptr);
    Action askUserAction(const QString &question, const QStringList &actions, const QString &title = "", QWidget *parent = nullptr);
    
    // Configuration
    void setParentWidget(QWidget *parent) { m_defaultParent = parent; }
    void enablePopups(bool enabled) { m_popupsEnabled = enabled; }
    void enableLogging(bool enabled) { m_loggingEnabled = enabled; }

signals:
    void errorOccurred(const ErrorInfo &errorInfo);
    void userActionRequired(const QString &message, const QStringList &actions);

private slots:
    void onPopupActionClicked();

private:
    explicit ErrorHandler(QObject *parent = nullptr);
    ~ErrorHandler() override;
    
    QString severityToString(Severity severity) const;
    QString getDefaultTitle(Severity severity) const;
    void logError(const ErrorInfo &errorInfo);
    void showPopupMessage(const ErrorInfo &errorInfo, QWidget *parent);
    
    QWidget *m_defaultParent = nullptr;
    bool m_popupsEnabled = true;
    bool m_loggingEnabled = true;
};

// Convenience macros for error handling
#define HANDLE_ERROR(message) ErrorHandler::instance().showError(message, "", nullptr)
#define HANDLE_WARNING(message) ErrorHandler::instance().showWarning(message, "", nullptr)
#define HANDLE_INFO(message) ErrorHandler::instance().showInfo(message, "", nullptr)
#define HANDLE_CRITICAL(message) ErrorHandler::instance().showCritical(message, "", nullptr)

#define HANDLE_FILE_ERROR(path, operation, details) ErrorHandler::instance().handleFileError(path, operation, details, nullptr)
#define HANDLE_ENCODING_ERROR(input, details) ErrorHandler::instance().handleEncodingError(input, details, nullptr)

#define ASK_USER(question) ErrorHandler::instance().askUser(question, "", nullptr)

#endif // ERRORHANDLER_H