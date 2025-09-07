/***********************************************************************

                          C I N E   E N C O D E R
                                JULY, 2020
                            COPYRIGHT (C) 2020

 FILE: configurationmanager.h
 COMMENT: Centralized configuration and constants management
 LICENSE: GNU General Public License v3.0

***********************************************************************/

#ifndef CONFIGURATIONMANAGER_H
#define CONFIGURATIONMANAGER_H

#include <QObject>
#include <QSettings>
#include <QVariant>
#include <QStringList>
#include <QSize>
#include <QPoint>
#include <QColor>
#include <QFont>
#include <memory>

class ConfigurationManager : public QObject
{
    Q_OBJECT

public:
    // Configuration categories
    enum class Category {
        Application,
        Encoding,
        Interface,
        Files,
        Advanced
    };

    // Data types for strongly typed access
    enum class ValueType {
        Bool,
        Int,
        Double,
        String,
        StringList,
        Size,
        Point,
        Color,
        Font
    };

    struct ConfigKey {
        QString key;
        QVariant defaultValue;
        ValueType type;
        Category category;
        QString description;
        QVariant minValue = QVariant();
        QVariant maxValue = QVariant();
    };

    static ConfigurationManager& instance();

    // Configuration access
    template<typename T>
    T getValue(const QString &key, const T &defaultValue = T()) const;
    
    template<typename T>
    void setValue(const QString &key, const T &value);
    
    QVariant getValue(const QString &key) const;
    void setValue(const QString &key, const QVariant &value);
    
    // Typed getters for common types
    bool getBool(const QString &key, bool defaultValue = false) const;
    int getInt(const QString &key, int defaultValue = 0) const;
    double getDouble(const QString &key, double defaultValue = 0.0) const;
    QString getString(const QString &key, const QString &defaultValue = QString()) const;
    QStringList getStringList(const QString &key, const QStringList &defaultValue = QStringList()) const;
    QSize getSize(const QString &key, const QSize &defaultValue = QSize()) const;
    QPoint getPoint(const QString &key, const QPoint &defaultValue = QPoint()) const;
    QColor getColor(const QString &key, const QColor &defaultValue = QColor()) const;
    QFont getFont(const QString &key, const QFont &defaultValue = QFont()) const;
    
    // Typed setters
    void setBool(const QString &key, bool value);
    void setInt(const QString &key, int value);
    void setDouble(const QString &key, double value);
    void setString(const QString &key, const QString &value);
    void setStringList(const QString &key, const QStringList &value);
    void setSize(const QString &key, const QSize &value);
    void setPoint(const QString &key, const QPoint &value);
    void setColor(const QString &key, const QColor &value);
    void setFont(const QString &key, const QFont &value);
    
    // Configuration management
    void loadDefaults();
    void resetToDefaults();
    void resetCategory(Category category);
    void exportConfiguration(const QString &filePath) const;
    bool importConfiguration(const QString &filePath);
    
    // Validation
    bool isValidKey(const QString &key) const;
    bool validateValue(const QString &key, const QVariant &value) const;
    QStringList getValidationErrors() const;
    
    // Key discovery
    QStringList getAllKeys() const;
    QStringList getKeysInCategory(Category category) const;
    QString getKeyDescription(const QString &key) const;
    Category getKeyCategory(const QString &key) const;
    
    // Constants access (predefined values)
    static const QString& getApplicationName();
    static const QString& getApplicationVersion();
    static const QString& getConfigurationPath();
    static const QString& getLogPath();
    static const QString& getThumbnailPath();
    static const QString& getPresetPath();
    
    // Application constants
    static const QStringList& getSupportedVideoFormats();
    static const QStringList& getSupportedAudioFormats();
    static const QStringList& getSupportedSubtitleFormats();
    static const QStringList& getAvailableLanguages();
    
    // UI constants
    static double getScalingFactor();
    static QSize getDefaultWindowSize();
    static int getDefaultRowHeight();
    static int getDefaultFontSize();

signals:
    void configurationChanged(const QString &key, const QVariant &value);
    void configurationReset();

private:
    explicit ConfigurationManager(QObject *parent = nullptr);
    ~ConfigurationManager() override;

    void initializeKeys();
    void registerKey(const QString &key, const QVariant &defaultValue, 
                    ValueType type, Category category, const QString &description,
                    const QVariant &minValue = QVariant(), const QVariant &maxValue = QVariant());

    std::unique_ptr<QSettings> m_settings;
    QHash<QString, ConfigKey> m_keys;
    mutable QStringList m_validationErrors;
};

// Convenience macros for configuration access
#define CONFIG ConfigurationManager::instance()
#define GET_CONFIG_BOOL(key, def) ConfigurationManager::instance().getBool(key, def)
#define GET_CONFIG_INT(key, def) ConfigurationManager::instance().getInt(key, def)
#define GET_CONFIG_STRING(key, def) ConfigurationManager::instance().getString(key, def)
#define SET_CONFIG_BOOL(key, val) ConfigurationManager::instance().setBool(key, val)
#define SET_CONFIG_INT(key, val) ConfigurationManager::instance().setInt(key, val)
#define SET_CONFIG_STRING(key, val) ConfigurationManager::instance().setString(key, val)

// Template implementation
template<typename T>
T ConfigurationManager::getValue(const QString &key, const T &defaultValue) const
{
    const QVariant value = m_settings->value(key, QVariant::fromValue(defaultValue));
    return value.value<T>();
}

template<typename T>
void ConfigurationManager::setValue(const QString &key, const T &value)
{
    const QVariant variant = QVariant::fromValue(value);
    if (validateValue(key, variant)) {
        m_settings->setValue(key, variant);
        emit configurationChanged(key, variant);
    }
}

#endif // CONFIGURATIONMANAGER_H