/***********************************************************************

                          C I N E   E N C O D E R
                                JULY, 2020
                            COPYRIGHT (C) 2020

 FILE: errorhandler.cpp
 COMMENT: Centralized error handling implementation
 LICENSE: GNU General Public License v3.0

***********************************************************************/

#include "errorhandler.h"
#include "logger.h"
#include "popupmessage.h"
#include <QMessageBox>
#include <QApplication>
#include <QFileInfo>

ErrorHandler::ErrorHandler(QObject *parent) : QObject(parent)
{
}

ErrorHandler::~ErrorHandler() = default;

ErrorHandler& ErrorHandler::instance()
{
    static ErrorHandler handler;
    return handler;
}

void ErrorHandler::handleError(const ErrorInfo &errorInfo, QWidget *parent)
{
    // Log the error
    if (m_loggingEnabled) {
        logError(errorInfo);
    }
    
    // Emit signal for any listeners
    emit errorOccurred(errorInfo);
    
    // Show user interface if popups are enabled
    if (m_popupsEnabled) {
        showPopupMessage(errorInfo, parent);
    }
}

void ErrorHandler::handleException(const std::exception &e, const QString &context, QWidget *parent)
{
    ErrorInfo errorInfo;
    errorInfo.severity = Severity::Critical;
    errorInfo.title = "Unexpected Error";
    errorInfo.message = QString("An unexpected error occurred: %1").arg(e.what());
    errorInfo.context = context;
    errorInfo.suggestedAction = Action::Cancel;
    
    handleError(errorInfo, parent);
}

void ErrorHandler::showInfo(const QString &message, const QString &title, QWidget *parent)
{
    ErrorInfo errorInfo;
    errorInfo.severity = Severity::Info;
    errorInfo.title = title.isEmpty() ? getDefaultTitle(Severity::Info) : title;
    errorInfo.message = message;
    
    handleError(errorInfo, parent);
}

void ErrorHandler::showWarning(const QString &message, const QString &title, QWidget *parent)
{
    ErrorInfo errorInfo;
    errorInfo.severity = Severity::Warning;
    errorInfo.title = title.isEmpty() ? getDefaultTitle(Severity::Warning) : title;
    errorInfo.message = message;
    
    handleError(errorInfo, parent);
}

void ErrorHandler::showError(const QString &message, const QString &title, QWidget *parent)
{
    ErrorInfo errorInfo;
    errorInfo.severity = Severity::Error;
    errorInfo.title = title.isEmpty() ? getDefaultTitle(Severity::Error) : title;
    errorInfo.message = message;
    
    handleError(errorInfo, parent);
}

void ErrorHandler::showCritical(const QString &message, const QString &title, QWidget *parent)
{
    ErrorInfo errorInfo;
    errorInfo.severity = Severity::Critical;
    errorInfo.title = title.isEmpty() ? getDefaultTitle(Severity::Critical) : title;
    errorInfo.message = message;
    
    handleError(errorInfo, parent);
}

void ErrorHandler::handleFileError(const QString &filePath, const QString &operation, const QString &errorDetails, QWidget *parent)
{
    ErrorInfo errorInfo;
    errorInfo.severity = Severity::Error;
    errorInfo.title = "File Operation Error";
    errorInfo.message = QString("Failed to %1 file: %2").arg(operation, QFileInfo(filePath).fileName());
    errorInfo.details = QString("File: %1\nOperation: %2\nError: %3").arg(filePath, operation, errorDetails);
    errorInfo.context = "File Operations";
    
    handleError(errorInfo, parent);
}

void ErrorHandler::handleNetworkError(const QString &url, const QString &errorDetails, QWidget *parent)
{
    ErrorInfo errorInfo;
    errorInfo.severity = Severity::Warning;
    errorInfo.title = "Network Error";
    errorInfo.message = "Network operation failed";
    errorInfo.details = QString("URL: %1\nError: %2").arg(url, errorDetails);
    errorInfo.context = "Network Operations";
    
    handleError(errorInfo, parent);
}

void ErrorHandler::handleEncodingError(const QString &inputFile, const QString &errorDetails, QWidget *parent)
{
    ErrorInfo errorInfo;
    errorInfo.severity = Severity::Error;
    errorInfo.title = "Encoding Error";
    errorInfo.message = QString("Failed to encode: %1").arg(QFileInfo(inputFile).fileName());
    errorInfo.details = QString("Input file: %1\nError details: %2").arg(inputFile, errorDetails);
    errorInfo.context = "Video Encoding";
    errorInfo.suggestedAction = Action::Retry;
    
    handleError(errorInfo, parent);
}

