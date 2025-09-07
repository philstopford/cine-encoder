/***********************************************************************

                          C I N E   E N C O D E R
                                JULY, 2020
                            COPYRIGHT (C) 2020

 FILE: settingscontroller.cpp
 COMMENT: Controller implementation for settings
 LICENSE: GNU General Public License v3.0

***********************************************************************/

#include "settingscontroller.h"
#include "ui_settings.h"
#include "constants.h"
#include <QFontDatabase>
#include <QStringListModel>
#include <QListView>
#include <QRegularExpressionValidator>
#include <QApplication>

using namespace Constants;

SettingsController::SettingsController(QObject *parent)
    : QObject(parent)
    , m_ui(nullptr)
    , m_pOutputFolder(nullptr)
    , m_pTempFolder(nullptr)
    , m_pProtectFlag(nullptr)
    , m_pMultiInstances(nullptr)
    , m_pShowHdrFlag(nullptr)
    , m_pTimerInterval(nullptr)
    , m_pThreads(nullptr)
    , m_pFFMpegPrio(nullptr)
    , m_pTheme(nullptr)
    , m_pPrefixName(nullptr)
    , m_pSuffixName(nullptr)
    , m_pPrefxType(nullptr)
    , m_pSuffixType(nullptr)
    , m_pLanguage(nullptr)
    , m_pHideInTrayFlag(nullptr)
    , m_pFont(nullptr)
    , m_pFontSize(nullptr)
    , m_pSubtitlesDeselectAll(nullptr)
    , m_pSubtitlesBackground(nullptr)
    , m_pSubtitlesColor(nullptr)
    , m_pSubtitlesBackgroundColor(nullptr)
    , m_pSubtitlesLocation(nullptr)
    , m_pSubtitlesFont(nullptr)
    , m_pSubtitlesFontSize(nullptr)
{
}

SettingsController::~SettingsController() = default;

void SettingsController::initialize(Ui::Settings* ui,
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
                                  int* pSubtitlesFontSize)
{
    m_ui = ui;
    m_pOutputFolder = pOutputFolder;
    m_pTempFolder = pTempFolder;
    m_pProtectFlag = pProtectFlag;
    m_pMultiInstances = pMultiInstances;
    m_pShowHdrFlag = pShowHdrFlag;
    m_pTimerInterval = pTimerInterval;
    m_pThreads = pThreads;
    m_pFFMpegPrio = pFFMpegPrio;
    m_pTheme = pTheme;
    m_pPrefixName = pPrefixName;
    m_pSuffixName = pSuffixName;
    m_pPrefxType = pPrefxType;
    m_pSuffixType = pSuffixType;
    m_pLanguage = pLanguage;
    m_pHideInTrayFlag = pHideInTrayFlag;
    m_pFont = pFont;
    m_pFontSize = pFontSize;
    m_pSubtitlesDeselectAll = pSubtitlesDeselectAll;
    m_pSubtitlesBackground = pSubtitlesBackground;
    m_pSubtitlesColor = pSubtitlesColor;
    m_pSubtitlesBackgroundColor = pSubtitlesBackgroundColor;
    m_pSubtitlesLocation = pSubtitlesLocation;
    m_pSubtitlesFont = pSubtitlesFont;
    m_pSubtitlesFontSize = pSubtitlesFontSize;

    // Load current values from external pointers to data model
    syncFromExternalPointers();

    // Setup UI components that don't change
    setupFontComboBoxes();
}

