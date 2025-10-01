/***********************************************************************

                          C I N E   E N C O D E R
                                JULY, 2020
                            COPYRIGHT (C) 2020

 FILE: mainwindowcontroller.cpp
 COMMENT: Controller implementation for MainWindow business logic
 LICENSE: GNU General Public License v3.0

***********************************************************************/

#include "mainwindowcontroller.h"
#include "mainwindow.h"
#include "servicecontainer.h"
#include <QFileInfo>

MainWindowController::MainWindowController(MainWindow *view, QObject *parent)
    : QObject(parent), m_view(view)
{
    initializeServices();
    setupConnections();
}

MainWindowController::~MainWindowController() = default;

void MainWindowController::initializeServices()
{
    // Resolve services from the container
    m_logger = RESOLVE_SERVICE(ILogger);
    m_config = RESOLVE_SERVICE(IConfigurationManager);
    m_validator = RESOLVE_SERVICE(IInputValidator);
    m_encodingManager = RESOLVE_SERVICE(IEncodingManager);
    m_errorHandler = RESOLVE_SERVICE(IErrorHandler);
    
    // Log service initialization
    if (m_logger) {
        m_logger->info("MainWindowController initialized with dependency injection");
    }
}

void MainWindowController::setupConnections()
{
    if (m_encodingManager) {
        // Connect encoding manager signals to controller slots
        connect(m_encodingManager.get(), &IEncodingManager::statusChanged,
                this, &MainWindowController::onEncodingStatusChanged);
        connect(m_encodingManager.get(), &IEncodingManager::progressChanged,
                this, &MainWindowController::onEncodingProgressChanged);
        connect(m_encodingManager.get(), &IEncodingManager::encodingCompleted,
                this, &MainWindowController::onEncodingCompleted);
        connect(m_encodingManager.get(), &IEncodingManager::encodingError,
                this, &MainWindowController::onEncodingError);
    }
}

void MainWindowController::openFiles(const QStringList &fileNames)
{
    if (m_logger) {
        m_logger->info(QString("Opening %1 files").arg(fileNames.size()));
    }
    
    QStringList validFiles;
    QStringList errors;
    
    // Validate each file
    for (const QString &fileName : fileNames) {
        if (m_validator) {
            auto result = m_validator->validateFilePath(fileName, true);
            if (result.isValid) {
                validFiles.append(result.sanitizedValue);
            } else {
                errors.append(QString("Invalid file '%1': %2").arg(fileName, result.errorMessage));
            }
        } else {
            // Fallback validation without service
            if (QFileInfo::exists(fileName)) {
                validFiles.append(fileName);
            } else {
                errors.append(QString("File does not exist: %1").arg(fileName));
            }
        }
    }
    
    // Report errors
    if (!errors.isEmpty()) {
        QString errorMessage = "Some files could not be opened:\\n" + errors.join("\\n");
        if (m_errorHandler) {
            m_errorHandler->handleError(errorMessage, m_view);
        }
        if (m_logger) {
            m_logger->error(errorMessage);
        }
    }
    
    // Add valid files to the view
    // Note: This would typically call a method on the view to update the file list
    // For now, this is a placeholder showing the separation of concerns
    m_fileQueue = validFiles;
    
    updateUIState();
}

void MainWindowController::removeFile(int index)
{
    if (index >= 0 && index < m_fileQueue.size()) {
        QString removedFile = m_fileQueue.takeAt(index);
        if (m_logger) {
            m_logger->info(QString("Removed file: %1").arg(removedFile));
        }
        updateUIState();
    }
}

void MainWindowController::removeAllFiles()
{
    int count = m_fileQueue.size();
    m_fileQueue.clear();
    
    if (m_logger) {
        m_logger->info(QString("Removed all %1 files from queue").arg(count));
    }
    
    updateUIState();
}

