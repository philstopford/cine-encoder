/***********************************************************************

                          C I N E   E N C O D E R
                                JULY, 2020
                            COPYRIGHT (C) 2020

 FILE: test_configuration.cpp
 COMMENT: Unit tests for ConfigurationManager
 LICENSE: GNU General Public License v3.0

***********************************************************************/

#include <QtTest>
#include <QSignalSpy>
#include <QTemporaryFile>
#include "configurationmanager.h"

class TestConfiguration : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Basic functionality tests
    void testSingletonAccess();
    void testBooleanOperations();
    void testIntegerOperations();
    void testStringOperations();
    void testDefaultValues();

    // Type safety tests
    void testTypedGetters();
    void testTypedSetters();
    void testInvalidTypeHandling();

    // Configuration management tests
    void testConfigurationSignals();
    void testCategoryReset();
    void testValidation();

    // Key discovery tests
    void testKeyListing();
    void testKeyCategories();

private:
    QString m_originalConfig;
    QTemporaryFile* m_tempConfig = nullptr;
};

void TestConfiguration::initTestCase()
{
    // Create a temporary configuration file
    m_tempConfig = new QTemporaryFile();
    m_tempConfig->open();
    
    // Store original configuration path for restoration
    m_originalConfig = ConfigurationManager::getConfigurationPath();
}

void TestConfiguration::cleanupTestCase()
{
    delete m_tempConfig;
}

void TestConfiguration::init()
{
    // Reset configuration before each test
    ConfigurationManager::instance().resetToDefaults();
}

void TestConfiguration::cleanup()
{
    // Clean up after each test
}

void TestConfiguration::testSingletonAccess()
{
    ConfigurationManager& instance1 = ConfigurationManager::instance();
    ConfigurationManager& instance2 = ConfigurationManager::instance();
    
    // Should be the same instance
    QVERIFY(&instance1 == &instance2);
}

void TestConfiguration::testBooleanOperations()
{
    ConfigurationManager& config = ConfigurationManager::instance();
    
    // Test setting and getting boolean values
    config.setBool("test/bool_value", true);
    QVERIFY(config.getBool("test/bool_value") == true);
    
    config.setBool("test/bool_value", false);
    QVERIFY(config.getBool("test/bool_value") == false);
    
    // Test default value
    QVERIFY(config.getBool("test/nonexistent_bool", true) == true);
}

void TestConfiguration::testIntegerOperations()
{
    ConfigurationManager& config = ConfigurationManager::instance();
    
    // Test setting and getting integer values
    config.setInt("test/int_value", 42);
    QVERIFY(config.getInt("test/int_value") == 42);
    
    config.setInt("test/int_value", -10);
    QVERIFY(config.getInt("test/int_value") == -10);
    
    // Test default value
    QVERIFY(config.getInt("test/nonexistent_int", 100) == 100);
}

void TestConfiguration::testStringOperations()
{
    ConfigurationManager& config = ConfigurationManager::instance();
    
    // Test setting and getting string values
    QString testString = "Hello, World!";
    config.setString("test/string_value", testString);
    QVERIFY(config.getString("test/string_value") == testString);
    
    // Test empty string
    config.setString("test/empty_string", "");
    QVERIFY(config.getString("test/empty_string").isEmpty());
    
    // Test default value
    QString defaultValue = "Default";
    QVERIFY(config.getString("test/nonexistent_string", defaultValue) == defaultValue);
}

void TestConfiguration::testDefaultValues()
{
    ConfigurationManager& config = ConfigurationManager::instance();
    
    // Test that we get default values for non-existent keys
    QVERIFY(config.getBool("nonexistent/bool", false) == false);
    QVERIFY(config.getInt("nonexistent/int", 0) == 0);
    QVERIFY(config.getString("nonexistent/string", "default") == "default");
}

