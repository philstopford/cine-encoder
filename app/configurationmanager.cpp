/***********************************************************************

                          C I N E   E N C O D E R
                                JULY, 2020
                            COPYRIGHT (C) 2020

 FILE: configurationmanager.cpp
 COMMENT: Centralized configuration management implementation
 LICENSE: GNU General Public License v3.0

***********************************************************************/

#include "configurationmanager.h"
#include "logger.h"
#include "constants.h"
#include "helper.h"
#include <QApplication>
#include <QStandardPaths>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

using namespace Constants;

ConfigurationManager::ConfigurationManager(QObject *parent) : QObject(parent)
{
    // Initialize settings with application-specific location
    const QString configPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QDir().mkpath(configPath);
    
    m_settings = std::make_unique<QSettings>(configPath + "/cine-encoder.conf", QSettings::IniFormat);
    
    initializeKeys();
    loadDefaults();
    
    LOG_INFO(QString("Configuration manager initialized with path: %1").arg(m_settings->fileName()));
}

ConfigurationManager::~ConfigurationManager() = default;

ConfigurationManager& ConfigurationManager::instance()
{
    static ConfigurationManager manager;
    return manager;
}

void ConfigurationManager::initializeKeys()
{
    // Application settings
    registerKey("app/language", "en", ValueType::String, Category::Application, 
                "Application language");
    registerKey("app/theme", 0, ValueType::Int, Category::Application, 
                "Application theme index", 0, 10);
    registerKey("app/hideInTray", false, ValueType::Bool, Category::Application, 
                "Hide application in system tray when minimized");
    registerKey("app/multipleInstances", false, ValueType::Bool, Category::Application, 
                "Allow multiple application instances");
    registerKey("app/checkUpdates", true, ValueType::Bool, Category::Application,
                "Check for updates automatically");

    // Interface settings
    registerKey("ui/fontSize", 10, ValueType::Int, Category::Interface, 
                "Interface font size", 8, 16);
    registerKey("ui/font", "Arial", ValueType::String, Category::Interface,
                "Interface font family");
    registerKey("ui/windowGeometry", QSize(1500, 920), ValueType::Size, Category::Interface,
                "Main window size");
    registerKey("ui/windowPosition", QPoint(100, 100), ValueType::Point, Category::Interface,
                "Main window position");
    registerKey("ui/expandedState", false, ValueType::Bool, Category::Interface,
                "Window expanded state");
    registerKey("ui/rowHeight", 45, ValueType::Int, Category::Interface,
                "Table row height", 20, 100);
    registerKey("ui/showHDR", false, ValueType::Bool, Category::Interface,
                "Show HDR information in interface");

    // Encoding settings  
    registerKey("encoding/threads", 0, ValueType::Int, Category::Encoding,
                "Number of encoding threads (0 = auto)", 0, 64);
    registerKey("encoding/priority", 0, ValueType::Int, Category::Encoding,
                "Encoding process priority", -2, 2);
    registerKey("encoding/timerInterval", DEFAULTTIMER, ValueType::Int, Category::Encoding,
                "Timer update interval in milliseconds", 100, 5000);
    registerKey("encoding/protection", false, ValueType::Bool, Category::Encoding,
                "Enable encoding protection");

    // File settings
    registerKey("files/outputFolder", QDir::homePath(), ValueType::String, Category::Files,
                "Default output folder path");
    registerKey("files/tempFolder", QStandardPaths::writableLocation(QStandardPaths::TempLocation),
                ValueType::String, Category::Files, "Temporary files folder");
    registerKey("files/prefixType", 0, ValueType::Int, Category::Files,
                "Output filename prefix type", 0, 3);
    registerKey("files/suffixType", 0, ValueType::Int, Category::Files, 
                "Output filename suffix type", 0, 3);
    registerKey("files/prefixName", QString(), ValueType::String, Category::Files,
                "Custom filename prefix");
    registerKey("files/suffixName", QString(), ValueType::String, Category::Files,
                "Custom filename suffix");

    // Subtitle settings
    registerKey("subtitles/font", "Arial", ValueType::String, Category::Interface,
                "Subtitle font family");
    registerKey("subtitles/fontSize", 16, ValueType::Int, Category::Interface,
                "Subtitle font size", 8, 48);
    registerKey("subtitles/color", QColor(Qt::white), ValueType::Color, Category::Interface,
                "Subtitle text color");
    registerKey("subtitles/background", false, ValueType::Bool, Category::Interface,
                "Enable subtitle background");
    registerKey("subtitles/backgroundColor", QColor(Qt::black), ValueType::Color, Category::Interface,
                "Subtitle background color");
    registerKey("subtitles/backgroundAlpha", 128, ValueType::Int, Category::Interface,
                "Subtitle background transparency", 0, 255);
    registerKey("subtitles/location", 0, ValueType::Int, Category::Interface,
                "Subtitle location on screen", 0, 8);
    registerKey("subtitles/deselectAll", false, ValueType::Bool, Category::Interface,
                "Deselect all subtitles by default");

    LOG_DEBUG(QString("Registered %1 configuration keys").arg(m_keys.size()));
}

