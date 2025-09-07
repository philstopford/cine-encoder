/***********************************************************************

                          C I N E   E N C O D E R
                                JULY, 2020
                            COPYRIGHT (C) 2020

 FILE: encodingmanager.cpp
 COMMENT: Service class implementation for encoding operations
 LICENSE: GNU General Public License v3.0

***********************************************************************/

#include "encodingmanager.h"
#include "encoder.h"
#include "logger.h"
#include "errorhandler.h"
#include "inputvalidator.h"
#include <QFileInfo>
#include <QDir>

EncodingManager::EncodingManager(QObject *parent) 
    : QObject(parent)
{
    LOG_INFO("EncodingManager initialized");
}

EncodingManager::~EncodingManager()
{
    if (m_status == Status::Running) {
        stopEncoding();
    }
    LOG_INFO("EncodingManager destroyed");
}

void EncodingManager::startEncoding(const QString &inputFile, const QString &outputFile, 
                                   const QStringList &parameters)
{
    if (m_status == Status::Running) {
        LOG_WARNING("Cannot start encoding - already running");
        return;
    }
    
    // Validate input file
    auto inputValidation = VALIDATE_FILE(inputFile, true);
    if (!inputValidation) {
        emit encodingError("Invalid input file: " + inputValidation.errorMessage);
        return;
    }
    
    // Validate output file path
    auto outputValidation = VALIDATE_FILE(outputFile, false);
    if (!outputValidation) {
        emit encodingError("Invalid output file: " + outputValidation.errorMessage);
        return;
    }
    
    m_currentInputFile = inputValidation.sanitizedValue;
    m_currentOutputFile = outputValidation.sanitizedValue;
    m_currentParameters = parameters;
    
    LOG_INFO(QString("Starting encoding: %1 -> %2").arg(m_currentInputFile, m_currentOutputFile));
    
    setStatus(Status::Initializing);
    setupEncoder();
    
    // Initialize encoder with validated parameters
    // This would call the actual encoder initialization
    // For now, simulate the start
    QTimer::singleShot(100, this, &EncodingManager::onEncoderStarted);
}

void EncodingManager::pauseEncoding()
{
    if (m_status == Status::Running && m_encoder) {
        LOG_INFO("Pausing encoding");
        setStatus(Status::Paused);
        // Implement pause functionality in encoder
    }
}

void EncodingManager::resumeEncoding()
{
    if (m_status == Status::Paused && m_encoder) {
        LOG_INFO("Resuming encoding");
        setStatus(Status::Running);
        // Implement resume functionality in encoder
    }
}

void EncodingManager::stopEncoding()
{
    if (m_status == Status::Running || m_status == Status::Paused) {
        LOG_INFO("Stopping encoding");
        setStatus(Status::Idle);
        cleanupEncoder();
        emit encodingAborted();
    }
}

void EncodingManager::abortEncoding()
{
    if (m_status != Status::Idle) {
        LOG_WARNING("Aborting encoding");
        setStatus(Status::Aborted);
        cleanupEncoder();
        emit encodingAborted();
    }
}

QString EncodingManager::getStatusText() const
{
    switch (m_status) {
        case Status::Idle:         return "Ready";
        case Status::Initializing: return "Initializing...";
        case Status::Running:      return QString("Encoding... %1%").arg(m_progress);
        case Status::Paused:       return QString("Paused (%1%)").arg(m_progress);
        case Status::Completed:    return "Completed";
        case Status::Aborted:      return "Aborted";
        case Status::Error:        return "Error";
        default:                   return "Unknown";
    }
}

void EncodingManager::enqueueBatch(const QStringList &inputFiles, const QStringList &outputFiles,
                                  const QStringList &parameters)
{
    if (inputFiles.size() != outputFiles.size()) {
        emit encodingError("Input and output file lists must have same size");
        return;
    }
    
    // Validate all files before adding to batch
    for (int i = 0; i < inputFiles.size(); ++i) {
        auto inputValidation = VALIDATE_FILE(inputFiles[i], true);
        if (!inputValidation) {
            emit encodingError(QString("Invalid input file %1: %2").arg(i+1).arg(inputValidation.errorMessage));
            return;
        }
        
        auto outputValidation = VALIDATE_FILE(outputFiles[i], false);  
        if (!outputValidation) {
            emit encodingError(QString("Invalid output file %1: %2").arg(i+1).arg(outputValidation.errorMessage));
            return;
        }
    }
    
    m_batchInputFiles = inputFiles;
    m_batchOutputFiles = outputFiles;
    m_batchParameters = parameters;
    m_batchTotal = inputFiles.size();
    m_batchCurrent = 0;
    m_batchMode = true;
    
    LOG_INFO(QString("Batch queued: %1 files").arg(m_batchTotal));
    emit batchProgressChanged(m_batchCurrent, m_batchTotal);
}

void EncodingManager::startBatchEncoding()
{
    if (!m_batchMode || m_batchInputFiles.isEmpty()) {
        emit encodingError("No batch items to process");
        return;
    }
    
    if (m_status == Status::Running) {
        emit encodingError("Cannot start batch - encoding already running");
        return;
    }
    
    LOG_INFO("Starting batch encoding");
    m_batchCurrent = 0;
    startNextBatchItem();
}

