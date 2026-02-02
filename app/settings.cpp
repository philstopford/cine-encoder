/***********************************************************************

                          C I N E   E N C O D E R
                                JULY, 2020
                            COPYRIGHT (C) 2020

 FILE: settings.cpp
 COMMENT:
 LICENSE: GNU General Public License v3.0

***********************************************************************/

#include "settings.h"
#include "ui_settings.h"
#include "settingscontroller.h"
#include "uiconnectionhelper.h"
#include <QFileDialog>
#include <QKeyEvent>
#include <QListView>
#include <QMap>
#include <QFontDatabase>
#include <QStringListModel>
#include <QColorDialog>
#include "constants.h"

typedef void(Settings::*FnVoidVoid)();
typedef void(Settings::*FnVoidInt)(int);

using namespace Constants;

Settings::Settings(QWidget *parent):
    BaseWindow(parent, true),
    ui(new Ui::Settings),
    m_controller(new SettingsController(this)),
    m_windowActivated(false)
{
    ui->setupUi(centralWidget());
    setTitleBar(ui->frame_top);
    ui->frameMiddle->setFocusPolicy(Qt::StrongFocus);
    ui->frame_main->setProperty("scale", int(Helper::scaling() * 100));

    setupConnections();
    setupUIComponents();
}

Settings::~Settings()
{
    delete m_controller;
    delete ui;
}

void Settings::setParameters(QString    *pOutputFolder,
                             QString    *pTempFolder,
                             bool       *pProtectFlag,
                             bool       *pMultiInstances,
                             bool       *pShowHdrFlag,
                             int        *pTimerInterval,
                             int        *pThreads,
                             int        *pFfmpegPrio,
                             int        *pTheme,
                             QString    *pPrefixName,
                             QString    *pSuffixName,
                             int        *pPrefxType,
                             int        *pSuffixType,
                             bool       *pHideInTrayFlag,
                             QString    *pLanguage,
                             int        *pFontSize,
                             QString    *pFont,
                             int        *pSubtitlesFontSize,
                             QString    *pSubtitlesFont,
                             bool       *pSubtitlesDeselectAll,
                             bool       *pSubtitlesBackground,
                             QColor     *pSubtitlesColor,
                             QColor     *pSubtitlesBackgroundColor,
                             int        *pSubtitlesBackgroundAlpha,
                             int        *pSubtitlesLocation,
                             bool       *pDeinterlaceEnabled,
                             int        *pDeinterlaceFilter)
{
    QFont title_font;
    title_font.setPointSize(10);
    ui->label_title->setFont(title_font);

    // Store member pointers for backward compatibility
    m_pShowHdrFlag = pShowHdrFlag;
    m_pOutputFolder = pOutputFolder;
    m_pTempFolder = pTempFolder;
    m_pProtectFlag = pProtectFlag;
    m_pMultiInstances = pMultiInstances;
    m_pTimerInterval = pTimerInterval;
    m_pThreads = pThreads;
    m_pFFMpegPrio = pFfmpegPrio;
    m_pTheme = pTheme;
    m_pPrefixName = pPrefixName;
    m_pSuffixName = pSuffixName;
    m_pPrefxType = pPrefxType;
    m_pSuffixType = pSuffixType;
    m_pHideInTrayFlag = pHideInTrayFlag;
    m_pLanguage = pLanguage;
    m_pFont = pFont;
    m_pFontSize = pFontSize;
    m_pSubtitlesFont = pSubtitlesFont;
    m_pSubtitlesFontSize = pSubtitlesFontSize;
    m_pSubtitlesDeselectAll = pSubtitlesDeselectAll;
    m_pSubtitlesBackground = pSubtitlesBackground;
    m_pSubtitlesColor = pSubtitlesColor;
    m_pSubtitlesBackgroundColor = pSubtitlesBackgroundColor;
    m_pSubtitlesBackgroundAlpha = pSubtitlesBackgroundAlpha;
    m_pSubtitlesLocation = pSubtitlesLocation;
    m_pDeinterlaceEnabled = pDeinterlaceEnabled;
    m_pDeinterlaceFilter = pDeinterlaceFilter;

    // Initialize temp colors
    m_pSubtitlesColor_temp = QColor(m_pSubtitlesColor->name());
    m_pSubtitlesBackgroundColor_temp = QColor(m_pSubtitlesBackgroundColor->name());
    ui->spinBox_background->setValue(*m_pSubtitlesBackgroundAlpha);

    // Initialize controller with all parameters
    m_controller->initialize(ui, 
                           pOutputFolder, pTempFolder, pProtectFlag, pMultiInstances,
                           pShowHdrFlag, pTimerInterval, pThreads, pFfmpegPrio, pTheme,
                           pPrefixName, pSuffixName, pPrefxType, pSuffixType,
                           pLanguage, pHideInTrayFlag, pFont, pFontSize,
                           pSubtitlesDeselectAll, pSubtitlesBackground,
                           pSubtitlesColor, pSubtitlesBackgroundColor,
                           pSubtitlesLocation, pSubtitlesFont, pSubtitlesFontSize,
                           pDeinterlaceEnabled, pDeinterlaceFilter);

    // Update UI from current data
    m_controller->updateUI();
}