void SettingsController::loadFromUI()
{
    if (!m_ui) return;

    // Load font settings
    m_data.setFont(m_ui->comboBox_font->currentText());
    m_data.setSubtitlesFont(m_ui->comboBox_subtitles_font->currentText());

    // Load font sizes
    const int fontSizeIndex = m_ui->comboBox_fontsize->currentIndex();
    const int subtitlesFontSizeIndex = m_ui->comboBox_subtitles_fontsize->currentIndex();
    const int fontSizes[6] = {8, 9, 10, 11, 12, 13};
    m_data.setFontSize(fontSizes[fontSizeIndex]);
    m_data.setSubtitlesFontSize(fontSizes[subtitlesFontSizeIndex]);

    // Load theme and language (these may require restart)
    m_data.setTheme(m_ui->comboBox_theme->currentIndex());
    
    const int langIndex = m_ui->comboBox_lang->currentIndex();
    const QString languages[4] = {"en", "zh", "de", "ru"};
    m_data.setLanguage(languages[langIndex]);

    // Load priority
    m_data.setFFMpegPrio(m_ui->comboBox_priority->currentIndex());

    // Load paths
    m_data.setTempFolder(m_ui->lineEdit_tempPath->text());
    m_data.setOutputFolder(m_ui->lineEdit_outPath->text());

    // Load checkbox states
    m_data.setHideInTrayFlag(m_ui->checkBox_tray->checkState() == 2);
    m_data.setShowHdrFlag(m_ui->checkBox_showHDR->checkState() == 2);
    m_data.setProtectFlag(m_ui->checkBox_protection->checkState() == 2);
    m_data.setMultiInstances(m_ui->checkBox_allowDuplicates->checkState() == 2);
    m_data.setSubtitlesDeselectAll(m_ui->checkBox_subtitles_deselectall->checkState() == 2);
    m_data.setSubtitlesBackground(m_ui->checkBox_subtitles_background->checkState() == 2);

    // Load spinbox values
    m_data.setThreads(m_ui->spinBox_threads->value());
    m_data.setTimerInterval(m_ui->spinBox_protectionTimer->value());

    // Load subtitle settings
    m_data.setSubtitlesColor(m_subtitlesColorTemp);
    m_data.setSubtitlesBackgroundColor(QColor(m_subtitlesBackgroundColorTemp.red(),
                                             m_subtitlesBackgroundColorTemp.green(),
                                             m_subtitlesBackgroundColorTemp.blue(),
                                             m_ui->spinBox_background->value()));
    m_data.setSubtitlesLocation(m_ui->comboBox_subtitles_location->currentIndex());

    // Load prefix/suffix settings
    m_data.setPrefixType(m_ui->comboBoxPrefixType->currentIndex());
    m_data.setSuffixType(m_ui->comboBoxSuffixType->currentIndex());
    
    if (m_data.getPrefixType() != 0) {
        m_data.setPrefixName(m_ui->lineEditPrefix->text());
    }
    if (m_data.getSuffixType() == 0) {
        m_data.setSuffixName(m_ui->lineEditSuffix->text());
    }
}

void SettingsController::updateUI()
{
    if (!m_ui) return;

    // Sync data model from external pointers
    syncFromExternalPointers();

    // Update paths
    m_ui->lineEdit_tempPath->setText(m_data.getTempFolder());
    m_ui->lineEdit_outPath->setText(m_data.getOutputFolder());

    // Update checkboxes
    m_ui->checkBox_protection->setChecked(m_data.getProtectFlag());
    m_ui->checkBox_tray->setChecked(m_data.getHideInTrayFlag());
    m_ui->checkBox_showHDR->setChecked(m_data.getShowHdrFlag());
    m_ui->checkBox_allowDuplicates->setChecked(m_data.getMultiInstances());
    m_ui->checkBox_subtitles_deselectall->setChecked(m_data.getSubtitlesDeselectAll());
    m_ui->checkBox_subtitles_background->setChecked(m_data.getSubtitlesBackground());

    // Update spinboxes
    m_ui->spinBox_threads->setValue(m_data.getThreads());
    m_ui->spinBox_protectionTimer->setValue(m_data.getTimerInterval());

    // Update combo boxes
    m_ui->comboBox_theme->setCurrentIndex(m_data.getTheme() < 2 ? m_data.getTheme() : 0);
    m_ui->comboBox_priority->setCurrentIndex(m_data.getFFMpegPrio());
    m_ui->comboBoxPrefixType->setCurrentIndex(m_data.getPrefixType());
    m_ui->comboBoxSuffixType->setCurrentIndex(m_data.getSuffixType());

    // Update text fields
    if (m_data.getSuffixType() == 0) {
        m_ui->lineEditSuffix->setText(m_data.getSuffixName());
    }

    // Update font sizes
    updateFontSizes();

    // Update subtitle settings
    updateSubtitleSettings();
}

void SettingsController::applySettings()
{
    // Load data from UI
    loadFromUI();

    // Sync to external pointers (for backward compatibility)
    syncToExternalPointers();
}

