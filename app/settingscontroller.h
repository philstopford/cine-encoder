/***********************************************************************

                          C I N E   E N C O D E R
                                JULY, 2020
                            COPYRIGHT (C) 2020

 FILE: settingscontroller.h
 COMMENT: Controller for settings, mediating between UI and data model
 LICENSE: GNU General Public License v3.0

***********************************************************************/

#ifndef SETTINGSCONTROLLER_H
#define SETTINGSCONTROLLER_H

#include <QObject>
#include <QColor>
#include "settingsdata.h"
#include "configurationmanager.h"
#include "inputvalidator.h"
#include "errorhandler.h"

namespace Ui {
    class Settings;
}

/**
 * @brief Controller class for managing settings interactions
 * 
 * This class mediates between the UI and the data model,
 * handling business logic and keeping the UI focused on presentation.
 */
class SettingsController : public QObject
{
    Q_OBJECT

public:
    explicit SettingsController(QObject *parent = nullptr);
    ~SettingsController();

    // Initialize with UI and external data pointers
    void initialize(Ui::Settings* ui, 
                   QString* pOutputFolder, QString* pTempFolder,
                   bool* pProtectFlag, bool* pMultiInstances,
                   bool* pShowHdrFlag, int* pTimerInterval,
                   int* pThreads, int* pFFMpegPrio, int* pTheme,
                   QString* pPrefixName, QString* pSuffixName,
                   int* pPrefxType, int* pSuffixType,
                   QString* pLanguage, bool* pHideInTrayFlag,
                   QString* pFont, int* pFontSize,
                   bool* pSubtitlesDeselectAll, bool* pSubtitlesBackground,
                   QColor* pSubtitlesColor, QColor* pSubtitlesBackgroundColor,
                   int* pSubtitlesLocation, QString* pSubtitlesFont,
                   int* pSubtitlesFontSize,
                   bool* pDeinterlaceEnabled, int* pDeinterlaceFilter);

    // Load data from UI to model
    void loadFromUI();
    
    // Update UI from model
    void updateUI();

    // Apply settings - business logic
    void applySettings();
    
    // Reset settings to defaults
    void resetSettings();

    // Validation
    bool validateSettings() const;

    // Check if restart is needed
    bool requiresRestart() const;

    // Get data model (for read-only access)
    const SettingsData& getData() const;

private:
    void syncToExternalPointers();
    void syncFromExternalPointers();
    void setupFontComboBoxes();
    void updateFontSizes();
    void updateThemeAndLanguage();
    void updateSubtitleSettings();

    SettingsData m_data;
    Ui::Settings* m_ui;
    
    // External data pointers (for backward compatibility)
    QString* m_pOutputFolder;
    QString* m_pTempFolder;
    bool* m_pProtectFlag;
    bool* m_pMultiInstances;
    bool* m_pShowHdrFlag;
    int* m_pTimerInterval;
    int* m_pThreads;
    int* m_pFFMpegPrio;
    int* m_pTheme;
    QString* m_pPrefixName;
    QString* m_pSuffixName;
    int* m_pPrefxType;
    int* m_pSuffixType;
    QString* m_pLanguage;
    bool* m_pHideInTrayFlag;
    QString* m_pFont;
    int* m_pFontSize;
    bool* m_pSubtitlesDeselectAll;
    bool* m_pSubtitlesBackground;
    QColor* m_pSubtitlesColor;
    QColor* m_pSubtitlesBackgroundColor;
    int* m_pSubtitlesLocation;
    QString* m_pSubtitlesFont;
    int* m_pSubtitlesFontSize;
    bool* m_pDeinterlaceEnabled;
    int* m_pDeinterlaceFilter;

    // Temporary colors for UI feedback
    QColor m_subtitlesColorTemp;
    QColor m_subtitlesBackgroundColorTemp;
};

#endif // SETTINGSCONTROLLER_H