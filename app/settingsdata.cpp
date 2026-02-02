/***********************************************************************

                          C I N E   E N C O D E R
                                JULY, 2020
                            COPYRIGHT (C) 2020

 FILE: settingsdata.cpp
 COMMENT: Data model implementation for settings
 LICENSE: GNU General Public License v3.0

***********************************************************************/

#include "settingsdata.h"
#include "constants.h"

using namespace Constants;

SettingsData::SettingsData()
    : m_outputFolder("")  // Initialize to empty string instead of null
    , m_tempFolder("")    // Initialize to empty string instead of null
    , m_protectFlag(false)
    , m_multiInstances(false)
    , m_showHdrFlag(false)
    , m_timerInterval(DEFAULTTIMER)
    , m_threads(0)
    , m_ffmpegPrio(normal)
    , m_theme(0)
    , m_language("en")
    , m_fontSize(10)
    , m_hideInTrayFlag(false)
    , m_prefixType(0)
    , m_prefixName(defaultPrefix())
    , m_suffixType(0)
    , m_suffixName(defaultSuffix())
    , m_subtitlesDeselectAll(false)
    , m_subtitlesBackground(false)
    , m_subtitlesColor(Qt::white)
    , m_subtitlesBackgroundColor(Qt::black)
    , m_subtitlesLocation(0)
    , m_subtitlesFontSize(10)
    , m_deinterlaceEnabled(false)
    , m_deinterlaceFilter(DEINTERLACE_NONE)
    , m_requiresRestart(false)
{
}

SettingsData::~SettingsData() = default;

// Output folder
void SettingsData::setOutputFolder(const QString& folder)
{
    m_outputFolder = folder;
}

QString SettingsData::getOutputFolder() const
{
    return m_outputFolder;
}

// Temp folder
void SettingsData::setTempFolder(const QString& folder)
{
    m_tempFolder = folder;
}

QString SettingsData::getTempFolder() const
{
    return m_tempFolder;
}

// Protect flag
void SettingsData::setProtectFlag(bool flag)
{
    m_protectFlag = flag;
}

bool SettingsData::getProtectFlag() const
{
    return m_protectFlag;
}

// Multi instances
void SettingsData::setMultiInstances(bool flag)
{
    m_multiInstances = flag;
}

bool SettingsData::getMultiInstances() const
{
    return m_multiInstances;
}

// Show HDR flag
void SettingsData::setShowHdrFlag(bool flag)
{
    m_showHdrFlag = flag;
}

bool SettingsData::getShowHdrFlag() const
{
    return m_showHdrFlag;
}

// Timer interval
void SettingsData::setTimerInterval(int interval)
{
    m_timerInterval = interval;
}

int SettingsData::getTimerInterval() const
{
    return m_timerInterval;
}

// Threads
void SettingsData::setThreads(int threads)
{
    m_threads = threads;
}

int SettingsData::getThreads() const
{
    return m_threads;
}

// FFmpeg priority
void SettingsData::setFFMpegPrio(int prio)
{
    if (isValidPriority(prio)) {
        m_ffmpegPrio = prio;
    }
}

int SettingsData::getFFMpegPrio() const
{
    return m_ffmpegPrio;
}

// Theme
void SettingsData::setTheme(int theme)
{
    if (m_theme != theme) {
        m_theme = theme;
        m_requiresRestart = true;
    }
}

int SettingsData::getTheme() const
{
    return m_theme;
}

// Language
void SettingsData::setLanguage(const QString& lang)
{
    if (m_language != lang) {
        m_language = lang;
        m_requiresRestart = true;
    }
}

QString SettingsData::getLanguage() const
{
    return m_language;
}

// Font
void SettingsData::setFont(const QString& font)
{
    m_font = font;
}

QString SettingsData::getFont() const
{
    return m_font;
}

// Font size
void SettingsData::setFontSize(int size)
{
    m_fontSize = size;
}

int SettingsData::getFontSize() const
{
    return m_fontSize;
}

// Hide in tray flag
void SettingsData::setHideInTrayFlag(bool flag)
{
    m_hideInTrayFlag = flag;
}

bool SettingsData::getHideInTrayFlag() const
{
    return m_hideInTrayFlag;
}

// Prefix type
void SettingsData::setPrefixType(int type)
{
    m_prefixType = type;
}

int SettingsData::getPrefixType() const
{
    return m_prefixType;
}