void ConfigurationManager::registerKey(const QString &key, const QVariant &defaultValue,
                                      ValueType type, Category category, const QString &description,
                                      const QVariant &minValue, const QVariant &maxValue)
{
    ConfigKey configKey;
    configKey.key = key;
    configKey.defaultValue = defaultValue;
    configKey.type = type;
    configKey.category = category;
    configKey.description = description;
    configKey.minValue = minValue;
    configKey.maxValue = maxValue;
    
    m_keys[key] = configKey;
}

void ConfigurationManager::loadDefaults()
{
    int loadedCount = 0;
    for (auto it = m_keys.constBegin(); it != m_keys.constEnd(); ++it) {
        if (!m_settings->contains(it.key())) {
            m_settings->setValue(it.key(), it.value().defaultValue);
            loadedCount++;
        }
    }
    
    if (loadedCount > 0) {
        LOG_INFO(QString("Loaded %1 default configuration values").arg(loadedCount));
    }
}

QVariant ConfigurationManager::getValue(const QString &key) const
{
    if (m_keys.contains(key)) {
        return m_settings->value(key, m_keys[key].defaultValue);
    } else {
        LOG_WARNING(QString("Unknown configuration key: %1").arg(key));
        return m_settings->value(key);
    }
}

void ConfigurationManager::setValue(const QString &key, const QVariant &value)
{
    if (validateValue(key, value)) {
        const QVariant oldValue = getValue(key);
        m_settings->setValue(key, value);
        
        if (oldValue != value) {
            emit configurationChanged(key, value);
            LOG_DEBUG(QString("Configuration changed: %1 = %2").arg(key, value.toString()));
        }
    } else {
        LOG_WARNING(QString("Invalid configuration value for %1: %2").arg(key, value.toString()));
    }
}

bool ConfigurationManager::getBool(const QString &key, bool defaultValue) const
{
    return getValue(key, defaultValue).toBool();
}

int ConfigurationManager::getInt(const QString &key, int defaultValue) const
{
    return getValue(key, defaultValue).toInt();
}

double ConfigurationManager::getDouble(const QString &key, double defaultValue) const
{
    return getValue(key, defaultValue).toDouble();
}

QString ConfigurationManager::getString(const QString &key, const QString &defaultValue) const
{
    return getValue(key, defaultValue).toString();
}

QStringList ConfigurationManager::getStringList(const QString &key, const QStringList &defaultValue) const
{
    return getValue(key, defaultValue).toStringList();
}

QSize ConfigurationManager::getSize(const QString &key, const QSize &defaultValue) const
{
    return getValue(key, defaultValue).toSize();
}

QPoint ConfigurationManager::getPoint(const QString &key, const QPoint &defaultValue) const
{
    return getValue(key, defaultValue).toPoint();
}

QColor ConfigurationManager::getColor(const QString &key, const QColor &defaultValue) const
{
    return getValue(key, defaultValue).value<QColor>();
}

QFont ConfigurationManager::getFont(const QString &key, const QFont &defaultValue) const
{
    return getValue(key, defaultValue).value<QFont>();
}

void ConfigurationManager::setBool(const QString &key, bool value)
{
    setValue(key, QVariant(value));
}

void ConfigurationManager::setInt(const QString &key, int value)
{
    setValue(key, QVariant(value));
}

void ConfigurationManager::setDouble(const QString &key, double value)
{
    setValue(key, QVariant(value));
}

void ConfigurationManager::setString(const QString &key, const QString &value)
{
    setValue(key, QVariant(value));
}

void ConfigurationManager::setStringList(const QString &key, const QStringList &value)
{
    setValue(key, QVariant(value));
}

void ConfigurationManager::setSize(const QString &key, const QSize &value)
{
    setValue(key, QVariant(value));
}

void ConfigurationManager::setPoint(const QString &key, const QPoint &value)
{
    setValue(key, QVariant(value));
}

void ConfigurationManager::setColor(const QString &key, const QColor &value)
{
    setValue(key, QVariant(value));
}

void ConfigurationManager::setFont(const QString &key, const QFont &value)
{
    setValue(key, QVariant(value));
}

void ConfigurationManager::resetToDefaults()
{
    LOG_INFO("Resetting all configuration to defaults");
    
    for (auto it = m_keys.constBegin(); it != m_keys.constEnd(); ++it) {
        m_settings->setValue(it.key(), it.value().defaultValue);
    }
    
    emit configurationReset();
}

void ConfigurationManager::resetCategory(Category category)
{
    LOG_INFO(QString("Resetting configuration category: %1").arg(static_cast<int>(category)));
    
    for (auto it = m_keys.constBegin(); it != m_keys.constEnd(); ++it) {
        if (it.value().category == category) {
            m_settings->setValue(it.key(), it.value().defaultValue);
        }
    }
    
    emit configurationReset();
}

