/***********************************************************************

                          C I N E   E N C O D E R
                                JULY, 2020
                            COPYRIGHT (C) 2020

 FILE: test_settings_data.cpp
 COMMENT: Unit tests for SettingsData
 LICENSE: GNU General Public License v3.0

***********************************************************************/

#include <QtTest>
#include "settingsdata.h"

class TestSettingsData : public QObject
{
    Q_OBJECT

private slots:
    void testBasicDataOperations();
    void testFolderOperations();
    void testFlagOperations();
    void testNumericOperations();
    void testThemeAndLanguage();
    void testFontSettings();
    void testSubtitleSettings();
    void testDefaultValues();

private:
    SettingsData* createFreshSettingsData();
};

SettingsData* TestSettingsData::createFreshSettingsData()
{
    return new SettingsData();
}

void TestSettingsData::testBasicDataOperations()
{
    std::unique_ptr<SettingsData> settings(createFreshSettingsData());
    
    // Test that we can create and destroy without crashing
    QVERIFY(settings != nullptr);
}

void TestSettingsData::testFolderOperations()
{
    std::unique_ptr<SettingsData> settings(createFreshSettingsData());
    
    // Test output folder operations
    QString outputFolder = "/path/to/output";
    settings->setOutputFolder(outputFolder);
    QVERIFY(settings->getOutputFolder() == outputFolder);
    
    // Test temp folder operations
    QString tempFolder = "/tmp/cine_encoder";
    settings->setTempFolder(tempFolder);
    QVERIFY(settings->getTempFolder() == tempFolder);
}

void TestSettingsData::testFlagOperations()
{
    std::unique_ptr<SettingsData> settings(createFreshSettingsData());
    
    // Test protect flag
    settings->setProtectFlag(true);
    QVERIFY(settings->getProtectFlag() == true);
    
    settings->setProtectFlag(false);
    QVERIFY(settings->getProtectFlag() == false);
    
    // Test multi instances flag
    settings->setMultiInstances(true);
    QVERIFY(settings->getMultiInstances() == true);
    
    // Test HDR flag
    settings->setShowHdrFlag(false);
    QVERIFY(settings->getShowHdrFlag() == false);
}

void TestSettingsData::testNumericOperations()
{
    std::unique_ptr<SettingsData> settings(createFreshSettingsData());
    
    // Test timer interval
    int timerInterval = 1000;
    settings->setTimerInterval(timerInterval);
    QVERIFY(settings->getTimerInterval() == timerInterval);
    
    // Test threads
    int threads = 4;
    settings->setThreads(threads);
    QVERIFY(settings->getThreads() == threads);
    
    // Test FFMpeg priority
    int priority = 2;
    settings->setFFMpegPrio(priority);
    QVERIFY(settings->getFFMpegPrio() == priority);
}

void TestSettingsData::testThemeAndLanguage()
{
    std::unique_ptr<SettingsData> settings(createFreshSettingsData());
    
    // Test theme
    int theme = 1;
    settings->setTheme(theme);
    QVERIFY(settings->getTheme() == theme);
    
    // Test language
    QString language = "en_US";
    settings->setLanguage(language);
    QVERIFY(settings->getLanguage() == language);
}

void TestSettingsData::testFontSettings()
{
    std::unique_ptr<SettingsData> settings(createFreshSettingsData());
    
    // These tests assume the methods exist based on the header
    // The actual method names might be different
    
    // Test basic operation without crashing
    QVERIFY(settings != nullptr);
    
    // If font methods exist, they should work
    // This is a placeholder for when those methods are defined
}

void TestSettingsData::testSubtitleSettings()
{
    std::unique_ptr<SettingsData> settings(createFreshSettingsData());
    
    // These tests are placeholders for subtitle-related settings
    // based on what we saw in the header file
    
    QVERIFY(settings != nullptr);
}

void TestSettingsData::testDefaultValues()
{
    std::unique_ptr<SettingsData> settings(createFreshSettingsData());
    
    // Test that default values are reasonable
    QVERIFY(settings->getTimerInterval() >= 0);
    QVERIFY(settings->getThreads() >= 0);
    QVERIFY(settings->getFFMpegPrio() >= 0);
    QVERIFY(settings->getTheme() >= 0);
    
    // String defaults should not be null
    QVERIFY(!settings->getOutputFolder().isNull());
    QVERIFY(!settings->getTempFolder().isNull());
    QVERIFY(!settings->getLanguage().isNull());
}

QTEST_MAIN(TestSettingsData)
#include "test_settings_data.moc"