bool ErrorHandler::askUser(const QString &question, const QString &title, QWidget *parent)
{
    QWidget *parentWidget = parent ? parent : m_defaultParent;
    const QString dialogTitle = title.isEmpty() ? "Confirmation" : title;
    
    const int result = QMessageBox::question(parentWidget, dialogTitle, question, 
                                            QMessageBox::Yes | QMessageBox::No, 
                                            QMessageBox::No);
    
    LOG_INFO(QString("User question: %1, Answer: %2").arg(question, (result == QMessageBox::Yes ? "Yes" : "No")));
    
    return result == QMessageBox::Yes;
}

ErrorHandler::Action ErrorHandler::askUserAction(const QString &question, const QStringList &actions, const QString &title, QWidget *parent)
{
    QWidget *parentWidget = parent ? parent : m_defaultParent;
    const QString dialogTitle = title.isEmpty() ? "Action Required" : title;
    
    QMessageBox msgBox(parentWidget);
    msgBox.setWindowTitle(dialogTitle);
    msgBox.setText(question);
    msgBox.setIcon(QMessageBox::Question);
    
    QList<QPushButton*> buttons;
    for (int i = 0; i < actions.size(); ++i) {
        auto *button = msgBox.addButton(actions[i], QMessageBox::ActionRole);
        buttons.append(button);
        if (i == 0) {
            msgBox.setDefaultButton(button);
        }
    }
    
    msgBox.exec();
    
    for (int i = 0; i < buttons.size(); ++i) {
        if (msgBox.clickedButton() == buttons[i]) {
            LOG_INFO(QString("User action: %1, Selected: %2").arg(question, actions[i]));
            return static_cast<Action>(i + 1); // Offset by 1 since None is 0
        }
    }
    
    return Action::None;
}

QString ErrorHandler::severityToString(Severity severity) const
{
    switch (severity) {
        case Severity::Info:     return "INFO";
        case Severity::Warning:  return "WARNING";
        case Severity::Error:    return "ERROR";
        case Severity::Critical: return "CRITICAL";
        default:                 return "UNKNOWN";
    }
}

QString ErrorHandler::getDefaultTitle(Severity severity) const
{
    switch (severity) {
        case Severity::Info:     return "Information";
        case Severity::Warning:  return "Warning";
        case Severity::Error:    return "Error";
        case Severity::Critical: return "Critical Error";
        default:                 return "Message";
    }
}

void ErrorHandler::logError(const ErrorInfo &errorInfo)
{
    const QString logMessage = QString("Error [%1] in %2: %3")
                                .arg(severityToString(errorInfo.severity))
                                .arg(errorInfo.context.isEmpty() ? "Unknown" : errorInfo.context)
                                .arg(errorInfo.message);
    
    switch (errorInfo.severity) {
        case Severity::Info:
            LOG_INFO(logMessage);
            break;
        case Severity::Warning:
            LOG_WARNING(logMessage);
            break;
        case Severity::Error:
            LOG_ERROR(logMessage);
            if (!errorInfo.details.isEmpty()) {
                LOG_ERROR("Details: " + errorInfo.details);
            }
            break;
        case Severity::Critical:
            LOG_CRITICAL(logMessage);
            if (!errorInfo.details.isEmpty()) {
                LOG_CRITICAL("Details: " + errorInfo.details);
            }
            break;
    }
}

void ErrorHandler::showPopupMessage(const ErrorInfo &errorInfo, QWidget *parent)
{
    QWidget *parentWidget = parent ? parent : m_defaultParent;
    
    // For critical and error messages, use QMessageBox for immediate attention
    if (errorInfo.severity >= Severity::Error) {
        QMessageBox::Icon icon;
        switch (errorInfo.severity) {
            case Severity::Error:    icon = QMessageBox::Critical; break;
            case Severity::Critical: icon = QMessageBox::Critical; break;
            default:                 icon = QMessageBox::Warning; break;
        }
        
        QMessageBox msgBox(icon, errorInfo.title, errorInfo.message, QMessageBox::Ok, parentWidget);
        if (!errorInfo.details.isEmpty()) {
            msgBox.setDetailedText(errorInfo.details);
        }
        msgBox.exec();
    }
    // For info and warning messages, try to use popup message if available
    else {
        // Try to find a main window for popup messages
        QWidget *mainWindow = parentWidget;
        while (mainWindow && mainWindow->parent()) {
            mainWindow = qobject_cast<QWidget*>(mainWindow->parent());
        }
        
        if (mainWindow) {
            // Use floating popup message - this would need to be implemented
            // For now, fall back to QMessageBox
            QMessageBox::Icon icon = (errorInfo.severity == Severity::Warning) ? 
                                   QMessageBox::Warning : QMessageBox::Information;
            
            QMessageBox msgBox(icon, errorInfo.title, errorInfo.message, QMessageBox::Ok, parentWidget);
            msgBox.exec();
        }
    }
}

void ErrorHandler::onPopupActionClicked()
{
    // Handle popup action clicks if needed
}