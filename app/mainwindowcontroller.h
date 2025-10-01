/***********************************************************************

                          C I N E   E N C O D E R
                                JULY, 2020
                            COPYRIGHT (C) 2020

 FILE: mainwindowcontroller.h
 COMMENT: Controller to separate MainWindow UI logic from business logic
 LICENSE: GNU General Public License v3.0

***********************************************************************/

#ifndef MAINWINDOWCONTROLLER_H
#define MAINWINDOWCONTROLLER_H

#include <QObject>
#include <QStringList>
#include <memory>
#include "interfaces.h"

class MainWindow;

/**
 * @brief Controller to mediate between MainWindow and business services
 * 
 * This controller handles the coordination between the UI and business logic,
 * making the code more testable and maintainable. It follows the MVP (Model-View-Presenter)
 * pattern where the controller acts as the presenter.
 */
class MainWindowController : public QObject
{
    Q_OBJECT

public:
    explicit MainWindowController(MainWindow *view, QObject *parent = nullptr);
    ~MainWindowController() override;

    // File management operations
    void openFiles(const QStringList &fileNames);
    void removeFile(int index);
    void removeAllFiles();
    void addFilesToQueue(const QStringList &fileNames);

    // Encoding operations
    void startEncoding();
    void pauseEncoding();
    void resumeEncoding();
    void stopEncoding();
    void abortEncoding();

    // Settings operations
    void loadSettings();
    void saveSettings();
    void resetSettings();
    void showSettingsDialog();

    // UI state management
    void updateProgress(int progress);
    void setEnabled(bool enabled);
    void showMessage(const QString &message);
    void showError(const QString &error);

    // Validation operations
    bool validateCurrentSelection();
    bool validateOutputPath();
    bool validateEncodingParameters();

    // Batch operations
    void setupBatchEncoding();
    void processBatchQueue();

public slots:
    // Slots to handle UI events
    void onFileSelectionChanged();
    void onParametersChanged();
    void onOutputPathChanged();

private slots:
    // Slots to handle service events
    void onEncodingStatusChanged(IEncodingManager::Status status);
    void onEncodingProgressChanged(int progress, float remainingTime);
    void onEncodingCompleted(const QString &outputFile);
    void onEncodingError(const QString &errorMessage);

private:
    // Helper methods
    void setupConnections();
    void updateUIState();
    void validateAndUpdateUI();
    QStringList getEncodingParameters();
    QString getOutputPath();
    QStringList getSelectedInputFiles();
    
    // Dependency injection
    void initializeServices();
    
    MainWindow *m_view = nullptr;
    
    // Service dependencies
    std::shared_ptr<ILogger> m_logger;
    std::shared_ptr<IConfigurationManager> m_config;
    std::shared_ptr<IInputValidator> m_validator;
    std::shared_ptr<IEncodingManager> m_encodingManager;
    std::shared_ptr<IErrorHandler> m_errorHandler;
    
    // Controller state
    bool m_encodingInProgress = false;
    int m_currentProgress = 0;
    QStringList m_fileQueue;
    int m_currentFileIndex = 0;
};

#endif // MAINWINDOWCONTROLLER_H