bool ConfigurationManager::isValidKey(const QString &key) const
{
    return m_keys.contains(key);
}

bool ConfigurationManager::validateValue(const QString &key, const QVariant &value) const
{
    m_validationErrors.clear();
    
    if (!m_keys.contains(key)) {
        m_validationErrors.append(QString("Unknown key: %1").arg(key));
        return false;
    }
    
    const ConfigKey &configKey = m_keys[key];
    
    // Check type compatibility
    if (!value.canConvert(configKey.defaultValue.type())) {
        m_validationErrors.append(QString("Type mismatch for key %1").arg(key));
        return false;
    }
    
    // Check range for numeric types
    if (configKey.minValue.isValid() && configKey.maxValue.isValid()) {
        if (configKey.type == ValueType::Int) {
            const int intValue = value.toInt();
            const int minValue = configKey.minValue.toInt();
            const int maxValue = configKey.maxValue.toInt();
            if (intValue < minValue || intValue > maxValue) {
                m_validationErrors.append(QString("Value out of range for key %1: %2 (range: %3-%4)")
                                         .arg(key).arg(intValue).arg(minValue).arg(maxValue));
                return false;
            }
        } else if (configKey.type == ValueType::Double) {
            const double doubleValue = value.toDouble();
            const double minValue = configKey.minValue.toDouble();
            const double maxValue = configKey.maxValue.toDouble();
            if (doubleValue < minValue || doubleValue > maxValue) {
                m_validationErrors.append(QString("Value out of range for key %1: %2 (range: %3-%4)")
                                         .arg(key).arg(doubleValue).arg(minValue).arg(maxValue));
                return false;
            }
        }
    }
    
    return true;
}

QStringList ConfigurationManager::getValidationErrors() const
{
    return m_validationErrors;
}

QStringList ConfigurationManager::getAllKeys() const
{
    return m_keys.keys();
}

QStringList ConfigurationManager::getKeysInCategory(Category category) const
{
    QStringList keys;
    for (auto it = m_keys.constBegin(); it != m_keys.constEnd(); ++it) {
        if (it.value().category == category) {
            keys.append(it.key());
        }
    }
    return keys;
}

QString ConfigurationManager::getKeyDescription(const QString &key) const
{
    return m_keys.contains(key) ? m_keys[key].description : QString();
}

ConfigurationManager::Category ConfigurationManager::getKeyCategory(const QString &key) const
{
    return m_keys.contains(key) ? m_keys[key].category : Category::Application;
}

// Static constants
const QString& ConfigurationManager::getApplicationName()
{
    static const QString name = "Cine Encoder";
    return name;
}

const QString& ConfigurationManager::getApplicationVersion()
{
    static const QString version = "2.0.0"; // Update as needed
    return version;
}

const QString& ConfigurationManager::getConfigurationPath()
{
    static const QString path = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    return path;
}

const QString& ConfigurationManager::getLogPath()
{
    static const QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/logs";
    return path;
}

const QString& ConfigurationManager::getThumbnailPath()
{
    static const QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/thumbnails";
    return path;
}

const QString& ConfigurationManager::getPresetPath()
{
    static const QString path = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/presets.xml";
    return path;
}

const QStringList& ConfigurationManager::getSupportedVideoFormats()
{
    static const QStringList formats = {
        "mp4", "avi", "mkv", "mov", "wmv", "flv", "webm", "m4v", "mpg", "mpeg",
        "3gp", "asf", "rm", "rmvb", "ts", "m2ts", "mts", "vob", "ogv", "divx"
    };
    return formats;
}

const QStringList& ConfigurationManager::getSupportedAudioFormats()
{
    static const QStringList formats = {
        "mp3", "aac", "ogg", "wav", "flac", "m4a", "wma", "ac3", "dts", "opus",
        "amr", "aiff", "au", "ra", "ape", "tak"
    };
    return formats;
}

const QStringList& ConfigurationManager::getSupportedSubtitleFormats()
{
    static const QStringList formats = {
        "srt", "ass", "ssa", "sub", "idx", "vtt", "sup", "pgs", "txt"
    };
    return formats;
}

const QStringList& ConfigurationManager::getAvailableLanguages()
{
    static const QStringList languages = {
        "en", "es", "fr", "de", "it", "pt", "ru", "zh", "ja", "ko"
    };
    return languages;
}

double ConfigurationManager::getScalingFactor()
{
    return Helper::scaling();
}

QSize ConfigurationManager::getDefaultWindowSize()
{
    return QSize(1500, 920) * Helper::scaling();
}

int ConfigurationManager::getDefaultRowHeight()
{
    return static_cast<int>(45 * Helper::scaling());
}

int ConfigurationManager::getDefaultFontSize()
{
    return 10;
}