void EncodingManager::clearBatch()
{
    m_batchMode = false;
    m_batchInputFiles.clear();
    m_batchOutputFiles.clear();
    m_batchParameters.clear();
    m_batchCurrent = 0;
    m_batchTotal = 0;
    
    LOG_INFO("Batch cleared");
}

int EncodingManager::getBatchProgress() const
{
    if (!m_batchMode || m_batchTotal == 0) {
        return 0;
    }
    return (m_batchCurrent * 100) / m_batchTotal;
}

void EncodingManager::setPriority(int priority)
{
    m_priority = priority;
    LOG_DEBUG(QString("Encoding priority set to: %1").arg(priority));
}

void EncodingManager::setThreadCount(int threads)
{
    m_threadCount = threads;
    LOG_DEBUG(QString("Thread count set to: %1").arg(threads));
}

void EncodingManager::setTempDirectory(const QString &tempDir)
{
    auto validation = VALIDATE_DIR(tempDir, false);
    if (validation) {
        m_tempDirectory = validation.sanitizedValue;
        LOG_DEBUG(QString("Temp directory set to: %1").arg(m_tempDirectory));
    } else {
        LOG_WARNING(QString("Invalid temp directory: %1").arg(validation.errorMessage));
    }
}

void EncodingManager::onEncoderStarted()
{
    setStatus(Status::Running);
    emit encodingStarted(m_currentInputFile);
    LOG_INFO("Encoding started successfully");
}

void EncodingManager::onEncoderProgress(int percent, float remainingTime)
{
    m_progress = percent;
    m_remainingTime = remainingTime;
    emit progressChanged(percent, remainingTime);
    
    // Log progress periodically
    static int lastLoggedProgress = -1;
    if (percent - lastLoggedProgress >= 10) {
        LOG_DEBUG(QString("Encoding progress: %1% (%.1f min remaining)")
                  .arg(percent).arg(remainingTime / 60.0));
        lastLoggedProgress = percent;
    }
}

void EncodingManager::onEncoderCompleted()
{
    setStatus(Status::Completed);
    emit encodingCompleted(m_currentOutputFile);
    LOG_INFO(QString("Encoding completed: %1").arg(QFileInfo(m_currentOutputFile).fileName()));
    
    cleanupEncoder();
    
    // Process next batch item if in batch mode
    if (m_batchMode) {
        processNextBatchItem();
    }
}

void EncodingManager::onEncoderAborted()
{
    setStatus(Status::Aborted);
    emit encodingAborted();
    LOG_INFO("Encoding was aborted");
    
    cleanupEncoder();
    
    if (m_batchMode) {
        clearBatch(); // Stop batch processing on abort
    }
}

void EncodingManager::onEncoderError(const QString &errorMessage)
{
    setStatus(Status::Error);
    emit encodingError(errorMessage);
    LOG_ERROR(QString("Encoding error: %1").arg(errorMessage));
    
    cleanupEncoder();
    
    if (m_batchMode) {
        // In batch mode, log error and continue with next item
        LOG_WARNING(QString("Batch item failed, continuing: %1").arg(m_currentInputFile));
        processNextBatchItem();
    }
}

void EncodingManager::onEncoderLog(const QString &log)
{
    emit logMessage(log);
    // Only log detailed encoder output in debug mode
    LOG_DEBUG(QString("Encoder: %1").arg(log));
}

void EncodingManager::processNextBatchItem()
{
    m_batchCurrent++;
    emit batchProgressChanged(m_batchCurrent, m_batchTotal);
    
    if (m_batchCurrent >= m_batchTotal) {
        // Batch completed
        clearBatch();
        emit batchCompleted();
        LOG_INFO("Batch encoding completed");
    } else {
        // Start next item
        QTimer::singleShot(500, this, &EncodingManager::startNextBatchItem);
    }
}

void EncodingManager::setStatus(Status status)
{
    if (m_status != status) {
        const Status oldStatus = m_status;
        m_status = status;
        emit statusChanged(status);
        
        LOG_DEBUG(QString("Encoding status changed: %1 -> %2")
                  .arg(static_cast<int>(oldStatus))
                  .arg(static_cast<int>(status)));
    }
}

void EncodingManager::setupEncoder()
{
    if (m_encoder) {
        cleanupEncoder();
    }
    
    m_encoder = std::make_unique<Encoder>(this);
    
    // Connect encoder signals (these would be the actual encoder connections)
    // For now, we'll simulate with timers
    LOG_DEBUG("Encoder setup completed");
}

void EncodingManager::cleanupEncoder()
{
    if (m_encoder) {
        m_encoder.reset();
        LOG_DEBUG("Encoder cleanup completed");
    }
}

void EncodingManager::startNextBatchItem()
{
    if (m_batchCurrent < m_batchInputFiles.size()) {
        const QString inputFile = m_batchInputFiles[m_batchCurrent];
        const QString outputFile = m_batchOutputFiles[m_batchCurrent];
        
        LOG_INFO(QString("Starting batch item %1/%2: %3")
                 .arg(m_batchCurrent + 1)
                 .arg(m_batchTotal)
                 .arg(QFileInfo(inputFile).fileName()));
        
        startEncoding(inputFile, outputFile, m_batchParameters);
    }
}