// Prefix name
void SettingsData::setPrefixName(const QString& name)
{
    m_prefixName = name;
}

QString SettingsData::getPrefixName() const
{
    return m_prefixName;
}

// Suffix type
void SettingsData::setSuffixType(int type)
{
    m_suffixType = type;
}

int SettingsData::getSuffixType() const
{
    return m_suffixType;
}

// Suffix name
void SettingsData::setSuffixName(const QString& name)
{
    m_suffixName = name;
}

QString SettingsData::getSuffixName() const
{
    return m_suffixName;
}

// Subtitles deselect all
void SettingsData::setSubtitlesDeselectAll(bool flag)
{
    m_subtitlesDeselectAll = flag;
}

bool SettingsData::getSubtitlesDeselectAll() const
{
    return m_subtitlesDeselectAll;
}

// Subtitles background
void SettingsData::setSubtitlesBackground(bool flag)
{
    m_subtitlesBackground = flag;
}

bool SettingsData::getSubtitlesBackground() const
{
    return m_subtitlesBackground;
}

// Subtitles color
void SettingsData::setSubtitlesColor(const QColor& color)
{
    m_subtitlesColor = color;
}

QColor SettingsData::getSubtitlesColor() const
{
    return m_subtitlesColor;
}

// Subtitles background color
void SettingsData::setSubtitlesBackgroundColor(const QColor& color)
{
    m_subtitlesBackgroundColor = color;
}

QColor SettingsData::getSubtitlesBackgroundColor() const
{
    return m_subtitlesBackgroundColor;
}

// Subtitles location
void SettingsData::setSubtitlesLocation(int location)
{
    m_subtitlesLocation = location;
}

int SettingsData::getSubtitlesLocation() const
{
    return m_subtitlesLocation;
}

// Subtitles font
void SettingsData::setSubtitlesFont(const QString& font)
{
    m_subtitlesFont = font;
}

QString SettingsData::getSubtitlesFont() const
{
    return m_subtitlesFont;
}

// Subtitles font size
void SettingsData::setSubtitlesFontSize(int size)
{
    m_subtitlesFontSize = size;
}

int SettingsData::getSubtitlesFontSize() const
{
    return m_subtitlesFontSize;
}

// Deinterlace enabled
void SettingsData::setDeinterlaceEnabled(bool flag)
{
    m_deinterlaceEnabled = flag;
}

bool SettingsData::getDeinterlaceEnabled() const
{
    return m_deinterlaceEnabled;
}

// Deinterlace filter
void SettingsData::setDeinterlaceFilter(int filter)
{
    m_deinterlaceFilter = filter;
}

int SettingsData::getDeinterlaceFilter() const
{
    return m_deinterlaceFilter;
}

// Requires restart
bool SettingsData::requiresRestart() const
{
    return m_requiresRestart;
}

void SettingsData::setRequiresRestart(bool restart)
{
    m_requiresRestart = restart;
}

// Reset to defaults
void SettingsData::resetToDefaults()
{
    m_outputFolder.clear();
    m_tempFolder.clear();
    m_protectFlag = false;
    m_multiInstances = false;
    m_showHdrFlag = false;
    m_timerInterval = DEFAULTTIMER;
    m_threads = 0;
    m_ffmpegPrio = normal;
    m_theme = 0;
    m_language = "en";
    m_fontSize = 8;
    m_hideInTrayFlag = false;
    m_prefixType = 0;
    m_prefixName = defaultPrefix();
    m_suffixType = 0;
    m_suffixName = defaultSuffix();
    m_subtitlesDeselectAll = false;
    m_subtitlesBackground = false;
    m_subtitlesColor = Qt::white;
    m_subtitlesBackgroundColor = Qt::black;
    m_subtitlesLocation = 0;
    m_subtitlesFontSize = 8;
    m_deinterlaceEnabled = false;
    m_deinterlaceFilter = DEINTERLACE_NONE;
    m_requiresRestart = true;
}

// Validation methods
bool SettingsData::isValidOutputFolder(const QString& folder) const
{
    // Add validation logic here if needed
    return !folder.isEmpty();
}

bool SettingsData::isValidTempFolder(const QString& folder) const
{
    // Add validation logic here if needed  
    return !folder.isEmpty();
}

bool SettingsData::isValidPriority(int prio) const
{
    return prio >= lowest && prio <= highest;
}