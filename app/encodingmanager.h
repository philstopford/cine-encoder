/***********************************************************************

                          C I N E   E N C O D E R
                                JULY, 2020
                            COPYRIGHT (C) 2020

 FILE: encodingmanager.h
 COMMENT: Service class for managing encoding operations
 LICENSE: GNU General Public License v3.0

***********************************************************************/

#ifndef ENCODINGMANAGER_H
#define ENCODINGMANAGER_H

#include <QObject>
#include <QTimer>
#include <QProcess>
#include <memory>

class Encoder;
class MainWindow;

class EncodingManager : public QObject
{
    Q_OBJECT

public:
    enum class Status {
        Idle,
        Initializing,
        Running,
        Paused,
        Completed,
        Aborted,
        Error
    };

    explicit EncodingManager(QObject *parent = nullptr);
    ~EncodingManager() override;

    // Encoding control
    void startEncoding(const QString &inputFile, const QString &outputFile, 
                      const QStringList &parameters);
    void pauseEncoding();
    void resumeEncoding();
    void stopEncoding();
    void abortEncoding();
    
    // Status and progress
    Status getStatus() const { return m_status; }
    int getProgress() const { return m_progress; }
    float getRemainingTime() const { return m_remainingTime; }
    QString getCurrentFile() const { return m_currentInputFile; }
    QString getStatusText() const;
    
    // Batch encoding
    void enqueueBatch(const QStringList &inputFiles, const QStringList &outputFiles,
                     const QStringList &parameters);
    void startBatchEncoding();
    void clearBatch();
    bool isBatchMode() const { return m_batchMode; }
    int getBatchProgress() const;
    int getBatchTotal() const { return m_batchTotal; }
    int getBatchCurrent() const { return m_batchCurrent; }
    
    // Configuration
    void setPriority(int priority);
    void setThreadCount(int threads);
    void setTempDirectory(const QString &tempDir);

signals:
    void statusChanged(Status status);
    void progressChanged(int percent, float remainingTime);
    void encodingStarted(const QString &inputFile);
    void encodingCompleted(const QString &outputFile);
    void encodingAborted();
    void encodingError(const QString &errorMessage);
    void batchProgressChanged(int current, int total);
    void batchCompleted();
    void logMessage(const QString &message);

private slots:
    void onEncoderStarted();
    void onEncoderProgress(int percent, float remainingTime);
    void onEncoderCompleted();
    void onEncoderAborted();
    void onEncoderError(const QString &errorMessage);
    void onEncoderLog(const QString &log);
    void processNextBatchItem();

private:
    void setStatus(Status status);
    void setupEncoder();
    void cleanupEncoder();
    void startNextBatchItem();
    
    Status m_status = Status::Idle;
    int m_progress = 0;
    float m_remainingTime = 0.0f;
    
    QString m_currentInputFile;
    QString m_currentOutputFile;
    QStringList m_currentParameters;
    
    // Batch processing
    bool m_batchMode = false;
    QStringList m_batchInputFiles;
    QStringList m_batchOutputFiles;
    QStringList m_batchParameters;
    int m_batchCurrent = 0;
    int m_batchTotal = 0;
    
    // Configuration
    int m_priority = 0;
    int m_threadCount = 0;
    QString m_tempDirectory;
    
    // Encoder instance
    std::unique_ptr<Encoder> m_encoder;
};

#endif // ENCODINGMANAGER_H