void SettingsController::resetSettings()
{
    m_data.resetToDefaults();
    
    // Update UI to reflect defaults
    if (m_ui) {
        m_ui->lineEdit_tempPath->clear();
        m_ui->lineEdit_outPath->clear();
        m_ui->checkBox_showHDR->setChecked(false);
        m_ui->checkBox_tray->setChecked(false);
        m_ui->checkBox_protection->setChecked(false);
        m_ui->comboBox_priority->setCurrentIndex(normal);
        m_ui->checkBox_allowDuplicates->setChecked(false);
        m_ui->spinBox_protectionTimer->setEnabled(false);
        m_ui->spinBox_threads->setValue(0);
        m_ui->comboBox_theme->setCurrentIndex(0);
        m_ui->comboBox_lang->setCurrentIndex(0);
        m_ui->comboBoxPrefixType->setCurrentIndex(0);
        m_ui->comboBoxSuffixType->setCurrentIndex(0);
        m_ui->lineEditPrefix->setText(DEFAULTPREFIX);
        m_ui->lineEditSuffix->setText(DEFAULTSUFFIX);
        m_ui->comboBox_fontsize->setCurrentIndex(0);
    }
}

bool SettingsController::validateSettings() const
{
    // Add validation logic here
    return true;
}

bool SettingsController::requiresRestart() const
{
    return m_data.requiresRestart();
}

const SettingsData& SettingsController::getData() const
{
    return m_data;
}

void SettingsController::syncToExternalPointers()
{
    if (!m_pOutputFolder || !m_pTempFolder) return;

    *m_pOutputFolder = m_data.getOutputFolder();
    *m_pTempFolder = m_data.getTempFolder();
    *m_pProtectFlag = m_data.getProtectFlag();
    *m_pMultiInstances = m_data.getMultiInstances();
    *m_pShowHdrFlag = m_data.getShowHdrFlag();
    *m_pTimerInterval = m_data.getTimerInterval();
    *m_pThreads = m_data.getThreads();
    *m_pFFMpegPrio = m_data.getFFMpegPrio();
    *m_pTheme = m_data.getTheme();
    *m_pLanguage = m_data.getLanguage();
    *m_pFont = m_data.getFont();
    *m_pFontSize = m_data.getFontSize();
    *m_pHideInTrayFlag = m_data.getHideInTrayFlag();
    *m_pPrefxType = m_data.getPrefixType();
    *m_pPrefixName = m_data.getPrefixName();
    *m_pSuffixType = m_data.getSuffixType();
    *m_pSuffixName = m_data.getSuffixName();
    *m_pSubtitlesDeselectAll = m_data.getSubtitlesDeselectAll();
    *m_pSubtitlesBackground = m_data.getSubtitlesBackground();
    *m_pSubtitlesColor = m_data.getSubtitlesColor();
    *m_pSubtitlesBackgroundColor = m_data.getSubtitlesBackgroundColor();
    *m_pSubtitlesLocation = m_data.getSubtitlesLocation();
    *m_pSubtitlesFont = m_data.getSubtitlesFont();
    *m_pSubtitlesFontSize = m_data.getSubtitlesFontSize();
}

void SettingsController::syncFromExternalPointers()
{
    if (!m_pOutputFolder || !m_pTempFolder) return;

    m_data.setOutputFolder(*m_pOutputFolder);
    m_data.setTempFolder(*m_pTempFolder);
    m_data.setProtectFlag(*m_pProtectFlag);
    m_data.setMultiInstances(*m_pMultiInstances);
    m_data.setShowHdrFlag(*m_pShowHdrFlag);
    m_data.setTimerInterval(*m_pTimerInterval);
    m_data.setThreads(*m_pThreads);
    m_data.setFFMpegPrio(*m_pFFMpegPrio);
    m_data.setTheme(*m_pTheme);
    m_data.setLanguage(*m_pLanguage);
    m_data.setFont(*m_pFont);
    m_data.setFontSize(*m_pFontSize);
    m_data.setHideInTrayFlag(*m_pHideInTrayFlag);
    m_data.setPrefixType(*m_pPrefxType);
    m_data.setPrefixName(*m_pPrefixName);
    m_data.setSuffixType(*m_pSuffixType);
    m_data.setSuffixName(*m_pSuffixName);
    m_data.setSubtitlesDeselectAll(*m_pSubtitlesDeselectAll);
    m_data.setSubtitlesBackground(*m_pSubtitlesBackground);
    m_data.setSubtitlesColor(*m_pSubtitlesColor);
    m_data.setSubtitlesBackgroundColor(*m_pSubtitlesBackgroundColor);
    m_data.setSubtitlesLocation(*m_pSubtitlesLocation);
    m_data.setSubtitlesFont(*m_pSubtitlesFont);
    m_data.setSubtitlesFontSize(*m_pSubtitlesFontSize);

    // Initialize temporary colors
    m_subtitlesColorTemp = *m_pSubtitlesColor;
    m_subtitlesBackgroundColorTemp = *m_pSubtitlesBackgroundColor;
}