void Settings::onCloseWindow()
{
    closeDialog();
}

void Settings::onButtonApply()
{
    // Use controller to handle the business logic
    m_controller->applySettings();
    
    // Check if restart is needed
    restart_needed = m_controller->requiresRestart();
    
    acceptDialog();
}

void Settings::onButtonReset()
{
    // Use controller to handle reset logic
    m_controller->resetSettings();
    restart_needed = true;
}

void Settings::showEvent(QShowEvent *event)
{
    BaseWindow::showEvent(event);
    if (!m_windowActivated) {
        m_windowActivated = true;
        setMinimumSize(QSize(589, 550) * Helper::scaling());
        SETTINGS(stn);
        if (stn.childGroups().contains("SettingsWidget")) {
            stn.beginGroup("SettingsWidget");
            restoreGeometry(stn.value("SettingsWidget/geometry", geometry()).toByteArray());
            stn.endGroup();
        } else {
            resize(QSize(589, 654) * Helper::scaling());
        }
        QSizeF size(this->size());
        QPoint center = QPointF(size.width()/2, size.height()/2).toPoint();
        move(parentWidget()->geometry().center() - center);
        setStyleSheet(Helper::getCss(*m_pTheme));
    }
}

bool Settings::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        auto *keyEvent = dynamic_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return) {
            ui->frameMiddle->setFocus();
            return true;
        }
    }
    return BaseWindow::eventFilter(watched, event);
}

void Settings::onButtonOutputPath()
{
    QStringList result;
    Helper::openFileDialog(Helper::FileDialogType::SELECTFOLDER,
                           tr("Select output folder"),
                           *m_pOutputFolder,
                           result);
    if (!result.isEmpty()) {
        ui->lineEdit_outPath->setText(result.at(0));
    }
}

void Settings::onButtonTempPath()
{
    QStringList result;
    Helper::openFileDialog(Helper::FileDialogType::SELECTFOLDER,
                           tr("Select temp folder"),
                           *m_pTempFolder,
                           result);
    if (!result.isEmpty()) {
        ui->lineEdit_tempPath->setText(result.at(0));
    }
}

void Settings::onCheckBoxProtectFlag_clicked()
{
    int stts_protect = ui->checkBox_protection->checkState();
    ui->spinBox_protectionTimer->setEnabled(stts_protect == 2);
}

void Settings::onComboBoxFfmpegPriority_indexChanged(int index)
{
    if (index < lowest) {
        index = lowest;
    }
    if (index > highest) {
        index = highest;
    }
    *m_pFFMpegPrio = index;
}

void Settings::onComboBoxPrefixType_indexChanged(int index)
{
    if (index == 0) {
        ui->lineEditPrefix->setEnabled(false);
        ui->lineEditPrefix->setText(tr("None"));
    } else {
        ui->lineEditPrefix->setEnabled(true);
        ui->lineEditPrefix->setText(*m_pPrefixName);
    }
}

void Settings::onComboBoxSuffixType_indexChanged(int index)
{
    if (index == 0) {
        ui->lineEditSuffix->setEnabled(true);
        ui->lineEditSuffix->setText(*m_pSuffixName);
    } else {
        ui->lineEditSuffix->setEnabled(false);
        ui->lineEditSuffix->setText("_hhmmss_MMddyyyy");
    }
}

void Settings::onComboBoxFont_indexChanged(int index)
{
    QFont font;
    const QString family = ui->comboBox_font->itemText(index);
    font.setFamily(family);
    ui->comboBox_font->setFont(font);
}

void Settings::onComboBoxSubtitlesFont_indexChanged(int index)
{
    QFont font;
    const QString family = ui->comboBox_subtitles_font->itemText(index);
    font.setFamily(family);
    ui->comboBox_subtitles_font->setFont(font);
}

