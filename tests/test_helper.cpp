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
    void testMakeFileStringFFMPEGReady_singleQuote();
    void testMakeFileStringFFMPEGReady_colon();
    void testMakeFileStringFFMPEGReady_backslash();
    void testMakeFileStringFFMPEGReady_space();
    void testMakeFileStringFFMPEGReady_brackets();
    void testMakeFileStringFFMPEGReady_complex();
};

void TestHelper::testMakeFileStringFFMPEGReady_singleQuote()
{
    // Test filename with single quote
    QString input = "Don't.mkv";
    QString result = Helper::makeFileStringFFMPEGReady(input);
    
    // Single quote should be escaped as \\\' (three backslashes + quote)
    // In C++ string literal: "\\\\\\\'" 
    QString expected = "Don\\\\\\\\'t.mkv";
    QCOMPARE(result, expected);
}

void TestHelper::testMakeFileStringFFMPEGReady_colon()
{
    // Test filename with colon
    QString input = "Test:File.mkv";
    QString result = Helper::makeFileStringFFMPEGReady(input);
    
    // Colon should be escaped as \\:
    QString expected = "Test\\\\:File.mkv";
    QCOMPARE(result, expected);
}

void TestHelper::testMakeFileStringFFMPEGReady_backslash()
{
    // Test filename with backslash
    QString input = "Test\\File.mkv";
    QString result = Helper::makeFileStringFFMPEGReady(input);
    
    // Backslash should be escaped as \\\\
    QString expected = "Test\\\\\\\\\\\\\\\\File.mkv";
    QCOMPARE(result, expected);
}

void TestHelper::testMakeFileStringFFMPEGReady_space()
{
    // Test filename with space
    QString input = "Test File.mkv";
    QString result = Helper::makeFileStringFFMPEGReady(input);
    
    // Space should be escaped as \ 
    QString expected = "Test\\ File.mkv";
    QCOMPARE(result, expected);
}

void TestHelper::testMakeFileStringFFMPEGReady_brackets()
{
    // Test filename with brackets
    QString input = "Test[1].mkv";
    QString result = Helper::makeFileStringFFMPEGReady(input);
    
    // Brackets should be escaped
    QString expected = "Test\\[1\\].mkv";
    QCOMPARE(result, expected);
}

void TestHelper::testMakeFileStringFFMPEGReady_complex()
{
    // Test filename with multiple special characters
    QString input = "Don't Test [2023].mkv";
    QString result = Helper::makeFileStringFFMPEGReady(input);
    
    // All special characters should be properly escaped
    QString expected = "Don\\\\\\\\'t\\ Test\\ \\[2023\\].mkv";
    QCOMPARE(result, expected);
}

QTEST_MAIN(TestHelper)
#include "test_helper.moc"
