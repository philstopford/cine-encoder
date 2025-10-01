/***********************************************************************

                          C I N E   E N C O D E R
                                JULY, 2020
                            COPYRIGHT (C) 2020

 FILE: test_input_validator.cpp
 COMMENT: Unit tests for InputValidator
 LICENSE: GNU General Public License v3.0

***********************************************************************/

#include <QtTest>
#include <QTemporaryFile>
#include <QDir>
#include "inputvalidator.h"

class TestInputValidator : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    // File validation tests
    void testFilePathValidation();
    void testDirectoryPathValidation();
    void testFileNameValidation();

    // Numeric validation tests
    void testNumberValidation();
    void testIntegerValidation();
    void testBitrateValidation();
    void testFrameRateValidation();

    // Video/Audio specific tests
    void testResolutionValidation();
    void testTimeValueValidation();

    // Sanitization tests
    void testFilePathSanitization();
    void testFileNameSanitization();
    void testFFmpegParameterSanitization();

    // Extension validation tests
    void testVideoExtensionValidation();
    void testAudioExtensionValidation();
    void testSubtitleExtensionValidation();

private:
    QTemporaryFile* m_tempFile = nullptr;
    QDir m_tempDir;
};

void TestInputValidator::initTestCase()
{
    // Create a temporary file for testing
    m_tempFile = new QTemporaryFile();
    m_tempFile->open();
    
    // Create a temporary directory
    m_tempDir = QDir::temp();
    m_tempDir.mkpath("input_validator_test");
    m_tempDir.cd("input_validator_test");
}

void TestInputValidator::cleanupTestCase()
{
    delete m_tempFile;
    
    // Clean up temporary directory
    m_tempDir.cdUp();
    m_tempDir.rmdir("input_validator_test");
}

void TestInputValidator::testFilePathValidation()
{
    // Test valid existing file
    auto result = InputValidator::validateFilePath(m_tempFile->fileName(), true);
    QVERIFY(result.isValid);
    QVERIFY(result.errorMessage.isEmpty());

    // Test non-existent file (when must exist)
    result = InputValidator::validateFilePath("/non/existent/file.txt", true);
    QVERIFY(!result.isValid);
    QVERIFY(!result.errorMessage.isEmpty());

    // Test non-existent file (when existence not required)
    result = InputValidator::validateFilePath("/valid/path/newfile.txt", false);
    QVERIFY(result.isValid);

    // Test invalid path characters
    result = InputValidator::validateFilePath("invalid<>path", false);
    QVERIFY(!result.isValid);
}

void TestInputValidator::testDirectoryPathValidation()
{
    // Test valid existing directory
    auto result = InputValidator::validateDirectoryPath(m_tempDir.absolutePath(), true);
    QVERIFY(result.isValid);

    // Test non-existent directory (when must exist)
    result = InputValidator::validateDirectoryPath("/non/existent/directory", true);
    QVERIFY(!result.isValid);

    // Test non-existent directory (when existence not required)
    result = InputValidator::validateDirectoryPath("/valid/new/directory", false);
    QVERIFY(result.isValid);
}

void TestInputValidator::testFileNameValidation()
{
    // Test valid filename
    auto result = InputValidator::validateFileName("valid_filename.txt");
    QVERIFY(result.isValid);
    QVERIFY(result.sanitizedValue == "valid_filename.txt");

    // Test filename with invalid characters
    result = InputValidator::validateFileName("invalid<>filename.txt");
    QVERIFY(!result.isValid || result.sanitizedValue != "invalid<>filename.txt");

    // Test empty filename
    result = InputValidator::validateFileName("");
    QVERIFY(!result.isValid);

    // Test filename that's too long
    QString longName(300, 'a');
    longName += ".txt";
    result = InputValidator::validateFileName(longName);
    QVERIFY(!result.isValid);
}

