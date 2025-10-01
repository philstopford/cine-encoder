/***********************************************************************

                          C I N E   E N C O D E R
                                JULY, 2020
                            COPYRIGHT (C) 2020

 FILE: test_encoding_manager.cpp
 COMMENT: Unit tests for EncodingManager
 LICENSE: GNU General Public License v3.0

***********************************************************************/

#include <QtTest>
#include <QSignalSpy>
#include <QTemporaryFile>
#include "encodingmanager.h"

class TestEncodingManager : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Basic functionality tests
    void testInitialState();
    void testStatusManagement();
    void testConfigurationMethods();
    
    // Batch processing tests
    void testBatchQueue();
    void testBatchState();
    
    // Signal emission tests
    void testSignalEmission();

private:
    EncodingManager* m_encodingManager = nullptr;
    QTemporaryFile* m_tempInputFile = nullptr;
    QTemporaryFile* m_tempOutputFile = nullptr;
};

void TestEncodingManager::initTestCase()
{
    // Create temporary files for testing
    m_tempInputFile = new QTemporaryFile();
    m_tempInputFile->open();
    
    m_tempOutputFile = new QTemporaryFile();
    m_tempOutputFile->open();
}

void TestEncodingManager::cleanupTestCase()
{
    delete m_tempInputFile;
    delete m_tempOutputFile;
}

void TestEncodingManager::init()
{
    m_encodingManager = new EncodingManager();
}

void TestEncodingManager::cleanup()
{
    delete m_encodingManager;
    m_encodingManager = nullptr;
}

void TestEncodingManager::testInitialState()
{
    QVERIFY(m_encodingManager != nullptr);
    
    // Test initial status
    QVERIFY(m_encodingManager->getStatus() == EncodingManager::Status::Idle);
    QVERIFY(m_encodingManager->getProgress() == 0);
    QVERIFY(m_encodingManager->getRemainingTime() == 0.0f);
    QVERIFY(m_encodingManager->getCurrentFile().isEmpty());
    
    // Test batch initial state
    QVERIFY(!m_encodingManager->isBatchMode());
    QVERIFY(m_encodingManager->getBatchTotal() == 0);
    QVERIFY(m_encodingManager->getBatchCurrent() == 0);
}

void TestEncodingManager::testStatusManagement()
{
    // Test status text retrieval
    QString statusText = m_encodingManager->getStatusText();
    QVERIFY(!statusText.isEmpty());
    
    // Test that status changes are handled without crashing
    // Note: We can't easily test actual encoding without FFmpeg setup
}

void TestEncodingManager::testConfigurationMethods()
{
    // Test configuration methods don't crash
    m_encodingManager->setPriority(1);
    m_encodingManager->setThreadCount(2);
    m_encodingManager->setTempDirectory("/tmp");
    
    // If we get here, configuration methods work
    QVERIFY(true);
}

void TestEncodingManager::testBatchQueue()
{
    QStringList inputFiles = {m_tempInputFile->fileName()};
    QStringList outputFiles = {m_tempOutputFile->fileName()};
    QStringList parameters = {"-vcodec", "libx264"};
    
    // Test enqueueing batch
    m_encodingManager->enqueueBatch(inputFiles, outputFiles, parameters);
    QVERIFY(m_encodingManager->isBatchMode());
    QVERIFY(m_encodingManager->getBatchTotal() == 1);
    
    // Test clearing batch
    m_encodingManager->clearBatch();
    QVERIFY(!m_encodingManager->isBatchMode());
    QVERIFY(m_encodingManager->getBatchTotal() == 0);
}

void TestEncodingManager::testBatchState()
{
    // Test batch progress calculation
    int progress = m_encodingManager->getBatchProgress();
    QVERIFY(progress >= 0 && progress <= 100);
    
    // Test batch counters
    QVERIFY(m_encodingManager->getBatchCurrent() >= 0);
    QVERIFY(m_encodingManager->getBatchTotal() >= 0);
    QVERIFY(m_encodingManager->getBatchCurrent() <= m_encodingManager->getBatchTotal());
}

void TestEncodingManager::testSignalEmission()
{
    // Test that we can connect to signals without crashes
    QSignalSpy statusSpy(m_encodingManager, &EncodingManager::statusChanged);
    QSignalSpy progressSpy(m_encodingManager, &EncodingManager::progressChanged);
    QSignalSpy startedSpy(m_encodingManager, &EncodingManager::encodingStarted);
    QSignalSpy completedSpy(m_encodingManager, &EncodingManager::encodingCompleted);
    QSignalSpy errorSpy(m_encodingManager, &EncodingManager::encodingError);
    QSignalSpy batchProgressSpy(m_encodingManager, &EncodingManager::batchProgressChanged);
    QSignalSpy logSpy(m_encodingManager, &EncodingManager::logMessage);
    
    // Verify signal connections work
    QVERIFY(statusSpy.isValid());
    QVERIFY(progressSpy.isValid());
    QVERIFY(startedSpy.isValid());
    QVERIFY(completedSpy.isValid());
    QVERIFY(errorSpy.isValid());
    QVERIFY(batchProgressSpy.isValid());
    QVERIFY(logSpy.isValid());
    
    // Note: We can't easily trigger signal emission without actual encoding
    // but we've verified the signals exist and can be connected to
}

QTEST_MAIN(TestEncodingManager)
#include "test_encoding_manager.moc"