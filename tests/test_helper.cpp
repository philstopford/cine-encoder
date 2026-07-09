/***********************************************************************

                          C I N E   E N C O D E R
                                JULY, 2020
                            COPYRIGHT (C) 2020

 FILE: test_helper.cpp
 COMMENT: Unit tests for Helper utility functions
 LICENSE: GNU General Public License v3.0

***********************************************************************/

#include <QtTest>
#include "helper.h"

class TestHelper : public QObject
{
    Q_OBJECT

private slots:
    void testMakeFileStringFFMPEGFilterReady_singleQuote();
    void testMakeFileStringFFMPEGFilterReady_colon();
    void testMakeFileStringFFMPEGFilterReady_backslash();
    void testMakeFileStringFFMPEGFilterReady_space();
    void testMakeFileStringFFMPEGFilterReady_brackets();
    void testMakeFileStringFFMPEGFilterReady_complex();
    void testMakeFileStringFFMPEGReady_noEscaping();
};

void TestHelper::testMakeFileStringFFMPEGFilterReady_singleQuote()
{
    // Test filename with single quote for filter usage
    QString input1 = "Don't.mkv";  // ASCII apostrophe
    QString result1 = Helper::makeFileStringFFMPEGFilterReady(input1);
    QString expected1 = "Don\\'t.mkv";
    QCOMPARE(result1, expected1);
    
    // Test Unicode right single quotation mark
    QString input2 = QString("Don%1t.mkv").arg(QChar(0x2019));
    QString result2 = Helper::makeFileStringFFMPEGFilterReady(input2);
    QString expected2 = "Don\\'t.mkv";
    QCOMPARE(result2, expected2);
}

void TestHelper::testMakeFileStringFFMPEGFilterReady_colon()
{
    // Test filename with colon for filter usage
    QString input = "Test:File.mkv";
    QString result = Helper::makeFileStringFFMPEGFilterReady(input);
    
    // Colon should be escaped as \:
    QString expected = "Test\\:File.mkv";
    QCOMPARE(result, expected);
}

void TestHelper::testMakeFileStringFFMPEGFilterReady_backslash()
{
    // Test filename with backslash for filter usage
    QString input = "Test\\File.mkv";
    QString result = Helper::makeFileStringFFMPEGFilterReady(input);
    
    // Backslash should be escaped as "\\\\"
    QString expected = "Test\\\\File.mkv";
    QCOMPARE(result, expected);
}

void TestHelper::testMakeFileStringFFMPEGFilterReady_space()
{
    // Test filename with space for filter usage
    QString input = "Test File.mkv";
    QString result = Helper::makeFileStringFFMPEGFilterReady(input);
    
    // Space should be escaped "\ "
    QString expected = "Test\\ File.mkv";
    QCOMPARE(result, expected);
}

void TestHelper::testMakeFileStringFFMPEGFilterReady_brackets()
{
    // Test filename with brackets for filter usage
    QString input = "Test[1].mkv";
    QString result = Helper::makeFileStringFFMPEGFilterReady(input);
    
    // Brackets should be escaped
    QString expected = "Test\\[1\\].mkv";
    QCOMPARE(result, expected);
}

void TestHelper::testMakeFileStringFFMPEGFilterReady_complex()
{
    // Test filename with multiple special characters for filter usage
    QString input = "Don't Test [2023].mkv";
    QString result = Helper::makeFileStringFFMPEGFilterReady(input);
    
    // All special characters should be properly escaped
    // Single quote: \'  Space: \  Brackets: \[ \]
    QString expected = "Don\\'t\\ Test\\ \\[2023\\].mkv";
    QCOMPARE(result, expected);
}

void TestHelper::testMakeFileStringFFMPEGReady_noEscaping()
{
    // Test that makeFileStringFFMPEGReady doesn't escape (used for file paths with QProcess)
    QString input = "Don't Test [2023].mkv";
    QString result = Helper::makeFileStringFFMPEGReady(input);
    
    // Should return the string as-is since QProcess handles escaping
    QCOMPARE(result, input);
}

QTEST_MAIN(TestHelper)
#include "test_helper.moc"
