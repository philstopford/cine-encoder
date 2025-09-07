/***********************************************************************

                          C I N E   E N C O D E R
                                JULY, 2020
                            COPYRIGHT (C) 2020

 FILE: refactoring_example.cpp
 COMMENT: Before/After example of MainWindow method refactoring
 LICENSE: GNU General Public License v3.0

***********************************************************************/

/*
 * This demonstrates how a typical MainWindow method would be refactored
 * using the new architectural improvements.
 */

#include "logger.h"
#include "errorhandler.h"
#include "inputvalidator.h"
#include "configurationmanager.h"
#include "encodingmanager.h"

// ============================================================================
// BEFORE: Original MainWindow method - typical of the existing codebase
// ============================================================================

class MainWindowOld
{
public:
    void onStartEncoding_OLD_WAY()
    {
        // This is typical of current MainWindow methods:
        // - Long, doing multiple things
        // - Direct UI access mixed with business logic
        // - No error handling
        // - No validation
        // - Hard to test
        
        // Get current row (no validation)
        int row = ui->tableWidget->currentRow();
        
        // Direct UI access for data retrieval
        QString inputFile = ui->tableWidget->item(row, 0)->text();
        QString outputFile = ui->lineEdit_output->text();
        
        // Manual parameter construction (error-prone)
        QStringList params;
        if (ui->checkBox_video->isChecked()) {
            params << "-c:v" << ui->comboBox_codec->currentText();
            params << "-preset" << ui->comboBox_preset->currentText();
            if (ui->checkBox_bitrate->isChecked()) {
                params << "-b:v" << ui->spinBox_bitrate->text() + "k";
            }
        }
        
        // No validation - could crash or produce bad results
        
        // Complex encoding logic mixed in UI class
        encoder = new Encoder(this);
        connect(encoder, &Encoder::progress, this, &MainWindow::onEncodingProgress);
        connect(encoder, &Encoder::completed, this, &MainWindow::onEncodingCompleted);
        connect(encoder, &Encoder::error, this, &MainWindow::onEncodingError);
        
        // Start encoding with no error checking
        encoder->initEncoding(
            tempFile, inputFile, outputFile, 
            width, height, fps, startTime, endTime, duration,
            container, title, params, 
            data[row], threads, priority
        );
        
        // Update UI state directly
        ui->pushButton_start->setEnabled(false);
        ui->pushButton_stop->setEnabled(true);
        ui->progressBar->setValue(0);
        ui->label_status->setText("Encoding...");
        
        // Manual timer management
        timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &MainWindow::updateProgress);
        timer->start(1000);
        
        // Log with inconsistent method
        qDebug() << "Started encoding: " << inputFile;
    }
    
private:
    // ... 200+ member variables
    // ... UI pointer, encoder pointer, timer pointer, etc.
    QWidget *ui = nullptr;
    Encoder *encoder = nullptr;
    QTimer *timer = nullptr;
    // ... many more
};

// ============================================================================
// AFTER: Refactored with new architecture
// ============================================================================

class MainWindowNew
{
public:
    void onStartEncoding_NEW_WAY()
    {
        // New approach:
        // - Short, focused method
        // - Delegates to service layer
        // - Comprehensive error handling
        // - Input validation
        // - Easy to test and maintain
        
        LOG_INFO("User requested encoding start");
        
        // Validate current selection
        const auto selectedFiles = getSelectedFiles();
        if (selectedFiles.isEmpty()) {
            HANDLE_WARNING("Please select files to encode");
            return;
        }
        
        // Delegate to service layer for business logic
        if (!m_encodingService->canStartEncoding()) {
            HANDLE_ERROR("Cannot start encoding: Another operation is in progress");
            return;
        }
        
        // Use service to start encoding with proper error handling
        const EncodingJob job = createEncodingJobFromUI();
        if (!job.isValid()) {
            HANDLE_ERROR("Invalid encoding configuration: " + job.getValidationError());
            return;
        }
        
        m_encodingService->startEncoding(job);
        updateUIForEncodingStart();
    }
    
private:
    // Clean separation of concerns
    std::unique_ptr<EncodingManager> m_encodingService;
    std::unique_ptr<UIStateManager> m_uiStateManager;
    std::unique_ptr<FileSelectionManager> m_fileManager;
    
    QStringList getSelectedFiles() const
    {
        // Delegate to file manager service
        return m_fileManager->getSelectedFiles();
    }
    
    EncodingJob createEncodingJobFromUI() const
    {
        // Use data model with validation
        EncodingJob job;
        
        // Get configuration from centralized manager
        job.setThreadCount(GET_CONFIG_INT("encoding/threads", 0));
        job.setPriority(GET_CONFIG_INT("encoding/priority", 0));
        job.setTempDirectory(GET_CONFIG_STRING("files/tempFolder", "/tmp"));
        
        // Validate and sanitize user input
        const QString outputPath = m_uiStateManager->getOutputPath();
        const auto validation = VALIDATE_FILE(outputPath, false);
        if (!validation) {
            job.setValidationError("Invalid output path: " + validation.errorMessage);
            return job;
        }
        job.setOutputPath(validation.sanitizedValue);
        
        // Build parameters using safe methods
        const QStringList params = buildEncodingParameters();
        job.setParameters(InputValidator::sanitizeFFmpegParameters(params));
        
        return job;
    }
    