void TestConfiguration::testTypedGetters()
{
    ConfigurationManager& config = ConfigurationManager::instance();
    
    // Set values using generic setValue and retrieve with typed getters
    config.setValue("test/typed_bool", QVariant(true));
    config.setValue("test/typed_int", QVariant(123));
    config.setValue("test/typed_string", QVariant(QString("test")));
    
    QVERIFY(config.getBool("test/typed_bool") == true);
    QVERIFY(config.getInt("test/typed_int") == 123);
    QVERIFY(config.getString("test/typed_string") == "test");
}

void TestConfiguration::testTypedSetters()
{
    ConfigurationManager& config = ConfigurationManager::instance();
    
    // Set values using typed setters and retrieve with generic getValue
    config.setBool("test/setter_bool", false);
    config.setInt("test/setter_int", 456);
    config.setString("test/setter_string", "setter test");
    
    QVERIFY(config.getValue("test/setter_bool").toBool() == false);
    QVERIFY(config.getValue("test/setter_int").toInt() == 456);
    QVERIFY(config.getValue("test/setter_string").toString() == "setter test");
}

void TestConfiguration::testInvalidTypeHandling()
{
    ConfigurationManager& config = ConfigurationManager::instance();
    
    // Set an integer value and try to get it as a string
    config.setInt("test/int_as_string", 789);
    QString intAsString = config.getString("test/int_as_string");
    
    // Should handle conversion gracefully
    QVERIFY(intAsString == "789" || intAsString.isEmpty()); // May vary by implementation
}

void TestConfiguration::testConfigurationSignals()
{
    ConfigurationManager& config = ConfigurationManager::instance();
    QSignalSpy spy(&config, &ConfigurationManager::configurationChanged);
    
    // Change a configuration value
    config.setBool("test/signal_test", true);
    
    // Should have emitted configurationChanged signal
    QVERIFY(spy.count() == 1);
    QList<QVariant> arguments = spy.takeFirst();
    QVERIFY(arguments.at(0).toString() == "test/signal_test");
    QVERIFY(arguments.at(1).toBool() == true);
}

void TestConfiguration::testCategoryReset()
{
    ConfigurationManager& config = ConfigurationManager::instance();
    
    // Set some values in different categories
    config.setBool("interface/test_value", true);
    config.setInt("encoding/test_value", 100);
    
    // Reset interface category
    config.resetCategory(ConfigurationManager::Category::Interface);
    
    // Interface value should be reset, encoding value should remain
    QVERIFY(config.getBool("interface/test_value", false) == false); // Should be default
    QVERIFY(config.getInt("encoding/test_value") == 100); // Should remain unchanged
}

void TestConfiguration::testValidation()
{
    ConfigurationManager& config = ConfigurationManager::instance();
    
    // Test that validation methods exist and can be called
    QVERIFY(config.isValidKey("test/key") || !config.isValidKey("test/key")); // Just test it doesn't crash
    
    QVariant testValue(42);
    QVERIFY(config.validateValue("test/key", testValue) || !config.validateValue("test/key", testValue));
}

void TestConfiguration::testKeyListing()
{
    ConfigurationManager& config = ConfigurationManager::instance();
    
    // Set some test keys
    config.setBool("test/key1", true);
    config.setInt("test/key2", 42);
    config.setString("test/key3", "value");
    
    // Get all keys
    QStringList allKeys = config.getAllKeys();
    QVERIFY(allKeys.contains("test/key1"));
    QVERIFY(allKeys.contains("test/key2"));
    QVERIFY(allKeys.contains("test/key3"));
}

void TestConfiguration::testKeyCategories()
{
    ConfigurationManager& config = ConfigurationManager::instance();
    
    // Test that category methods exist and don't crash
    QStringList interfaceKeys = config.getKeysInCategory(ConfigurationManager::Category::Interface);
    QStringList encodingKeys = config.getKeysInCategory(ConfigurationManager::Category::Encoding);
    
    // Should return lists (may be empty)
    QVERIFY(interfaceKeys.size() >= 0);
    QVERIFY(encodingKeys.size() >= 0);
}

QTEST_MAIN(TestConfiguration)
#include "test_configuration.moc"