void Settings::subtitles_color_change()
{
    QColorDialog cdialog(*m_pSubtitlesColor);

    m_pSubtitlesColor_temp = QColor(m_pSubtitlesColor->red(),
                                    m_pSubtitlesColor->green(),
                                    m_pSubtitlesColor->blue(),
                                    0);

    if (cdialog.exec() == QDialog::Accepted) {
        m_pSubtitlesColor_temp = QColorDialog::getColor();
        m_pSubtitlesColor_temp.setAlpha(0);

        QString s("background: " + m_pSubtitlesColor_temp.name() + ";");
        ui->subtitles_color->setStyleSheet(s);
        ui->subtitles_color->update();
    }
}

void Settings::subtitles_background_color_change()
{
    m_pSubtitlesBackgroundColor_temp = QColor(m_pSubtitlesBackgroundColor->red(),
                                              m_pSubtitlesBackgroundColor->green(),
                                              m_pSubtitlesBackgroundColor->blue(),
                                              m_pSubtitlesBackgroundColor->alpha());
    QColorDialog cdialog(m_pSubtitlesBackgroundColor_temp);
    if (cdialog.exec() == QDialog::Accepted) {
        m_pSubtitlesBackgroundColor_temp = QColorDialog::getColor();
        m_pSubtitlesBackgroundColor_temp = QColor(m_pSubtitlesBackgroundColor_temp.red(),
                                                  m_pSubtitlesBackgroundColor_temp.green(),
                                                  m_pSubtitlesBackgroundColor_temp.blue(),
                                                  m_pSubtitlesBackgroundColor->alpha());

        QString s("background: "
                  + m_pSubtitlesBackgroundColor_temp.name()
                  + ";");
        ui->subtitles_background_color->setStyleSheet(s);
        ui->subtitles_background_color->update();
    }
}

void Settings::setupConnections()
{
    // Use helper for button connections
    QPushButton *btns[] = {
        ui->closeWindow, ui->buttonCancel, ui->buttonApply, ui->buttonReset,
        ui->buttonOutputPath, ui->buttonTempPath,
        ui->subtitles_background_color, ui->subtitles_color
    };
    FnVoidVoid btn_methods[] = {
        &Settings::onCloseWindow, &Settings::onCloseWindow, &Settings::onButtonApply,
        &Settings::onButtonReset, &Settings::onButtonOutputPath, &Settings::onButtonTempPath,
        &Settings::subtitles_background_color_change, &Settings::subtitles_color_change
    };
    UIConnectionHelper::connectButtons(btns, btn_methods, 8, this);

    // Use helper for tab button connections
    QList<QPushButton*> tabButtons = {
        ui->buttonTab_settingsPathsAndFiles, 
        ui->buttonTab_settingsAdvanced, 
        ui->buttonTab_settingsSubtitles
    };
    UIConnectionHelper::connectTabButtons(tabButtons, this, [this](int index) {
        ui->tabWidgetSettings->setCurrentIndex(index);
    });

    // Single checkbox connection
    UIConnectionHelper::connectSafely(ui->checkBox_protection, &QCheckBox::clicked, 
                                     this, &Settings::onCheckBoxProtectFlag_clicked);

    // Use helper for combo box connections
    QComboBox *boxes[] = {
        ui->comboBoxPrefixType, ui->comboBoxSuffixType, ui->comboBox_font, 
        ui->comboBox_subtitles_font, ui->comboBox_priority
    };
    FnVoidInt boxes_methods[] = {
        &Settings::onComboBoxPrefixType_indexChanged, &Settings::onComboBoxSuffixType_indexChanged,
        &Settings::onComboBoxFont_indexChanged, &Settings::onComboBoxSubtitlesFont_indexChanged,
        &Settings::onComboBoxFfmpegPriority_indexChanged
    };
    UIConnectionHelper::connectComboBoxes(boxes, boxes_methods, 5, this);

    // On close event
    connect(this, &Settings::destroyed, this, [this]() {
        SETTINGS(stn);
        stn.beginGroup("SettingsWidget");
        stn.setValue("SettingsWidget/geometry", this->saveGeometry());
        stn.endGroup();
    });
}

void Settings::setupUIComponents()
{
    // This method can be used for additional UI setup that doesn't involve connections
    // For now, it's kept simple to maintain existing behavior
}