void TestInputValidator::testNumberValidation()
{
    // Test valid numbers
    auto result = InputValidator::validateNumber("42.5", 0, 100);
    QVERIFY(result.isValid);
    QVERIFY(result.sanitizedValue.toDouble() == 42.5);

    // Test number out of range (too high)
    result = InputValidator::validateNumber("150", 0, 100);
    QVERIFY(!result.isValid);

    // Test number out of range (too low)
    result = InputValidator::validateNumber("-10", 0, 100);
    QVERIFY(!result.isValid);

    // Test invalid number format
    result = InputValidator::validateNumber("not_a_number", 0, 100);
    QVERIFY(!result.isValid);

    // Test valid negative number with appropriate range
    result = InputValidator::validateNumber("-5.5", -10, 10);
    QVERIFY(result.isValid);
}

void TestInputValidator::testIntegerValidation()
{
    // Test valid integer
    auto result = InputValidator::validateInteger("42", 0, 100);
    QVERIFY(result.isValid);
    QVERIFY(result.sanitizedValue.toInt() == 42);

    // Test invalid integer (floating point)
    result = InputValidator::validateInteger("42.5", 0, 100);
    QVERIFY(!result.isValid);

    // Test integer out of range
    result = InputValidator::validateInteger("150", 0, 100);
    QVERIFY(!result.isValid);

    // Test non-numeric string
    result = InputValidator::validateInteger("abc", 0, 100);
    QVERIFY(!result.isValid);
}

void TestInputValidator::testBitrateValidation()
{
    // Test valid bitrates
    auto result = InputValidator::validateBitrate("1000");
    QVERIFY(result.isValid);

    result = InputValidator::validateBitrate("2000k");
    QVERIFY(result.isValid);

    result = InputValidator::validateBitrate("5M");
    QVERIFY(result.isValid);

    // Test invalid bitrates
    result = InputValidator::validateBitrate("invalid");
    QVERIFY(!result.isValid);

    result = InputValidator::validateBitrate("0");
    QVERIFY(!result.isValid);
}

void TestInputValidator::testFrameRateValidation()
{
    // Test common frame rates
    auto result = InputValidator::validateFrameRate("24");
    QVERIFY(result.isValid);

    result = InputValidator::validateFrameRate("29.97");
    QVERIFY(result.isValid);

    result = InputValidator::validateFrameRate("60");
    QVERIFY(result.isValid);

    // Test invalid frame rates
    result = InputValidator::validateFrameRate("0");
    QVERIFY(!result.isValid);

    result = InputValidator::validateFrameRate("invalid");
    QVERIFY(!result.isValid);

    result = InputValidator::validateFrameRate("200"); // Too high
    QVERIFY(!result.isValid);
}

void TestInputValidator::testResolutionValidation()
{
    // Test common resolutions
    auto result = InputValidator::validateResolution("1920", "1080");
    QVERIFY(result.isValid);
    QVERIFY(result.sanitizedValue == "1920x1080");

    result = InputValidator::validateResolution("1280", "720");
    QVERIFY(result.isValid);

    // Test invalid resolutions
    result = InputValidator::validateResolution("0", "1080");
    QVERIFY(!result.isValid);

    result = InputValidator::validateResolution("1920", "0");
    QVERIFY(!result.isValid);

    result = InputValidator::validateResolution("invalid", "1080");
    QVERIFY(!result.isValid);
}

void TestInputValidator::testTimeValueValidation()
{
    // Test valid time formats
    auto result = InputValidator::validateTimeValue("00:01:30");
    QVERIFY(result.isValid);

    result = InputValidator::validateTimeValue("1:30:45.5");
    QVERIFY(result.isValid);

    result = InputValidator::validateTimeValue("90"); // Seconds only
    QVERIFY(result.isValid);

    // Test invalid time formats
    result = InputValidator::validateTimeValue("invalid_time");
    QVERIFY(!result.isValid);

    result = InputValidator::validateTimeValue("25:00:00"); // Invalid hours
    QVERIFY(!result.isValid);

    result = InputValidator::validateTimeValue("00:61:00"); // Invalid minutes
    QVERIFY(!result.isValid);
}

