/***********************************************************************

                          C I N E   E N C O D E R
                                JULY, 2020
                            COPYRIGHT (C) 2020

 FILE: test_service_container.cpp
 COMMENT: Unit tests for ServiceContainer and dependency injection
 LICENSE: GNU General Public License v3.0

***********************************************************************/

#include <QtTest>
#include <memory>
#include "servicecontainer.h"
#include "interfaces.h"

// Mock implementations for testing
class MockLogger : public ILogger
{
public:
    mutable QStringList messages;
    mutable QString lastLevel;
    
    void debug(const QString &message) override {
        lastLevel = "DEBUG";
        messages.append(message);
    }
    
    void info(const QString &message) override {
        lastLevel = "INFO";
        messages.append(message);
    }
    
    void warning(const QString &message) override {
        lastLevel = "WARNING";
        messages.append(message);
    }
    
    void error(const QString &message) override {
        lastLevel = "ERROR";
        messages.append(message);
    }
    
    void critical(const QString &message) override {
        lastLevel = "CRITICAL";
        messages.append(message);
    }
};

class MockConfigurationManager : public IConfigurationManager
{
public:
    mutable QHash<QString, QVariant> settings;
    
    bool getBool(const QString &key, bool defaultValue) const override {
        return settings.value(key, defaultValue).toBool();
    }
    
    int getInt(const QString &key, int defaultValue) const override {
        return settings.value(key, defaultValue).toInt();
    }
    
    QString getString(const QString &key, const QString &defaultValue) const override {
        return settings.value(key, defaultValue).toString();
    }
    
    void setBool(const QString &key, bool value) override {
        settings[key] = value;
    }
    
    void setInt(const QString &key, int value) override {
        settings[key] = value;
    }
    
    void setString(const QString &key, const QString &value) override {
        settings[key] = value;
    }
    
    bool isValidKey(const QString &key) const override {
        Q_UNUSED(key)
        return true; // Always valid for mock
    }
    
    bool validateValue(const QString &key, const QVariant &value) const override {
        Q_UNUSED(key)
        Q_UNUSED(value)
        return true; // Always valid for mock
    }
};

class MockInputValidator : public IInputValidator
{
public:
    mutable bool shouldValidate = true;
    mutable QString lastValidatedPath;
    
    ValidationResult validateFilePath(const QString &path, bool mustExist) override {
        lastValidatedPath = path;
        Q_UNUSED(mustExist)
        return ValidationResult{shouldValidate, shouldValidate ? "" : "Mock validation failed", path};
    }
    
    ValidationResult validateNumber(const QString &value, double min, double max) override {
        Q_UNUSED(min)
        Q_UNUSED(max)
        bool ok;
        double num = value.toDouble(&ok);
        return ValidationResult{ok && shouldValidate, ok ? "" : "Invalid number", QString::number(num)};
    }
    
    ValidationResult validateResolution(const QString &width, const QString &height) override {
        bool widthOk, heightOk;
        int w = width.toInt(&widthOk);
        int h = height.toInt(&heightOk);
        bool valid = widthOk && heightOk && shouldValidate;
        return ValidationResult{valid, valid ? "" : "Invalid resolution", QString("%1x%2").arg(w).arg(h)};
    }
    
    QString sanitizeFilePath(const QString &path) override {
        return path; // No sanitization in mock
    }
    
    QString sanitizeFileName(const QString &fileName) override {
        return fileName; // No sanitization in mock
    }
    
    QStringList sanitizeFFmpegParameters(const QStringList &params) override {
        return params; // No sanitization in mock
    }
};

class TestServiceContainer : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
    
    // Service container tests
    void testSingletonAccess();
    void testServiceRegistration();
    void testServiceResolution();
    void testMockServices();
    void testServiceLifecycle();

private:
    ServiceContainer* m_container = nullptr;
};

void TestServiceContainer::initTestCase()
{
    // Initialize any global test resources
}

void TestServiceContainer::cleanupTestCase()
{
    // Clean up global test resources
}

void TestServiceContainer::init()
{
    // Reset service container before each test
    m_container = &ServiceContainer::instance();
    m_container->clear();
}

void TestServiceContainer::cleanup()
{
    // Clean up after each test
    m_container->clear();
}