void MainWindowController::startEncoding()
{
    if (m_encodingInProgress) {
        if (m_logger) {
            m_logger->warning("Encoding already in progress");
        }
        return;
    }
    
    // Validate before starting
    if (!validateCurrentSelection()) {
        return;
    }
    
    if (!validateOutputPath()) {
        return;
    }
    
    if (!validateEncodingParameters()) {
        return;
    }
    
    // Get encoding details
    QStringList inputFiles = getSelectedInputFiles();
    QString outputPath = getOutputPath();
    QStringList parameters = getEncodingParameters();
    
    if (inputFiles.isEmpty()) {
        if (m_errorHandler) {
            m_errorHandler->handleError("No input files selected", m_view);
        }
        return;
    }
    
    // Start encoding
    if (m_encodingManager) {
        if (m_logger) {
            m_logger->info(QString("Starting encoding of %1 files").arg(inputFiles.size()));
        }
        
        m_encodingInProgress = true;
        m_currentFileIndex = 0;
        
        // For multiple files, we'll process them sequentially
        // Start with the first file
        if (!inputFiles.isEmpty()) {
            QString inputFile = inputFiles.first();
            QString outputFile = outputPath + "/" + QFileInfo(inputFile).baseName() + "_encoded." + 
                                QFileInfo(inputFile).suffix();
            
            m_encodingManager->startEncoding(inputFile, outputFile, parameters);
        }
        
        updateUIState();
    }
}

void MainWindowController::pauseEncoding()
{
    if (m_encodingManager && m_encodingInProgress) {
        m_encodingManager->pauseEncoding();
        if (m_logger) {
            m_logger->info("Encoding paused");
        }
    }
}

void MainWindowController::resumeEncoding()
{
    if (m_encodingManager && m_encodingInProgress) {
        m_encodingManager->resumeEncoding();
        if (m_logger) {
            m_logger->info("Encoding resumed");
        }
    }
}

void MainWindowController::stopEncoding()
{
    if (m_encodingManager && m_encodingInProgress) {
        m_encodingManager->stopEncoding();
        m_encodingInProgress = false;
        if (m_logger) {
            m_logger->info("Encoding stopped");
        }
        updateUIState();
    }
}

void MainWindowController::abortEncoding()
{
    stopEncoding(); // Same implementation for now
}

bool MainWindowController::validateCurrentSelection()
{
    QStringList selectedFiles = getSelectedInputFiles();
    
    if (selectedFiles.isEmpty()) {
        if (m_errorHandler) {
            m_errorHandler->handleError("Please select at least one input file", m_view);
        }
        return false;
    }
    
    // Validate each selected file
    for (const QString &file : selectedFiles) {
        if (m_validator) {
            auto result = m_validator->validateFilePath(file, true);
            if (!result.isValid) {
                if (m_errorHandler) {
                    m_errorHandler->handleError(
                        QString("Invalid file selection: %1\\n%2").arg(file, result.errorMessage),
                        m_view);
                }
                return false;
            }
        }
    }
    
    return true;
}

bool MainWindowController::validateOutputPath()
{
    QString outputPath = getOutputPath();
    
    if (outputPath.isEmpty()) {
        if (m_errorHandler) {
            m_errorHandler->handleError("Please specify an output directory", m_view);
        }
        return false;
    }
    
    if (m_validator) {
        auto result = m_validator->validateFilePath(outputPath, false);
        if (!result.isValid) {
            if (m_errorHandler) {
                m_errorHandler->handleError(
                    QString("Invalid output path: %1").arg(result.errorMessage),
                    m_view);
            }
            return false;
        }
    }
    
    return true;
}

bool MainWindowController::validateEncodingParameters()
{
    QStringList parameters = getEncodingParameters();
    
    if (m_validator) {
        QStringList sanitizedParams = m_validator->sanitizeFFmpegParameters(parameters);
        if (sanitizedParams != parameters) {
            if (m_logger) {
                m_logger->warning("Some encoding parameters were sanitized");
            }
        }
    }
    
    return true; // For now, assume parameters are always valid after sanitization
}

