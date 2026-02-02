/***********************************************************************

                          C I N E   E N C O D E R
                                JULY, 2020
                            COPYRIGHT (C) 2020

 FILE: settingsdata.h
 COMMENT: Data model for settings, separating business logic from UI
 LICENSE: GNU General Public License v3.0

***********************************************************************/

#ifndef SETTINGSDATA_H
#define SETTINGSDATA_H

#include <QString>
#include <QColor>

/**
 * @brief Data model class for managing settings data
 * 
 * This class encapsulates all settings data and provides
 * methods to manipulate it independently of the UI.
 */
class SettingsData
{
public:
    SettingsData();
    ~SettingsData();

    // Data access methods
    void setOutputFolder(const QString& folder);
    QString getOutputFolder() const;

    void setTempFolder(const QString& folder);
    QString getTempFolder() const;

    void setProtectFlag(bool flag);
    bool getProtectFlag() const;

    void setMultiInstances(bool flag);
    bool getMultiInstances() const;

    void setShowHdrFlag(bool flag);
    bool getShowHdrFlag() const;

    void setTimerInterval(int interval);
    int getTimerInterval() const;

    void setThreads(int threads);
    int getThreads() const;

    void setFFMpegPrio(int prio);
    int getFFMpegPrio() const;

    void setTheme(int theme);
    int getTheme() const;

    void setLanguage(const QString& lang);
    QString getLanguage() const;

    void setFont(const QString& font);
    QString getFont() const;

    void setFontSize(int size);
    int getFontSize() const;

    void setHideInTrayFlag(bool flag);
    bool getHideInTrayFlag() const;

    void setPrefixType(int type);
    int getPrefixType() const;

    void setPrefixName(const QString& name);
    QString getPrefixName() const;

    void setSuffixType(int type);
    int getSuffixType() const;

    void setSuffixName(const QString& name);
    QString getSuffixName() const;

    void setSubtitlesDeselectAll(bool flag);
    bool getSubtitlesDeselectAll() const;

    void setSubtitlesBackground(bool flag);
    bool getSubtitlesBackground() const;

    void setSubtitlesColor(const QColor& color);
    QColor getSubtitlesColor() const;

    void setSubtitlesBackgroundColor(const QColor& color);
    QColor getSubtitlesBackgroundColor() const;

    void setSubtitlesLocation(int location);
    int getSubtitlesLocation() const;

    void setSubtitlesFont(const QString& font);
    QString getSubtitlesFont() const;

    void setSubtitlesFontSize(int size);
    int getSubtitlesFontSize() const;

    void setDeinterlaceEnabled(bool flag);
    bool getDeinterlaceEnabled() const;

    void setDeinterlaceFilter(int filter);
    int getDeinterlaceFilter() const;

    // Business logic methods
    bool requiresRestart() const;
    void setRequiresRestart(bool restart);

    void resetToDefaults();

    // Data validation
    bool isValidOutputFolder(const QString& folder) const;
    bool isValidTempFolder(const QString& folder) const;
    bool isValidPriority(int prio) const;

private:
    QString m_outputFolder;
    QString m_tempFolder;
    bool m_protectFlag;
    bool m_multiInstances;
    bool m_showHdrFlag;
    int m_timerInterval;
    int m_threads;
    int m_ffmpegPrio;
    int m_theme;
    QString m_language;
    QString m_font;
    int m_fontSize;
    bool m_hideInTrayFlag;
    int m_prefixType;
    QString m_prefixName;
    int m_suffixType;
    QString m_suffixName;
    bool m_subtitlesDeselectAll;
    bool m_subtitlesBackground;
    QColor m_subtitlesColor;
    QColor m_subtitlesBackgroundColor;
    int m_subtitlesLocation;
    QString m_subtitlesFont;
    int m_subtitlesFontSize;
    bool m_deinterlaceEnabled;
    int m_deinterlaceFilter;
    bool m_requiresRestart;
};

#endif // SETTINGSDATA_H