void TestInputValidator::testFilePathSanitization()
{
    // Test sanitization of problematic paths
    QString sanitized = InputValidator::sanitizeFilePath("/path/with spaces/file.txt");
    QVERIFY(!sanitized.isEmpty());

    sanitized = InputValidator::sanitizeFilePath("C:\\Windows\\file.txt");
    QVERIFY(!sanitized.isEmpty());

    // Test that valid paths remain unchanged
    QString validPath = "/home/user/document.txt";
    sanitized = InputValidator::sanitizeFilePath(validPath);
    QVERIFY(sanitized.contains("document.txt"));
}

void TestInputValidator::testFileNameSanitization()
{
    // Test sanitization of problematic filenames
    QString sanitized = InputValidator::sanitizeFileName("file with spaces.txt");
    QVERIFY(!sanitized.isEmpty());
    QVERIFY(!sanitized.contains("<") && !sanitized.contains(">"));

    sanitized = InputValidator::sanitizeFileName("file<>name.txt");
    QVERIFY(!sanitized.contains("<") && !sanitized.contains(">"));

    // Test that valid filenames remain mostly unchanged
    QString validName = "valid_filename.txt";
    sanitized = InputValidator::sanitizeFileName(validName);
    QVERIFY(sanitized == validName || sanitized.contains("valid_filename"));
}

void TestInputValidator::testFFmpegParameterSanitization()
{
    // Test sanitization of FFmpeg parameters
    QString param = "-vcodec libx264";
    QString sanitized = InputValidator::sanitizeFFmpegParameter(param);
    QVERIFY(!sanitized.isEmpty());

    param = "dangerous;command";
    sanitized = InputValidator::sanitizeFFmpegParameter(param);
    QVERIFY(!sanitized.contains(";"));

    // Test parameter list sanitization
    QStringList params = {"-vcodec", "libx264", "-acodec", "aac"};
    QStringList sanitizedList = InputValidator::sanitizeFFmpegParameters(params);
    QVERIFY(sanitizedList.size() == params.size());
}

void TestInputValidator::testVideoExtensionValidation()
{
    // Test common video extensions
    QVERIFY(InputValidator::isValidVideoExtension("mp4"));
    QVERIFY(InputValidator::isValidVideoExtension("avi"));
    QVERIFY(InputValidator::isValidVideoExtension("mkv"));
    QVERIFY(InputValidator::isValidVideoExtension("mov"));

    // Test invalid extensions
    QVERIFY(!InputValidator::isValidVideoExtension("txt"));
    QVERIFY(!InputValidator::isValidVideoExtension("mp3"));
    QVERIFY(!InputValidator::isValidVideoExtension(""));
}

void TestInputValidator::testAudioExtensionValidation()
{
    // Test common audio extensions
    QVERIFY(InputValidator::isValidAudioExtension("mp3"));
    QVERIFY(InputValidator::isValidAudioExtension("wav"));
    QVERIFY(InputValidator::isValidAudioExtension("flac"));
    QVERIFY(InputValidator::isValidAudioExtension("aac"));

    // Test invalid extensions
    QVERIFY(!InputValidator::isValidAudioExtension("txt"));
    QVERIFY(!InputValidator::isValidAudioExtension("mp4"));
    QVERIFY(!InputValidator::isValidAudioExtension(""));
}

void TestInputValidator::testSubtitleExtensionValidation()
{
    // Test common subtitle extensions
    QVERIFY(InputValidator::isValidSubtitleExtension("srt"));
    QVERIFY(InputValidator::isValidSubtitleExtension("ass"));
    QVERIFY(InputValidator::isValidSubtitleExtension("vtt"));

    // Test invalid extensions
    QVERIFY(!InputValidator::isValidSubtitleExtension("txt"));
    QVERIFY(!InputValidator::isValidSubtitleExtension("mp4"));
    QVERIFY(!InputValidator::isValidSubtitleExtension(""));
}

QTEST_MAIN(TestInputValidator)
#include "test_input_validator.moc"