void MainWindowController::updateUIState()
{
    // Update UI based on current state
    // This is where we would call methods on the view to update its appearance
    
    bool hasFiles = !m_fileQueue.isEmpty();
    bool canEncode = hasFiles && !m_encodingInProgress;
    bool canStop = m_encodingInProgress;
    
    // In a real implementation, these would be calls to the view
    // For example: m_view->setEncodeButtonEnabled(canEncode);
    //              m_view->setStopButtonEnabled(canStop);
    //              m_view->setFileListEnabled(!m_encodingInProgress);
}

void MainWindowController::onEncodingStatusChanged(IEncodingManager::Status status)
{
    if (m_logger) {
        m_logger->info(QString("Encoding status changed to: %1").arg(static_cast<int>(status)));
    }
    
    switch (status) {
        case IEncodingManager::Status::Completed:
        case IEncodingManager::Status::Aborted:
        case IEncodingManager::Status::Error:
            m_encodingInProgress = false;
            break;
        default:
            break;
    }
    
    updateUIState();
}

void MainWindowController::onEncodingProgressChanged(int progress, float remainingTime)
{
    m_currentProgress = progress;
    
    // Update progress display
    // In a real implementation: m_view->setProgress(progress);
    //                          m_view->setRemainingTime(remainingTime);
}

void MainWindowController::onEncodingCompleted(const QString &outputFile)
{
    if (m_logger) {
        m_logger->info(QString("Encoding completed: %1").arg(outputFile));
    }
    
    // Process next file in queue if there are more
    m_currentFileIndex++;
    QStringList inputFiles = getSelectedInputFiles();
    
    if (m_currentFileIndex < inputFiles.size()) {
        // Start next file
        QString inputFile = inputFiles[m_currentFileIndex];
        QString outputPath = getOutputPath();
        QString outputFile = outputPath + "/" + QFileInfo(inputFile).baseName() + "_encoded." + 
                            QFileInfo(inputFile).suffix();
        QStringList parameters = getEncodingParameters();
        
        if (m_encodingManager) {
            m_encodingManager->startEncoding(inputFile, outputFile, parameters);
        }
    } else {
        // All files completed
        m_encodingInProgress = false;
        if (m_logger) {
            m_logger->info("All files encoded successfully");
        }
        
        if (m_errorHandler) {
            m_errorHandler->handleInfo("Encoding completed successfully", m_view);
        }
        
        updateUIState();
    }
}

void MainWindowController::onEncodingError(const QString &errorMessage)
{
    if (m_logger) {
        m_logger->error(QString("Encoding error: %1").arg(errorMessage));
    }
    
    if (m_errorHandler) {
        m_errorHandler->handleError(QString("Encoding failed: %1").arg(errorMessage), m_view);
    }
    
    m_encodingInProgress = false;
    updateUIState();
}

// Placeholder implementations for UI access methods
// These would be implemented to actually interact with the MainWindow
QStringList MainWindowController::getEncodingParameters()
{
    // This would get parameters from the UI
    return QStringList{"-vcodec", "libx264", "-acodec", "aac"};
}

QString MainWindowController::getOutputPath()
{
    // This would get the output path from the UI
    return "/tmp/output";
}

QStringList MainWindowController::getSelectedInputFiles()
{
    // This would get selected files from the UI
    return m_fileQueue;
}

void MainWindowController::loadSettings()
{
    if (m_config && m_logger) {
        m_logger->info("Loading application settings");
        // Load settings and apply to UI
    }
}

void MainWindowController::onFileSelectionChanged()
{
    // Handle file selection changes
    validateAndUpdateUI();
}

void MainWindowController::onParametersChanged()
{
    // Handle encoding parameter changes
    validateAndUpdateUI();
}

void MainWindowController::onOutputPathChanged()
{
    // Handle output path changes
    validateAndUpdateUI();
}

void MainWindowController::validateAndUpdateUI()
{
    // Validate current state and update UI accordingly
    bool isValid = validateCurrentSelection() && validateOutputPath() && validateEncodingParameters();
    
    // Update UI state based on validation results
    // This would update button states, error messages, etc.
    updateUIState();
}