void TestServiceContainer::testSingletonAccess()
{
    ServiceContainer& instance1 = ServiceContainer::instance();
    ServiceContainer& instance2 = ServiceContainer::instance();
    
    // Should be the same instance
    QVERIFY(&instance1 == &instance2);
    QVERIFY(&instance1 == m_container);
}

void TestServiceContainer::testServiceRegistration()
{
    // Test registering a service with factory
    m_container->registerFactory<ILogger>([]() -> std::shared_ptr<ILogger> {
        return std::make_shared<MockLogger>();
    });
    
    QVERIFY(m_container->isRegistered<ILogger>());
    QVERIFY(!m_container->isRegistered<IConfigurationManager>());
    
    // Test registering a singleton instance
    auto mockConfig = std::make_shared<MockConfigurationManager>();
    m_container->registerSingleton<IConfigurationManager>(mockConfig);
    
    QVERIFY(m_container->isRegistered<IConfigurationManager>());
}

void TestServiceContainer::testServiceResolution()
{
    // Register mock services
    m_container->registerFactory<ILogger>([]() -> std::shared_ptr<ILogger> {
        return std::make_shared<MockLogger>();
    });
    
    auto mockConfig = std::make_shared<MockConfigurationManager>();
    m_container->registerSingleton<IConfigurationManager>(mockConfig);
    
    // Resolve services
    auto logger = m_container->resolve<ILogger>();
    auto config = m_container->resolve<IConfigurationManager>();
    
    QVERIFY(logger != nullptr);
    QVERIFY(config != nullptr);
    QVERIFY(config.get() == mockConfig.get()); // Should be same instance for singleton
    
    // Test unregistered service returns nullptr
    auto unregistered = m_container->resolve<IInputValidator>();
    QVERIFY(unregistered == nullptr);
}

void TestServiceContainer::testMockServices()
{
    // Register mock logger
    m_container->registerFactory<ILogger>([]() -> std::shared_ptr<ILogger> {
        return std::make_shared<MockLogger>();
    });
    
    // Register mock input validator
    auto mockValidator = std::make_shared<MockInputValidator>();
    m_container->registerSingleton<IInputValidator>(mockValidator);
    
    // Resolve and test
    auto logger = m_container->resolve<ILogger>();
    auto validator = m_container->resolve<IInputValidator>();
    
    QVERIFY(logger != nullptr);
    QVERIFY(validator != nullptr);
    
    // Test mock logger functionality
    logger->info("Test message");
    auto mockLoggerPtr = std::dynamic_pointer_cast<MockLogger>(logger);
    QVERIFY(mockLoggerPtr != nullptr);
    QVERIFY(mockLoggerPtr->messages.contains("Test message"));
    QVERIFY(mockLoggerPtr->lastLevel == "INFO");
    
    // Test mock validator functionality
    auto result = validator->validateFilePath("/test/path", false);
    QVERIFY(result.isValid);
    QVERIFY(mockValidator->lastValidatedPath == "/test/path");
    
    // Test validation failure
    mockValidator->shouldValidate = false;
    result = validator->validateFilePath("/invalid/path", false);
    QVERIFY(!result.isValid);
}

void TestServiceContainer::testServiceLifecycle()
{
    // Test that factory creates new instances each time
    m_container->registerFactory<ILogger>([]() -> std::shared_ptr<ILogger> {
        return std::make_shared<MockLogger>();
    });
    
    auto logger1 = m_container->resolve<ILogger>();
    auto logger2 = m_container->resolve<ILogger>();
    
    QVERIFY(logger1 != nullptr);
    QVERIFY(logger2 != nullptr);
    QVERIFY(logger1.get() != logger2.get()); // Should be different instances
    
    // Test that singleton returns same instance
    auto mockConfig = std::make_shared<MockConfigurationManager>();
    m_container->registerSingleton<IConfigurationManager>(mockConfig);
    
    auto config1 = m_container->resolve<IConfigurationManager>();
    auto config2 = m_container->resolve<IConfigurationManager>();
    
    QVERIFY(config1 != nullptr);
    QVERIFY(config2 != nullptr);
    QVERIFY(config1.get() == config2.get()); // Should be same instance
    QVERIFY(config1.get() == mockConfig.get());
}

QTEST_MAIN(TestServiceContainer)
#include "test_service_container.moc"