void SettingsController::setupFontComboBoxes()
{
    if (!m_ui) return;

    QFontDatabase::WritingSystem values = QFontDatabase::WritingSystem::Latin;
    const QStringList fontFamilies = QFontDatabase::families(values);
    
    auto *fontModel = new QStringListModel(m_ui->comboBox_font);
    fontModel->setStringList(fontFamilies);
    m_ui->comboBox_font->blockSignals(true);
    m_ui->comboBox_font->setModel(fontModel);
    
    const QString appFontFamily = qApp->font().family();
    const int fontInd = m_ui->comboBox_font->findText(appFontFamily);
    if (fontInd != -1) {
        m_ui->comboBox_font->setCurrentIndex(fontInd);
    }
    m_ui->comboBox_font->blockSignals(false);

    auto *subtitlesFontModel = new QStringListModel(m_ui->comboBox_subtitles_font);
    subtitlesFontModel->setStringList(fontFamilies);
    m_ui->comboBox_subtitles_font->blockSignals(true);
    m_ui->comboBox_subtitles_font->setModel(subtitlesFontModel);
    
    const int subtitlesFontInd = m_ui->comboBox_subtitles_font->findText(m_data.getSubtitlesFont());
    if (subtitlesFontInd != -1) {
        m_ui->comboBox_subtitles_font->setCurrentIndex(subtitlesFontInd);
    }
    m_ui->comboBox_subtitles_font->blockSignals(false);

    // Set up list views for combo boxes
    auto *comboboxFontListView = new QListView(m_ui->comboBox_font);
    auto *comboboxSubtitlesFontListView = new QListView(m_ui->comboBox_subtitles_font);
    m_ui->comboBox_font->setView(comboboxFontListView);
    m_ui->comboBox_subtitles_font->setView(comboboxSubtitlesFontListView);

    // Set up validators
    auto *prefixValidator = new QRegularExpressionValidator(
        QRegularExpression("^[^\\\\/:*?\"<>|+%!@]*$"), m_ui->lineEditPrefix);
    auto *suffixValidator = new QRegularExpressionValidator(
        QRegularExpression("^[^\\\\/:*?\"<>|+%!@]*$"), m_ui->lineEditSuffix);
    m_ui->lineEditPrefix->setValidator(prefixValidator);
    m_ui->lineEditSuffix->setValidator(suffixValidator);
}

void SettingsController::updateFontSizes()
{
    if (!m_ui) return;

    const int fontSizes[6] = {8, 9, 10, 11, 12, 13};
    
    // Find the index for current font size
    int fontSizeIndex = 0;
    for (int i = 0; i < 6; i++) {
        if (fontSizes[i] == m_data.getFontSize()) {
            fontSizeIndex = i;
            break;
        }
    }
    
    int subtitlesFontSizeIndex = 0;
    for (int i = 0; i < 6; i++) {
        if (fontSizes[i] == m_data.getSubtitlesFontSize()) {
            subtitlesFontSizeIndex = i;
            break;
        }
    }
    
    m_ui->comboBox_fontsize->setCurrentIndex(fontSizeIndex);
    m_ui->comboBox_subtitles_fontsize->setCurrentIndex(subtitlesFontSizeIndex);
}

void SettingsController::updateSubtitleSettings()
{
    if (!m_ui) return;

    QString backgroundColorStyle = "background: " + m_data.getSubtitlesBackgroundColor().name() + ";";
    m_ui->subtitles_background_color->setStyleSheet(backgroundColorStyle);
    m_ui->subtitles_background_color->update();

    QString colorStyle = "background: " + m_data.getSubtitlesColor().name() + ";";
    m_ui->subtitles_color->setStyleSheet(colorStyle);
    m_ui->subtitles_color->update();

    m_ui->comboBox_subtitles_location->blockSignals(true);
    m_ui->comboBox_subtitles_location->setCurrentIndex(m_data.getSubtitlesLocation());
    m_ui->comboBox_subtitles_location->blockSignals(false);
}