    QStringList buildEncodingParameters() const
    {
        // Use builder pattern for complex parameter construction
        EncodingParameterBuilder builder;
        
        builder.setVideoCodec(m_uiStateManager->getSelectedVideoCodec());
        builder.setAudioCodec(m_uiStateManager->getSelectedAudioCodec());
        
        const QString bitrate = m_uiStateManager->getBitrate();
        const auto bitrateValidation = InputValidator::validateBitrate(bitrate);
        if (bitrateValidation) {
            builder.setBitrate(bitrateValidation.sanitizedValue);
        }
        
        const QString resolution = m_uiStateManager->getResolution();
        const auto resValidation = InputValidator::validateResolution(
            resolution.split('x').first(), resolution.split('x').last());
        if (resValidation) {
            builder.setResolution(resValidation.sanitizedValue);
        }
        
        return builder.build();
    }
    
    void updateUIForEncodingStart()
    {
        // Delegate UI state management to dedicated service
        m_uiStateManager->setEncodingState(true);
        LOG_DEBUG("UI updated for encoding start");
    }
};

// ============================================================================
// SUPPORTING CLASSES: New service classes that support the refactored approach
// ============================================================================

class EncodingJob
{
public:
    bool isValid() const { return m_validationError.isEmpty(); }
    QString getValidationError() const { return m_validationError; }
    void setValidationError(const QString &error) { m_validationError = error; }
    
    void setThreadCount(int count) { m_threadCount = count; }
    void setPriority(int priority) { m_priority = priority; }
    void setTempDirectory(const QString &dir) { m_tempDirectory = dir; }
    void setOutputPath(const QString &path) { m_outputPath = path; }
    void setParameters(const QStringList &params) { m_parameters = params; }
    
    int getThreadCount() const { return m_threadCount; }
    int getPriority() const { return m_priority; }
    QString getTempDirectory() const { return m_tempDirectory; }
    QString getOutputPath() const { return m_outputPath; }
    QStringList getParameters() const { return m_parameters; }
    
private:
    QString m_validationError;
    int m_threadCount = 0;
    int m_priority = 0;
    QString m_tempDirectory;
    QString m_outputPath;
    QStringList m_parameters;
};

class EncodingParameterBuilder
{
public:
    EncodingParameterBuilder& setVideoCodec(const QString &codec) {
        if (!codec.isEmpty()) {
            m_params << "-c:v" << codec;
        }
        return *this;
    }
    
    EncodingParameterBuilder& setAudioCodec(const QString &codec) {
        if (!codec.isEmpty()) {
            m_params << "-c:a" << codec;
        }
        return *this;
    }
    
    EncodingParameterBuilder& setBitrate(const QString &bitrate) {
        if (!bitrate.isEmpty()) {
            m_params << "-b:v" << bitrate + "k";
        }
        return *this;
    }
    
    EncodingParameterBuilder& setResolution(const QString &resolution) {
        if (!resolution.isEmpty()) {
            m_params << "-s" << resolution;
        }
        return *this;
    }
    
    QStringList build() const { return m_params; }
    
private:
    QStringList m_params;
};

class UIStateManager
{
public:
    void setEncodingState(bool encoding) {
        // Centralized UI state management
        LOG_DEBUG(QString("Setting encoding state: %1").arg(encoding ? "true" : "false"));
        // Update UI elements through signals/slots or direct access
    }
    
    QString getOutputPath() const { return "/path/to/output"; }
    QString getSelectedVideoCodec() const { return "libx264"; }
    QString getSelectedAudioCodec() const { return "aac"; }
    QString getBitrate() const { return "2000"; }
    QString getResolution() const { return "1920x1080"; }
};

class FileSelectionManager
{
public:
    QStringList getSelectedFiles() const {
        // Manage file selection logic
        return QStringList{"/path/to/input.mp4"};
    }
};

// ============================================================================
// COMPARISON SUMMARY
// ============================================================================

/*
 * BEFORE (MainWindowOld::onStartEncoding_OLD_WAY):
 * - 40+ lines of mixed concerns
 * - No error handling or validation
 * - Direct UI manipulation mixed with business logic
 * - Manual memory management
 * - Hard to test (requires full UI setup)
 * - Brittle (UI changes break business logic)
 * 
 * AFTER (MainWindowNew::onStartEncoding_NEW_WAY):
 * - 15 lines of focused coordination logic
 * - Comprehensive error handling and validation
 * - Clear separation between UI and business logic
 * - Automatic memory management with smart pointers
 * - Easy to test (mock service dependencies)
 * - Resilient (UI changes don't affect business logic)
 * 
 * NEW SUPPORTING CLASSES:
 * - EncodingJob: Pure data model with validation
 * - EncodingParameterBuilder: Safe parameter construction
 * - UIStateManager: Centralized UI state management
 * - FileSelectionManager: Dedicated file handling
 * 
 * BENEFITS:
 * ✓ Single Responsibility Principle: Each class has one job
 * ✓ Open/Closed Principle: Easy to extend without modification
 * ✓ Dependency Inversion: UI depends on abstractions, not concretions
 * ✓ Testability: Business logic can be unit tested
 * ✓ Maintainability: Changes are localized to specific areas
 * ✓ Reliability: Comprehensive error handling prevents crashes
 * ✓ Security: Input validation prevents malicious input
 * ✓ Usability: Better error messages guide users
 */