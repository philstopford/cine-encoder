/***********************************************************************

                          C I N E   E N C O D E R
                                JULY, 2020
                            COPYRIGHT (C) 2020

 FILE: mainwindow.cpp
 COMMENT:
 LICENSE: GNU General Public License v3.0

***********************************************************************/

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "uiconnectionhelper.h"
#include "notification.h"
#include "settings.h"
#include "preset.h"
#include "message.h"
#include "progress.h"
#include "tables.h"
#include "helper.h"
#include "report.h"
#include "streamconverter.h"
#include "fileiconprovider.h"
#include "configurationmanager.h"
#include <QDragEnterEvent>
#include <QMimeDatabase>
#include <QMimeData>
#include <QTableWidgetItem>
#include <QListView>
#include <QUrl>
#include <QList>
#include <QMenu>
#include <QDate>
#include <QStandardItemModel>
#include <QGridLayout>
#include <QDockWidget>
#include <QFile>
#include <QFileInfo>
#include <QSizePolicy>
#include <QTranslator>
#include <QScreen>
#include <QHeaderView>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <QOperatingSystemVersion>
#include <sstream>
#include <QXmlStreamWriter>
#include <QString>
#include <utility>

#if defined (Q_OS_UNIX)
    #ifndef UNICODE
        #define UNICODE
    #endif
    #include <MediaInfo/MediaInfo.h>
#include <QOperatingSystemVersion>

using namespace MediaInfoLib;
#elif defined(Q_OS_WIN64)
    #ifdef __MINGW64__
        #ifdef _UNICODE
            #define _itot _itow
        #else
            #define _itot itoa
        #endif
    #endif
    #include <windows.h>
    #include "MediaInfoDLL/MediaInfoDLL.h"
    using namespace MediaInfoDLL;
#endif

#define WINDOW_SIZE (QSize(1500, 920) * Helper::scaling())
#define ROWHEIGHT 25
#define ROWHEIGHTDFLT 45
#define DEFAULTPATH QDir::homePath()
#define PRESETFILE (SETTINGSPATH + QString("/presets.ini"))
#define XMLPRESETFILE (SETTINGSPATH + QString("/presets.xml"))
#define THUMBNAILPATH (SETTINGSPATH + QString("/thumbnails"))
#define GETINFO(a, b, c) QString::fromStdWString(MI.Get(a, b, __T(c)))
#define GINFO(a, b) QString::fromStdWString(MI.Get(Stream_General, a, __T(b), Info_Text, Info_Name))
#define VINFO(a, b) QString::fromStdWString(MI.Get(Stream_Video, a, __T(b)))
#define AINFO(a, b) QString::fromStdWString(MI.Get(Stream_Audio, a, __T(b)))
#define SINFO(a, b) QString::fromStdWString(MI.Get(Stream_Text, a, __T(b)))
#define GETTEXT(row, col) ui->tableWidget->item(row, ColumnIndex::col)->text()
// #define SLT(method) &MainWindow::method
// #define _CHECKS(row, chk) m_data[row].checks[Data::chk]
// #define _FIELDS(row, fld) m_data[row].fields[Data::fld]

typedef void(MainWindow::*FnVoidVoid)();
typedef void(MainWindow::*FnVoidInt)(int);

namespace MainWindowPrivate
{
    QLabel* createLabel(QWidget *parent, const char *name, const QString &text)
    {
        auto *label = new QLabel(parent);
        label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        label->setObjectName(QString::fromUtf8(name));
        label->setAlignment(Qt::AlignCenter);
        label->setText(text);
        return label;
    }

    void undoLineEdit(QLineEdit *line) {
        line->undo();
        if (line->text() != "") {
            line->setFocus();
            line->setCursorPosition(0);
            line->setModified(true);
        }
    };

    void clearLineEdit(QLineEdit *line) {
        line->clear();
        line->insert("");
        line->setFocus();
        line->setModified(true);
    }
}

MainWindow::MainWindow(QWidget *parent):
    BaseWindow(parent),
    ui(new Ui::Widget),
    m_pProcessThumbCreation(nullptr),
    m_openDir(DEFAULTPATH),
    m_status_encode_btn(EncodingStatus::START),
    m_row(-1),
    m_curTime(0),
    m_curFilename(""),
    m_curPath(""),
    m_temp_file(""),
    m_input_file(""),
    m_output_file(""),
    m_windowActivated(false),
    m_expandWindowsState(false),
    m_rowHeight(ROWHEIGHTDFLT)
{
    ui->setupUi(centralWidget());
    setTitleBar(ui->frame_top);
    //*************** Set labels *****************//
    auto *pTableLayout = new QHBoxLayout(ui->tableWidget);
    ui->tableWidget->setLayout(pTableLayout);
    m_pTableLabel = MainWindowPrivate::createLabel(ui->tableWidget, "TableWidgetLabel", tr("No media"));
    pTableLayout->addWidget(m_pTableLabel);
    m_pAudioLabel = MainWindowPrivate::createLabel(ui->frameAudio, "AudioLabel", tr("No audio"));
    ui->gridLayoutAudio->addWidget(m_pAudioLabel);
    m_pSubtitleLabel = MainWindowPrivate::createLabel(ui->frameSubtitle, "SubtitleLabel", tr("No subtitles"));
    ui->gridLayoutSubtitle->addWidget(m_pSubtitleLabel);

    //************** Create docks ******************//
    auto *pMiddleLayout = new QGridLayout(ui->frameMiddle);
    ui->frameMiddle->setLayout(pMiddleLayout);
    m_pDocksContainer = new QMainWindow(ui->frameMiddle);
    pMiddleLayout->addWidget(m_pDocksContainer);
    pMiddleLayout->setContentsMargins(2, 2, 2, 2);
    pMiddleLayout->setSpacing(0);
    m_pDocksContainer->setObjectName("DocksContainer");
    m_pDocksContainer->setWindowFlags(Qt::Widget);
    m_pDocksContainer->setDockNestingEnabled(true);

    m_pCentralDock = new QWidget(m_pDocksContainer);
    m_pCentralDock->setObjectName("CentralDock");
    m_pDocksContainer->setCentralWidget(m_pCentralDock);

    m_pDocksContainer->setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
    m_pDocksContainer->setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
    m_pDocksContainer->setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
    m_pDocksContainer->setCorner(Qt::BottomRightCorner, Qt::BottomDockWidgetArea);

    auto *pCentralDockLayout = new QGridLayout(m_pCentralDock);
    m_pCentralDock->setLayout(pCentralDockLayout);
    pCentralDockLayout->addWidget(ui->frameTask);
    pCentralDockLayout->setContentsMargins(0, 0, 0, 0);

    auto *pFrameSource = new QFrame(ui->frameMiddle);
    auto *pSourceLayout = new QGridLayout(pFrameSource);
    pSourceLayout->setContentsMargins(0, 0, 0, 0);
    pFrameSource->setLayout(pSourceLayout);
    m_pSplSource = new QSplitter(Qt::Horizontal, pFrameSource);
    pSourceLayout->addWidget(m_pSplSource);
    m_pSplSource->setHandleWidth(0);
    ui->framePreview->setParent(pFrameSource);
    ui->frameSource->setParent(pFrameSource);
    m_pSplSource->addWidget(ui->framePreview);
    m_pSplSource->addWidget(ui->frameSource);

    QString dockNames[] = {
        tr("Source"), tr("Presets"), tr("Output"),
        tr("Streams"), tr("Log"), tr("Metadata"), tr("Split"), tr("Browser")
    };
    Qt::DockWidgetArea dockArea[] = {
        Qt::LeftDockWidgetArea,   Qt::LeftDockWidgetArea,
        Qt::BottomDockWidgetArea, Qt::RightDockWidgetArea,
        Qt::RightDockWidgetArea,  Qt::RightDockWidgetArea,
        Qt::RightDockWidgetArea,  Qt::LeftDockWidgetArea
    };
    QString objNames[] = {
        "dockSource", "dockPresets", "dockOutput", "dockStreams",
        "dockLog", "dockMetadata", "dockSplit", "dockBrowser"
    };
    QFrame* dockFrames[] = {
        pFrameSource, ui->framePreset,  ui->frameOutput,
        ui->frameStreams, ui->frameLog, ui->frameMetadata, ui->frameSplit, ui->frameBrowser
    };
    for (int i = 0; i < DOCKS_COUNT; i++) {
        m_pDocks[i] = new QDockWidget(dockNames[i], m_pDocksContainer);
        m_pDocks[i]->setObjectName(objNames[i]);
        m_pDocks[i]->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::TopDockWidgetArea |
                                     Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);
        m_pDocks[i]->setFeatures(QDockWidget::DockWidgetClosable |
                                 QDockWidget::DockWidgetMovable |
                                 QDockWidget::DockWidgetFloatable
                /*QDockWidget::DockWidgetVerticalTitleBar*/);
        m_pDocks[i]->setWidget(dockFrames[i]);
        m_pDocksContainer->addDockWidget(dockArea[i], m_pDocks[i]);
    }
    for (int i = DockIndex::LOG_DOCK; i < DockIndex::SPLIT_DOCK + 1; i++) {
        m_pDocks[i]->toggleViewAction()->setChecked(false);
        m_pDocks[i]->setVisible(false);
        m_pDocks[i]->setFloating(true);
    }

    ui->streamAudio->setContentType(QStreamView::Content::Audio);
    ui->streamSubtitle->setContentType(QStreamView::Content::Subtitle);
    /*ui->switchCutting->setIcons(QIcon(QPixmap(":/resources/icons/svg/shortest.svg")),
                                QIcon(QPixmap(":/resources/icons/svg/not_shortest.svg")));
    ui->switchViewMode->setIcons(QIcon(QPixmap(":/resources/icons/svg/view_list.svg")),
                                 QIcon(QPixmap(":/resources/icons/svg/view_icons.svg")));*/
    ui->switchCutting->setToolTips(tr("Without cutting"), tr("Cut by shortest"));
    ui->switchViewMode->setToolTips(tr("List view"), tr("Icon view"));

    //*********** Set Event Filters ****************//
    m_pTableLabel->installEventFilter(this);
    ui->labelPreview->installEventFilter(this);
    ui->frameMiddle->setFocusPolicy(Qt::StrongFocus);
    setAcceptDrops(true);
    
    // Load column visibility settings
    loadColumnVisibilitySettings();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showEvent(QShowEvent *event)
{
    BaseWindow::showEvent(event);
    if (!m_windowActivated) {
        m_windowActivated = true;
        setParameters();
    }
}

void MainWindow::closeEvent(QCloseEvent *event) // Show prompt when close app
{
    event->ignore();
    if (showDialogMessage(tr("Quit program?"))) {
        if (m_pEncoder->getEncodingState() != QProcess::NotRunning)
            m_pEncoder->killEncoding();
        if (m_pProcessThumbCreation) {
            if (m_pProcessThumbCreation->state() != QProcess::NotRunning)
                m_pProcessThumbCreation->kill();
        }

        QFile xmlFile(XMLPRESETFILE);
        if (!xmlFile.open(QFile::WriteOnly | QFile::Text)) { // Open file in write only mode
            qDebug() << QString("Cannot write file %1(%2).").arg(XMLPRESETFILE, xmlFile.errorString());
            return;
        }
        QXmlStreamWriter stream(&xmlFile);
        stream.setAutoFormatting(true);
        stream.writeStartDocument();
        stream.writeStartElement("cineencoder");
        stream.writeTextElement("version", numToStr(PRESETS_VERSION));
        stream.writeTextElement("m_pos_top", numToStr(m_pos_top));
        stream.writeTextElement("m_pos_cld", numToStr(m_pos_cld));

        stream.writeStartElement("params");
        int i = 0;
        for (const QString& param : m_curParams) {
            stream.writeStartElement(param_names[i]);
            stream.writeCharacters(param);
            stream.writeEndElement();
            i++;
        }
        stream.writeEndElement();

        stream.writeStartElement("presettable");

        // Data structure internally is column-wise preset. We need to capture each column into an XML
        // entry
        int paramscount;
        try {
            paramscount = m_preset_table.count();
        }
        catch (...)
        {
            paramscount = 0;
        }
        int presetcount;
        try {
            presetcount = m_preset_table[0].count();

        }
        catch (...)
        {
            presetcount = 0;
        }

        // Preset
        for (int preset = 0; preset < presetcount; preset++) {
            stream.writeStartElement(QString("preset") + numToStr(preset));
            // Parameters
            for (int param = 0; param < paramscount; param++)
            {
                if (param < PARAMETERS_COUNT) {
                    stream.writeStartElement(param_names[param]);
                }
                else
                {
                    stream.writeStartElement("TYPE");
                }
                stream.writeCharacters(m_preset_table[param][preset]);
                stream.writeEndElement();
            }
            stream.writeEndElement();
        }

        stream.writeEndElement();
        stream.writeEndDocument();
        xmlFile.close();

        SETTINGS(stn);
        // Save Version
        stn.setValue("Version", SETTINGS_VERSION);
        // Save Window
        stn.beginGroup("Window");
        stn.setValue("Window/geometry", this->saveGeometry());
        stn.endGroup();
        // Save DocksContainer
        stn.beginGroup("DocksContainer");
        stn.setValue("DocksContainer/state", m_pDocksContainer->saveState());
        stn.setValue("DocksContainer/geometry", m_pDocksContainer->saveGeometry());
        stn.beginWriteArray("DocksContainer/docks_geometry");
            for (auto & m_pDock : m_pDocks) {
                stn.setArrayIndex(i);
                stn.setValue("DocksContainer/docks_geometry/dock_size", m_pDock->size());
            }
            stn.endArray();
        stn.endGroup();
        // Save Tables
        stn.beginGroup("Tables");
        stn.setValue("Tables/table_widget_state", ui->tableWidget->horizontalHeader()->saveState());
        stn.setValue("Tables/tree_widget_state", ui->treeWidget->header()->saveState());
        stn.setValue("Tables/splitter_state", m_pSpl->saveState());
        stn.setValue("Tables/splitter_source_state", m_pSplSource->saveState());
        stn.endGroup();

        saveXMLSettingsFile();
        
        // Save column visibility settings
        saveColumnVisibilitySettings();

        if (m_pTrayIcon)
            m_pTrayIcon->deleteLater();
        event->accept();
    }
}

void MainWindow::saveXMLSettingsFile()
{
    QFile xmlSettingsFile(XMLSETTINGSFILE);
    if (!xmlSettingsFile.open(QFile::WriteOnly | QFile::Text)) { // Open file in write only mode
        qDebug() << QString("Cannot write file %1(%2).").arg(XMLSETTINGSFILE, xmlSettingsFile.errorString());
        return;
    }
    QXmlStreamWriter streamSettings(&xmlSettingsFile);
    streamSettings.setAutoFormatting(true);
    streamSettings.writeStartDocument();
    streamSettings.writeStartElement("cineencoder");
    streamSettings.writeTextElement("version", numToStr(SETTINGS_VERSION));

    streamSettings.writeStartElement("settings");
    streamSettings.writeStartElement("prefix_type");
    streamSettings.writeCharacters(numToStr(m_prefxType));
    streamSettings.writeEndElement();
    streamSettings.writeStartElement("suffix_type");
    streamSettings.writeCharacters(numToStr(m_suffixType));
    streamSettings.writeEndElement();
    streamSettings.writeStartElement("prefix_name");
    streamSettings.writeCharacters(m_prefixName);
    streamSettings.writeEndElement();
    streamSettings.writeStartElement("suffix_name");
    streamSettings.writeCharacters(m_suffixName);
    streamSettings.writeEndElement();
    streamSettings.writeStartElement("timer_interval");
    streamSettings.writeCharacters(numToStr(m_timerInterval));
    streamSettings.writeEndElement();
    streamSettings.writeStartElement("theme");
    streamSettings.writeCharacters(numToStr(m_theme));
    streamSettings.writeEndElement();
    streamSettings.writeStartElement("protection");
    streamSettings.writeCharacters(numToStr(m_protectFlag));
    streamSettings.writeEndElement();
    streamSettings.writeStartElement("allow_duplicates");
    streamSettings.writeCharacters(numToStr(m_multiInstances));
    streamSettings.writeEndElement();
    streamSettings.writeStartElement("show_hdr_mode");
    streamSettings.writeCharacters(numToStr(m_showHdrFlag));
    streamSettings.writeEndElement();
    streamSettings.writeStartElement("temp_folder");
    streamSettings.writeCharacters(m_temp_folder);
    streamSettings.writeEndElement();
    streamSettings.writeStartElement("output_folder");
    streamSettings.writeCharacters(m_output_folder);
    streamSettings.writeEndElement();
    streamSettings.writeStartElement("open_dir");
    streamSettings.writeCharacters(m_openDir);
    streamSettings.writeEndElement();
    streamSettings.writeStartElement("batch_mode");
    streamSettings.writeCharacters(numToStr(m_batch_mode));
    streamSettings.writeEndElement();
    streamSettings.writeStartElement("tray");
    streamSettings.writeCharacters(numToStr(m_hideInTrayFlag));
    streamSettings.writeEndElement();
    streamSettings.writeStartElement("language");
    streamSettings.writeCharacters(m_language);
    streamSettings.writeEndElement();
    streamSettings.writeStartElement("font");
    streamSettings.writeCharacters(m_font);
    streamSettings.writeEndElement();
    streamSettings.writeStartElement("font_size");
    streamSettings.writeCharacters(numToStr(m_fontSize));
    streamSettings.writeEndElement();
    streamSettings.writeStartElement("row_size");
    streamSettings.writeCharacters(numToStr(m_rowHeight));
    streamSettings.writeEndElement();
    streamSettings.writeStartElement("switch_view_mode");
    streamSettings.writeCharacters(numToStr(ui->switchViewMode->currentIndex()));
    streamSettings.writeEndElement();
    streamSettings.writeStartElement("switch_cut_mode");
    streamSettings.writeCharacters(numToStr(ui->switchCutting->currentIndex()));
    streamSettings.writeEndElement();
    streamSettings.writeStartElement("subtitles_font");
    streamSettings.writeCharacters(m_subtitles_font);
    streamSettings.writeEndElement();
    streamSettings.writeStartElement("subtitles_font_size");
    streamSettings.writeCharacters(numToStr(m_subtitles_fontSize));
    streamSettings.writeEndElement();

    std::string test = m_subtitles_color.name().toStdString();
    std::string test2 = m_subtitles_background_color.name().toStdString();

    streamSettings.writeStartElement("subtitles_color");
    streamSettings.writeCharacters(m_subtitles_color.name());
    streamSettings.writeEndElement();
    streamSettings.writeStartElement("subtitles_deselectall");
    streamSettings.writeCharacters(numToStr(m_subtitles_deselectall));
    streamSettings.writeEndElement();
    streamSettings.writeStartElement("subtitles_background");
    streamSettings.writeCharacters(numToStr(m_subtitles_background));
    streamSettings.writeEndElement();
    streamSettings.writeStartElement("subtitles_background_alpha");
    streamSettings.writeCharacters(numToStr(m_subtitles_background_alpha));
    streamSettings.writeEndElement();
    streamSettings.writeStartElement("subtitles_background_color");
    streamSettings.writeCharacters(m_subtitles_background_color.name());
    streamSettings.writeEndElement();
    streamSettings.writeStartElement("subtitles_location");
    streamSettings.writeCharacters(numToStr(m_subtitles_location));
    streamSettings.writeEndElement();
    streamSettings.writeStartElement("threads");
    streamSettings.writeCharacters(numToStr(m_threads));
    streamSettings.writeEndElement();
    streamSettings.writeStartElement("ffmpeg_prio");
    streamSettings.writeCharacters(numToStr(m_ffmpeg_prio));
    streamSettings.writeEndElement();

    streamSettings.writeEndElement();
    xmlSettingsFile.close();
}

void MainWindow::paintEvent(QPaintEvent *event) // Disable QTab draw base
{
    if (event->type() == QEvent::Paint) {
        auto tabBars = m_pDocksContainer->findChildren<QTabBar*>();
        foreach (QTabBar *tabBar, tabBars) {
            if (tabBar->drawBase())
                 tabBar->setDrawBase(false);
        }
    }
}

void MainWindow::setTrayIcon()
{
    m_pTrayIcon = new QSystemTrayIcon(this);
    m_pTrayIcon->setIcon(QIcon(QPixmap(":/resources/icons/svg/cine-encoder.svg")));

    auto *trayMenu = new QMenu(this);
    const int ACT_COUNT = 3;
    QString actNames[ACT_COUNT] = {tr("Hide"), tr("Show"), tr("Exit")};
    FnVoidVoid actMethods[ACT_COUNT] = {
        &MainWindow::hide, &MainWindow::onRestoreWindowState, &MainWindow::onCloseWindow
    };
    for (int i = 0; i < ACT_COUNT; i++) {
        auto *act = new QAction(actNames[i], trayMenu);
        connect(act, &QAction::triggered, this, actMethods[i]);
        trayMenu->addAction(act);
    }
    m_pTrayIcon->setContextMenu(trayMenu);
    connect(m_pTrayIcon, &QSystemTrayIcon::activated, this, [this](QSystemTrayIcon::ActivationReason rsn) {
        switch (rsn) {
        case QSystemTrayIcon::Trigger:
        case QSystemTrayIcon::DoubleClick:
            onRestoreWindowState();
            break;
        default:
            break;
        }
    });
}

void MainWindow::createConnections()
{
    // Use UIConnectionHelper for button array connections
    const int BTN_COUNT = 31;
    QPushButton *btns[BTN_COUNT] = {
        ui->closeWindow,  ui->hideWindow,    ui->expandWindow,
        ui->addFiles,     ui->removeFile,    ui->sortUp,
        ui->sortDown,     ui->stop,          ui->start,
        ui->settings,     ui->clearMetadata, ui->undoMetadata,
        ui->addExtStream, ui->clearTitles,   ui->undoTitles,
        ui->framePrev,    ui->frameNext,     ui->setStartTime,
        ui->setEndTime,   ui->removePreset,  ui->editPreset,
        ui->applyPreset,  ui->addFilesHot,   ui->setOutFolder,
        ui->closeTWindow, ui->resetLabels,   ui->report,
        ui->back,         ui->forward, ui->removeAllFiles,
        ui->deselectTitles
    };
    FnVoidVoid btn_methods[BTN_COUNT] = {
            &MainWindow::onCloseWindow,  &MainWindow::onHideWindow,    &MainWindow::onExpandWindow,
            &MainWindow::onAddFiles,     &MainWindow::onRemoveFile,    &MainWindow::onSortUp,
            &MainWindow::onSortDown,     &MainWindow::onStop,          &MainWindow::onStart,
            &MainWindow::onSettings,     &MainWindow::onClearMetadata, &MainWindow::onUndoMetadata,
            &MainWindow::onAddExtStream, &MainWindow::onClearTitles,   &MainWindow::onUndoTitles,
            &MainWindow::onFramePrev,    &MainWindow::onFrameNext,     &MainWindow::onSetStartTime,
            &MainWindow::onSetEndTime,   &MainWindow::onRemovePreset,  &MainWindow::onEditPreset,
            &MainWindow::onApplyPreset,  &MainWindow::onAddFiles,      &MainWindow::onSetOutFolder,
            &MainWindow::onCloseWindow,  &MainWindow::onResetLabels,   &MainWindow::onReport,
            &MainWindow::onBack,         &MainWindow::onForward,       &MainWindow::onRemoveAllFiles,
            &MainWindow::onDeselectTitles
    };
    UIConnectionHelper::connectButtons(btns, btn_methods, BTN_COUNT, this);

    // Individual connections using helper
    UIConnectionHelper::connectSafely(ui->comboBox_changePrio, 
                                     static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
                                     this, &MainWindow::changePriority);

    // Streams actions
    UIConnectionHelper::connectSafely(ui->streamAudio, &QStreamView::onExtractTrack, 
                                     this, &MainWindow::onExtract);
    UIConnectionHelper::connectSafely(ui->streamSubtitle, &QStreamView::onExtractTrack, 
                                     this, &MainWindow::onExtract);
    
    // Stream selection change notifications for incompatibility highlighting
    connect(ui->streamAudio, &QStreamView::streamSelectionChanged, this, [this]() {
        updateFileIncompatibilityStatus(ui->tableWidget->currentRow());
    });
    connect(ui->streamSubtitle, &QStreamView::streamSelectionChanged, this, [this]() {
        updateFileIncompatibilityStatus(ui->tableWidget->currentRow());
    });

    // Table and UI controls
    UIConnectionHelper::connectSafely(ui->tableWidget, &QTableWidget::itemSelectionChanged,
                                     this, &MainWindow::onTableSelectionChanged);
    UIConnectionHelper::connectSafely(ui->switchViewMode, &QDoubleButton::indexChanged, 
                                     this, &MainWindow::onViewMode);
    UIConnectionHelper::connectSafely(ui->sliderTimeline, &QSlider::valueChanged, 
                                     this, &MainWindow::onSliderTimelineChanged);
    UIConnectionHelper::connectSafely(ui->sliderResize, &QSlider::valueChanged, 
                                     this, &MainWindow::onSliderResizeChanged);

    // Tree widget connections
    UIConnectionHelper::connectSafely(ui->treeWidget, &QTreeWidget::itemCollapsed, 
                                     this, &MainWindow::onTreeCollapsed);
    UIConnectionHelper::connectSafely(ui->treeWidget, &QTreeWidget::itemExpanded, 
                                     this, &MainWindow::onTreeExpanded);
    UIConnectionHelper::connectSafely(ui->treeWidget, &QTreeWidget::itemChanged, 
                                     this, &MainWindow::onTreeChanged);
    UIConnectionHelper::connectSafely(ui->treeWidget, &QTreeWidget::itemDoubleClicked, 
                                     this, &MainWindow::onTreeDblClicked);

    UIConnectionHelper::connectSafely(ui->treeDirs, &QTreeView::clicked, 
                                     this, &MainWindow::onTreeDirsClicked);
    UIConnectionHelper::connectSafely(ui->treeDirs, &QTreeView::doubleClicked, 
                                     this, &MainWindow::onTreeDirsDblClicked);

    // Encoder connections
    m_pEncoder = new Encoder(this);
    UIConnectionHelper::connectSafely(m_pEncoder, &Encoder::onEncodingMode, 
                                     this, &MainWindow::onEncodingMode);
    UIConnectionHelper::connectSafely(m_pEncoder, &Encoder::onEncodingStarted, 
                                     this, &MainWindow::onEncodingStarted);
    UIConnectionHelper::connectSafely(m_pEncoder, &Encoder::onEncodingInitError, 
                                     this, &MainWindow::onEncodingInitError);
    UIConnectionHelper::connectSafely(m_pEncoder, &Encoder::onEncodingProgress, 
                                     this, &MainWindow::onEncodingProgress);
    UIConnectionHelper::connectSafely(m_pEncoder, &Encoder::onEncodingLog, 
                                     this, &MainWindow::onEncodingLog);
    UIConnectionHelper::connectSafely(m_pEncoder, &Encoder::onEncodingCompleted, 
                                     this, &MainWindow::onEncodingCompleted);
    UIConnectionHelper::connectSafely(m_pEncoder, &Encoder::onEncodingAborted, 
                                     this, &MainWindow::onEncodingAborted);
    UIConnectionHelper::connectSafely(m_pEncoder, &Encoder::onEncodingError, 
                                     this, &MainWindow::onEncodingError);

    // Timer connections (keeping old style for compatibility with SIGNAL/SLOT macros)
    m_pTimer = new QTimer(this);
    connect(m_pTimer, SIGNAL(timeout()), this, SLOT(repeatHandler_Type_1()));
    m_pTimerSetThumbnail = new QTimer(this);
    m_pTimerSetThumbnail->setSingleShot(true);
    m_pTimerSetThumbnail->setInterval(800);
    connect(m_pTimerSetThumbnail, SIGNAL(timeout()), this, SLOT(repeatHandler_Type_2()));

    // Legacy combo box connection (keeping for compatibility)  
    connect(ui->comboBoxMode, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboModeChanged(int)));

    //************ Top menu actions ****************//
    m_pMenuBar = new QMenuBar(ui->frame_top);
    m_pMenuBar->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    auto *pTopLayout = dynamic_cast<QGridLayout*>(ui->frame_top->layout());
    if (pTopLayout)
        pTopLayout->addWidget(m_pMenuBar, 0, 5, Qt::AlignVCenter);
    QMenu *menuFiles = m_pMenuBar->addMenu(tr("File"));
    QMenu *menuEdit = m_pMenuBar->addMenu(tr("Edit"));
    QMenu *menuTools = m_pMenuBar->addMenu(tr("Tools"));
    QMenu *menuView = m_pMenuBar->addMenu(tr("View"));
    QMenu *menuPreferences = m_pMenuBar->addMenu(tr("Preferences"));
    QMenu *menuAbout = m_pMenuBar->addMenu(tr("About"));

    m_pActAddFiles = new QAction(tr("Add files"), menuFiles);
    m_pActRemoveFile = new QAction(tr("Remove from the list"), menuFiles);
    m_pActRemoveAllFiles = new QAction(tr("Clear the list"), menuFiles);
    m_pActCloseWindow = new QAction(tr("Close"), menuFiles);
    connect(m_pActAddFiles, &QAction::triggered, this, &MainWindow::onAddFiles);
    connect(m_pActRemoveFile, &QAction::triggered, this, &MainWindow::onRemoveFile);
    connect(m_pActRemoveAllFiles, &QAction::triggered, this, &MainWindow::onRemoveAllFiles);
    connect(m_pActCloseWindow, &QAction::triggered, this, &MainWindow::onCloseWindow);
    menuFiles->addAction(m_pActAddFiles);
    menuFiles->addAction(m_pActRemoveFile);
    menuFiles->addAction(m_pActRemoveAllFiles);
    menuFiles->addSeparator();
    menuFiles->addAction(m_pActCloseWindow);

    m_pActStart = new QAction(tr("Encode/Pause"), menuEdit);
    m_pActStop = new QAction(tr("Stop"), menuEdit);
    connect(m_pActStart, &QAction::triggered, this, &MainWindow::onStart);
    connect(m_pActStop, &QAction::triggered, this, &MainWindow::onStop);
    menuEdit->addAction(m_pActStart);
    menuEdit->addAction(m_pActStop);

    m_pActEditMetadata = new QAction(tr("Edit metadata"), menuTools);
    m_pActSelectAudio = new QAction(tr("Select audio streams"), menuTools);
    m_pActSelectSubtitles = new QAction(tr("Select subtitles"), menuTools);
    m_pActDeselectAudio = new QAction(tr("Deselect audio streams"), menuTools);
    m_pActDeselectSubtitles = new QAction(tr("Deselect subtitles"), menuTools);
    m_pActSplitVideo = new QAction(tr("Split video"), menuTools);
    connect(m_pActEditMetadata, &QAction::triggered, this, &MainWindow::showMetadataEditor);
    connect(m_pActSelectAudio, &QAction::triggered, this, &MainWindow::showAudioStreams);
    connect(m_pActSelectSubtitles, &QAction::triggered, this, &MainWindow::showSubtitles);
    connect(m_pActDeselectAudio, &QAction::triggered, this, &MainWindow::clearAudioStreams);
    connect(m_pActDeselectSubtitles, &QAction::triggered, this, &MainWindow::clearSubtitleStreams);
    connect(m_pActSplitVideo, &QAction::triggered, this, &MainWindow::showVideoSplitter);
    menuTools->addAction(m_pActEditMetadata);
    menuTools->addSeparator();
    menuTools->addAction(m_pActSelectAudio);
    menuTools->addAction(m_pActSelectSubtitles);
    menuTools->addSeparator();
    menuTools->addAction(m_pActDeselectAudio);
    menuTools->addAction(m_pActDeselectSubtitles);
    menuTools->addSeparator();
    menuTools->addAction(m_pActSplitVideo);

    m_pActResetView = new QAction(tr("Reset state"), menuView);
    connect(m_pActResetView, &QAction::triggered, this, &MainWindow::resetView);
    for (int i = 0; i < DOCKS_COUNT; i++)
        menuView->addAction(m_pDocks[i]->toggleViewAction());
    menuView->addSeparator();
    
    // Column visibility submenu
    setupColumnVisibilityMenus();
    updateColumnVisibilityMenus(); // Ensure checkmarks reflect current state
    menuView->addMenu(m_pColumnsMenu);
    menuView->addSeparator();
    
    menuView->addAction(m_pActResetView);

    m_pActSettings = new QAction(tr("Settings"), menuPreferences);
    connect(m_pActSettings, &QAction::triggered, this, &MainWindow::onSettings);
    menuPreferences->addAction(m_pActSettings);

    m_pActAbout = new QAction(tr("About"), menuAbout);
    m_pActDonate = new QAction(tr("Donate"), menuAbout);
    connect(m_pActAbout, &QAction::triggered, this, &MainWindow::onActionAbout);
    connect(m_pActDonate, &QAction::triggered, this, &MainWindow::onActionDonate);
    menuAbout->addAction(m_pActAbout);
    menuAbout->addSeparator();
    menuAbout->addAction(m_pActDonate);

    //********** Table menu actions ****************//
    ui->tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    m_pItemMenu = new QMenu(this);
    m_pItemMenu->addAction(m_pActRemoveFile);
    m_pItemMenu->addSeparator();
    m_pItemMenu->addAction(m_pActRemoveAllFiles);
    m_pItemMenu->addSeparator();
    m_pItemMenu->addAction(m_pActStart);
    m_pItemMenu->addSeparator();
    m_pItemMenu->addAction(m_pActEditMetadata);
    m_pItemMenu->addSeparator();
    m_pItemMenu->addAction(m_pActSelectAudio);
    m_pItemMenu->addAction(m_pActSelectSubtitles);
    m_pItemMenu->addSeparator();
    m_pItemMenu->addAction(m_pActDeselectAudio);
    m_pItemMenu->addAction(m_pActDeselectSubtitles);
    m_pItemMenu->addSeparator();
    m_pItemMenu->addAction(m_pActSplitVideo);
    connect(ui->tableWidget, &QTableWidget::customContextMenuRequested, this, &MainWindow::provideContextMenu);
    
    // Setup header context menu for column visibility
    ui->tableWidget->horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableWidget->horizontalHeader(), &QHeaderView::customContextMenuRequested, 
            this, &MainWindow::provideHeaderContextMenu);

    //********** File Browser actions **************//
    ui->listFiles->setContextMenuPolicy(Qt::CustomContextMenu);
    m_pFilesItemMenu = new QMenu(ui->listFiles);
    m_pActAddToTask = new QAction(tr("Add to task"), m_pFilesItemMenu);
    connect(m_pActAddToTask, &QAction::triggered, this, &MainWindow::onAddToTask);
    m_pFilesItemMenu->addAction(m_pActAddToTask);
    connect(ui->listFiles, &QListView::customContextMenuRequested, this, &MainWindow::provideListContextMenu);

    //*********** Tree menu actions ****************//
    ui->treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    auto *actAddSection = new QAction(tr("Add section"), this);
    auto *actAddPreset = new QAction(tr("Add preset"), this);
    auto *actRenamePreset = new QAction(tr("Rename"), this);
    auto *actRemovePreset = new QAction(tr("Remove"), this);
    auto *actApplyPreset = new QAction(tr("Apply"), this);
    auto *actEditPreset = new QAction(tr("Edit"), this);
    connect(actAddSection, &QAction::triggered, this, &MainWindow::onAddSection);
    connect(actAddPreset, &QAction::triggered, this, &MainWindow::onAddPreset);
    connect(actRenamePreset, &QAction::triggered, this, &MainWindow::onRenamePreset);
    connect(actRemovePreset, &QAction::triggered, this, &MainWindow::onRemovePreset);
    connect(actApplyPreset, &QAction::triggered, this, &MainWindow::onApplyPreset);
    connect(actEditPreset, &QAction::triggered, this, &MainWindow::onEditPreset);

    m_pSectionMenu = new QMenu(this);
    m_pSectionMenu->addAction(actAddSection);
    m_pSectionMenu->addAction(actAddPreset);
    m_pSectionMenu->addAction(actRenamePreset);
    m_pSectionMenu->addAction(actRemovePreset);

    m_pPresetMenu = new QMenu(this);
    m_pPresetMenu->addAction(actAddSection);
    m_pPresetMenu->addAction(actAddPreset);
    m_pPresetMenu->addAction(actRenamePreset);
    m_pPresetMenu->addAction(actRemovePreset);
    m_pPresetMenu->addSeparator();
    m_pPresetMenu->addAction(actApplyPreset);
    m_pPresetMenu->addAction(actEditPreset);
    connect(ui->treeWidget, &QTreeWidget::customContextMenuRequested, this, &MainWindow::providePresetContextMenu);

    //********** Preset menu actions ***************//
    auto *addPresetMenu = new QMenu(ui->addPreset);
    auto *_actAddSection = new QAction(tr("Add section"), this);
    auto *_actAddPreset = new QAction(tr("Add new preset"), this);
    _actAddSection->setIcon(QIcon(":/resources/icons/svg/folder_light.svg"));
    _actAddPreset->setIcon(QIcon(":/resources/icons/svg/file.svg"));
    connect(_actAddSection, &QAction::triggered, this, &MainWindow::onAddSection);
    connect(_actAddPreset, &QAction::triggered, this, &MainWindow::onAddPreset);
    addPresetMenu->addAction(_actAddSection);
    addPresetMenu->addSeparator();
    addPresetMenu->addAction(_actAddPreset);
    ui->addPreset->setMenu(addPresetMenu);

    //******* Metadata Elements Actions ************//
    auto connectAction = [this](QLineEdit* line, bool isVisible)->void {
        auto actionList = line->findChildren<QAction*>();
        if (!actionList.isEmpty()) {
            connect(actionList.first(), &QAction::triggered, this, [this, line]() {
                line->clear();
                line->insert("");
                line->setModified(true);
                ui->frameMiddle->setFocus();
            });
        }
        if (!isVisible) {
            line->setEnabled(true);
            line->setVisible(false);
        }
    };
    QList<QLineEdit*> videoMetadata = {
        ui->lineVTitle,  ui->lineVName,  ui->lineVYear,
        ui->lineVAuthor, ui->lineVPerf,  ui->lineVDescript
    };
    foreach (QLineEdit *lineEdit, videoMetadata)
        connectAction(lineEdit, true);

    for (int i = VIDEO_TITLE; i < VIDEO_DESCRIPTION + 1; i++) {
        Q_ASSERT(i < 6);
        connect(videoMetadata[i], &QLineEdit::editingFinished, this, [=](){
            if (m_row != -1) {
                if (videoMetadata[i]->isModified()) {
                    videoMetadata[i]->setModified(false);
                    const QString text = videoMetadata[i]->text();
                    m_data[m_row].videoMetadata[VIDEO_TITLE + i] = text;
                }
            }
        });
    }

    //******** Audio Elements **************//
    m_pAudioLabel->setVisible(true);
    //******* Subtitle Elements ************//
    m_pSubtitleLabel->setVisible(true);
}

void MainWindow::setParameters()    // Set parameters
{
    createConnections();
    m_newParams.resize(PARAMETERS_COUNT);
    m_curParams.resize(PARAMETERS_COUNT);

    m_data.clear();
    m_reportLog.clear();

    //************** File Browser ******************//
    m_pSpl = new QSplitter(Qt::Horizontal, ui->browserWidget);
    ui->browserWidget->layout()->addWidget(m_pSpl);
    m_pSpl->setHandleWidth(0);
    m_pSpl->addWidget(ui->dirsWidget);
    m_pSpl->addWidget(ui->filesWidget);

    auto *model_d = new QStandardItemModel(this);
    model_d->setHorizontalHeaderItem(0, new QStandardItem(tr("Folders")));
    auto *hv_d = new QHeaderView(Qt::Horizontal, ui->dirsWidget);
    QFont fnt = hv_d->font();
    fnt.setItalic(true);
    fnt.setBold(true);
    hv_d->setFont(fnt);
    hv_d->setFixedHeight(28 * Helper::scaling());
    hv_d->setModel(model_d);
    hv_d->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->dirsLayout->addWidget(hv_d);

    auto *model_f = new QStandardItemModel(this);
    model_f->setHorizontalHeaderItem(0, new QStandardItem(tr("Files")));
    auto *hv_f = new QHeaderView(Qt::Horizontal, ui->filesWidget);
    hv_f->setFont(fnt);
    hv_f->setFixedHeight(28 * Helper::scaling());
    hv_f->setModel(model_f);
    hv_f->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->filesLayout->addWidget(hv_f);
    setBrowser();
    auto *fip = new FileIconProvider();
    m_pDirModel->setIconProvider(fip);
    ui->lineEditFileFilter->hide();

    //************** Combo boxes *******************//
    auto comboBoxes = findChildren<QComboBox*>();
    foreach (auto combo, comboBoxes) {
        auto *_view = new QListView(combo);
        _view->setTextElideMode(Qt::ElideMiddle);
        combo->setView(_view);
    }
    ui->comboBoxPreset->setVisible(false);

    //********** Set default state *****************//
    m_pAnimation = new QAnimatedSvg(ui->labelAnimation, QSize(18, 18) * Helper::scaling());
    setProgressEnabled(false);
    m_pTableLabel->show();
    m_pAudioLabel->show();
    m_pSubtitleLabel->show();

    //************ Create folders ******************//
    if (!QDir(SETTINGSPATH).exists()) {
        QDir().mkdir(SETTINGSPATH);
        Print("Setting path did not exist, so was created ...");
    }
    if (QDir(THUMBNAILPATH).exists()) {
        unsigned int count_thumb = QDir(THUMBNAILPATH).count();
        if (count_thumb > 300) {
            QDir(THUMBNAILPATH).removeRecursively();
            Print("Thumbnails removed... ");
        }
    }
    if (!QDir(THUMBNAILPATH).exists()) {
        QDir().mkdir(THUMBNAILPATH);
        Print("Thumbnail path did not exist, so was created ...");
    }

    //************** Read presets ******************//
    bool validXmlFile = false;
    validXmlFile = readXMLPresetFile(XMLPRESETFILE);
    if (!validXmlFile)
    {
        setDefaultPresets();
    }

    //*********** Table parameters ****************//
    ui->tableWidget->setRowCount(0);
    ui->tableWidget->setShowGrid(false);
    ui->tableWidget->horizontalHeader()->setFont(fnt);
    ui->tableWidget->horizontalHeader()->setVisible(true);
    ui->tableWidget->verticalHeader()->setVisible(true);
    ui->tableWidget->verticalHeader()->setDefaultSectionSize(ROWHEIGHT * Helper::scaling());
    ui->tableWidget->setAlternatingRowColors(true);
    ui->tableWidget->setDropIndicatorShown(true);
    ui->tableWidget->setDragEnabled(true);
    ui->tableWidget->setDragDropOverwriteMode(true);
    ui->tableWidget->setDragDropMode(QAbstractItemView::DropOnly);
    ui->tableWidget->setDefaultDropAction(Qt::TargetMoveAction);
    ui->tableWidget->setColumnWidth(ColumnIndex::FILENAME, 350); // Make wider to accommodate path + filename
    ui->tableWidget->setColumnWidth(ColumnIndex::FORMAT, 80);
    ui->tableWidget->setColumnWidth(ColumnIndex::RESOLUTION, 85);
    ui->tableWidget->setColumnWidth(ColumnIndex::DURATION, 70);
    ui->tableWidget->setColumnWidth(ColumnIndex::FPS, 70);
    ui->tableWidget->setColumnWidth(ColumnIndex::AR, 60);
    ui->tableWidget->setColumnWidth(ColumnIndex::STATUS, 80);
    ui->tableWidget->setColumnWidth(ColumnIndex::PRESET_COL, 120);
    ui->tableWidget->setIconSize(QSize(16, 16) * Helper::scaling());

    for (int i = ColumnIndex::COLORRANGE; i <= ColumnIndex::MAXFALL; i++)
        ui->tableWidget->setColumnWidth(i, 82);

    for (int i = ColumnIndex::BITRATE; i <= ColumnIndex::COLORSPACE; i++)
        ui->tableWidget->hideColumn(i);

    for (int i = ColumnIndex::T_DUR; i <= ColumnIndex::T_ID; i++)
        ui->tableWidget->hideColumn(i);
    
    // Hide PATH column since filename now shows full path
    ui->tableWidget->hideColumn(ColumnIndex::PATH);

    //************* Read settings ******************//
    QList<int> dockSizesX{};
    QList<int> dockSizesY{};
    SETTINGS(stn);
    stn.beginGroup("Settings");
    stn.remove("");
    stn.endGroup();
    if (stn.value("Version").toInt() == SETTINGS_VERSION) {
        // Restore Window
        stn.beginGroup("Window");
        restoreGeometry(stn.value("Window/geometry").toByteArray());
        stn.endGroup();
        // Restore DocksContainer
        stn.beginGroup("DocksContainer");
        m_pDocksContainer->restoreState(stn.value("DocksContainer/state").toByteArray());
        int arraySize = stn.beginReadArray("DocksContainer/docks_geometry");
        for (int i = 0; i < arraySize && i < DOCKS_COUNT; i++) {
            stn.setArrayIndex(i);
            QSize size = stn.value("DocksContainer/docks_geometry/dock_size").toSize();
            dockSizesX.append(size.width());
            dockSizesY.append(size.height());
        }
        stn.endArray();
        stn.endGroup();
        // Restore Tables
        stn.beginGroup("Tables");
        ui->tableWidget->horizontalHeader()->restoreState(stn.value("Tables/table_widget_state").toByteArray());
        ui->treeWidget->header()->restoreState(stn.value("Tables/tree_widget_state").toByteArray());
        m_pSpl->restoreState(stn.value("Tables/splitter_state").toByteArray());
        m_pSplSource->restoreState(stn.value("Tables/splitter_source_state").toByteArray());
        stn.endGroup();
    } else {
        const QRect scr_rect = QApplication::primaryScreen()->availableGeometry();
        const QPoint topLeft = scr_rect.translated(100,100).topLeft();
        const QRect rect(topLeft, WINDOW_SIZE);
        setGeometry(rect);

        ui->treeWidget->setColumnWidth(0, 230);
        ui->treeWidget->setColumnWidth(1, 55);
        ui->treeWidget->setColumnWidth(2, 55);
        ui->treeWidget->setColumnWidth(3, 55);
        ui->treeWidget->setColumnWidth(4, 55);
        ui->treeWidget->setColumnWidth(5, 55);
        ui->treeWidget->setColumnWidth(6, 55);
    }

    // Defaults, to then be stomped over by the settings file.
    m_prefxType = 0;
    m_suffixType = 0;
    m_prefixName = "";
    m_suffixName = "";
    m_timerInterval = 30;
    m_theme = 0;
    m_protectFlag = false;
    m_multiInstances = false;
    m_showHdrFlag = false;
    m_temp_folder = "";
    m_output_folder = "";
    m_batch_mode = false;
    m_hideInTrayFlag = false;
    m_language = nullptr;
    m_font = "";
    m_fontSize = 8;
    m_rowHeight = 0;
    m_subtitles_font = "";
    m_subtitles_fontSize = 8;
    m_subtitles_color = 0xffffff;
    m_subtitles_background = false;
    m_subtitles_deselectall = true;
    m_subtitles_background_alpha = 150;
    m_subtitles_background_color = "#000000";
    m_subtitles_location = 0;
    m_threads = 0;
    m_ffmpeg_prio = Constants::normal;

    // Read settings from XML, to overwrite defaults above as-found.
    readXMLSettingsFile(XMLSETTINGSFILE);

    //*********** Preset parameters ****************//
    ui->treeWidget->clear();
    ui->treeWidget->header()->setFont(fnt);
    ui->treeWidget->setHeaderHidden(false);
    ui->treeWidget->setAlternatingRowColors(true);
    auto NUM_ROWS = m_preset_table[0].size();
    auto NUM_COLUMNS = m_preset_table.size();
    QString type;
    QFont parentFont;
    parentFont.setBold(true);
    parentFont.setItalic(true);
    for (int i = 0; i < NUM_ROWS; i++) {
        type = m_preset_table[PARAMETERS_COUNT][i];
        // Fix for typo in name within previous code.
        if ((type == "TopLewelItem") || (type == "TopLevelItem")) {
            auto *root = new QTreeWidgetItem();
            root->setText(0, m_preset_table[0][i]);
            root->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
            root->setFont(0, parentFont);
            setPresetIcon(root, true);
            ui->treeWidget->addTopLevelItem(root);
            ui->treeWidget->setCurrentItem(root);
            root->setFirstColumnSpanned(true);
        }
        if (type == "ChildItem") {
            auto *item = ui->treeWidget->currentItem();
            auto *child = new QTreeWidgetItem();
            for (int j = 0; j < PARAMETERS_COUNT; j++) {
                child->setText(j + 7, m_preset_table[j][i]);
            }
            QString savedPresetName = child->text(30 + 7);
            child->setText(0, savedPresetName);

            updateInfoFields(m_preset_table[1][i],
                             m_preset_table[2][i],
                             m_preset_table[3][i],
                             m_preset_table[4][i],
                             m_preset_table[11][i],
                             m_preset_table[12][i],
                             m_preset_table[21][i],
                             child, false);
            setItemStyle(child);
            item->addChild(child);
        }
    }
    if (m_pos_top != -1 && m_pos_cld != -1) {
        QTreeWidgetItem *item = ui->treeWidget->topLevelItem(m_pos_top)->child(m_pos_cld);
        ui->treeWidget->setCurrentItem(item);
    }
    // Print(NUM_ROWS << " x " << NUM_COLUMNS);
    for (int i = 7; i < 41; i++)
        ui->treeWidget->hideColumn(i);

    //*********** Other parameters *****************//
    if (dockSizesX.count() < DOCKS_COUNT || dockSizesY.count() < DOCKS_COUNT) {
        float coeffX[DOCKS_COUNT] = {0.39f, 0.39f, 0.61f, 0.23f, 0.23f, 0.23f, 0.23f, 0.39f};
        float coeffY[DOCKS_COUNT] = {0.16f, 0.5f, 0.1f, 0.9f, 0.9f, 0.9f, 0.9f, 0.3f};
        for (int i = 0; i < DOCKS_COUNT; i++) {
            const int dockWidth = static_cast<int>(coeffX[i] * WINDOW_SIZE.width());
            const int dockHeight = static_cast<int>(coeffY[i] * WINDOW_SIZE.height());
            dockSizesX.append(dockWidth);
            dockSizesY.append(dockHeight);
        }
    }
    QTimer::singleShot(200, this, [this, dockSizesX, dockSizesY](){
        setDocksParameters(dockSizesX, dockSizesY);
    });

    if (m_timerInterval < 15)
        m_timerInterval = 30;
    m_pTimer->setInterval(m_timerInterval*1000);

    if (m_rowHeight != 0)
        ui->sliderResize->setValue(m_rowHeight);

    m_expandWindowsState = isMaximized();

    if (m_batch_mode) {
        ui->comboBoxMode->blockSignals(true);
        ui->comboBoxMode->setCurrentIndex(1);
        ui->comboBoxMode->blockSignals(false);
    }
    setTrayIcon();
    if (m_hideInTrayFlag)
        m_pTrayIcon->show();
    if (m_theme > 1)
        m_theme = 1;
    setTheme(m_theme);
    // Print("Desktop env.: " << short(Helper::getEnv()));

    ui->treeDirs->setRootIndex(m_pDirModel->setRootPath(m_openDir));
    ui->listFiles->setRootIndex(m_pFileModel->setRootPath(m_openDir));

    ui->comboBox_changePrio->setCurrentIndex(m_ffmpeg_prio);
}

void MainWindow::readXMLSettingsFile(const QString& xmlFileName)
{
    int switchViewMode = 0;
    int switchCutMode = 0;

    QFile xmlFile(xmlFileName);
    bool settingsXMLFileValid = true;
    int settingsVer = 0;
    if (!xmlFile.open(QFile::ReadOnly | QFile::Text)) { // Open file in write only mode
        settingsXMLFileValid = false;
    }

    if (settingsXMLFileValid) {
        settingsXMLFileValid = false;
        QXmlStreamReader stream(&xmlFile);
        stream.readNextStartElement();
        if (stream.name().toString() == "cineencoder") {
            stream.readNextStartElement();
            if (stream.name().toString() == "version") {
                settingsVer = stream.readElementText().toInt();
                stream.readNextStartElement();
                if (stream.name().toString() == "settings") {
                    settingsXMLFileValid = true;
                }
            }
        }
        if (settingsXMLFileValid) {
            while (!stream.atEnd()) {
                stream.readNextStartElement();
                const QString tagName = stream.name().toString();
                if (tagName == "prefix_type") {
                    const QString value = stream.readElementText();
                    m_prefxType = value.toInt();
                }
                if (tagName == "suffix_type") {
                    const QString value = stream.readElementText();
                    m_suffixType = value.toInt();
                }
                if (tagName == "prefix_name") {
                    m_prefixName = stream.readElementText();
                }
                if (tagName == "suffix_name") {
                    m_suffixName = stream.readElementText();
                }
                if (tagName == "timer_interval") {
                    const QString value = stream.readElementText();
                    m_timerInterval = value.toInt();
                }
                if (tagName == "theme") {
                    const QString value = stream.readElementText();
                    m_theme = value.toInt();
                }
                if (tagName == "protection") {
                    const QString value = stream.readElementText();
                    m_protectFlag = value.toInt();
                }
                if (tagName == "allow_duplicates") {
                    const QString value = stream.readElementText();
                    m_multiInstances = value.toInt();
                }
                if (tagName == "show_hdr_mode") {
                    const QString value = stream.readElementText();
                    m_showHdrFlag = value.toInt();
                }
                if (tagName == "temp_folder") {
                    m_temp_folder = stream.readElementText();
                }
                if (tagName == "output_folder") {
                    m_output_folder = stream.readElementText();
                }
                if (tagName == "open_dir") {
                    m_openDir = stream.readElementText();
                }
                if (tagName == "batch_mode") {
                    const QString value = stream.readElementText();
                    m_batch_mode = value.toInt();
                }
                if (tagName == "tray") {
                    const QString value = stream.readElementText();
                    m_hideInTrayFlag = value.toInt();
                }
                if (tagName == "language") {
                    m_language = stream.readElementText();
                }
                if (tagName == "font") {
                    m_font = stream.readElementText();
                }
                if (tagName == "font_size") {
                    const QString value = stream.readElementText();
                    m_fontSize = value.toInt();
                }
                if (tagName == "subtitles_font") {
                    m_subtitles_font = stream.readElementText();
                }
                if (tagName == "subtitles_font_size") {
                    const QString value = stream.readElementText();
                    m_subtitles_fontSize = value.toInt();
                }
                if (tagName == "subtitles_deselectall") {
                    const QString value = stream.readElementText();
                    m_subtitles_deselectall = value.toInt();
                }
                if (tagName == "subtitles_background") {
                    const QString value = stream.readElementText();
                    m_subtitles_background = value.toInt();
                }
                if (tagName == "subtitles_color") {
                    m_subtitles_color = stream.readElementText();
                }
                if (tagName == "subtitles_background_color") {
                    m_subtitles_background_color = stream.readElementText();
                }
                if (tagName == "subtitles_background_alpha") {
                    const QString value = stream.readElementText();
                    m_subtitles_background_alpha = value.toInt();
                }
                if (tagName == "subtitles_location") {
                    const QString value = stream.readElementText();
                    m_subtitles_location = value.toInt();
                }
                if (tagName == "threads") {
                    const QString value = stream.readElementText();
                    m_threads = value.toInt();
                }
                if (tagName == "ffmpeg_prio") {
                    const QString value = stream.readElementText();
                    int tmp = value.toInt();
                    if (tmp > Constants::highest)
                    {
                        tmp = Constants::highest;
                    }
                    if (tmp < Constants::lowest)
                    {
                        tmp = Constants::lowest;
                    }
                    m_ffmpeg_prio = tmp;
                }
                if (tagName == "row_size") {
                    const QString value = stream.readElementText();
                    m_rowHeight = value.toInt();
                }
                if (tagName == "switch_view_mode") {
                    const QString value = stream.readElementText();
                    switchViewMode = value.toInt();
                }
                if (tagName == "switch_cut_mode") {
                    const QString value = stream.readElementText();
                    switchCutMode = value.toInt();
                }
            }
            xmlFile.close();
        }
    }
    ui->switchViewMode->setCurrentIndex(switchViewMode);
    ui->switchCutting->setCurrentIndex(switchCutMode);
}

void MainWindow::setDocksParameters(const QList<int>& dockSizesX, const QList<int>& dockSizesY)
{
    QList<QDockWidget*> docksVis;
    QList<int> dockVisSizesX;
    QList<int> dockVisSizesY;
    for (int i = 0; i < DOCKS_COUNT; i++) {
        if (m_pDocks[i]->isVisible() && !m_pDocks[i]->isFloating()){
            docksVis.append(m_pDocks[i]);
            dockVisSizesX.append(dockSizesX.at(i));
            dockVisSizesY.append(dockSizesY.at(i));
        }
    }
    m_pDocksContainer->resizeDocks(docksVis, dockVisSizesX, Qt::Horizontal);
    m_pDocksContainer->resizeDocks(docksVis, dockVisSizesY, Qt::Vertical);
    for (int i = 0; i < DOCKS_COUNT; i++) {
        if (m_pDocks[i]->isVisible() && m_pDocks[i]->isFloating()){
            m_pDocks[i]->setFloating(false); // Bypassing the error with detached docks in some Linux distributions
            m_pDocks[i]->setFloating(true);
        }
    }
}

int MainWindow::doesParamsContain(const QString& findMe)
{
    int index = -1;
    for (int i = 0; i < PARAMETERS_COUNT; i++)
    {
        if (param_names[i] == findMe)
        {
            index = i;
            break;
        }
    }

    return index;
}

bool MainWindow::readXMLPresetFile(const QString& file)
{
    const bool debug = false;
    m_preset_table.clear();
    bool validXmlFile = false;
    QFile xmlFile(file);
    int presets_added = 0;
    if (!xmlFile.open(QFile::ReadOnly | QFile::Text)) { // Open file in write only mode
        return validXmlFile;
    }
    QXmlStreamReader stream(&xmlFile);
    stream.readNextStartElement();
    // Check we have a cineencoder XML file.
    if (stream.name().toString() == QString("cineencoder")) {
        // Check our version.
        stream.readNextStartElement();
        if (stream.name().toString() != QString("version")) {
            return validXmlFile;
        }
        validXmlFile = true;
        auto version_from_xml = stream.readElementText();
        // Don't actually use the version, but read it in case compatibility shims needed later.

        if (validXmlFile) {
            QList<QString> ptable_list[PARAMETERS_COUNT + 1];

            while (!stream.atEnd()) {
                stream.readNextStartElement();
                QString nnn = stream.name().toString();
                std::string sss = nnn.toStdString();
                if (nnn == QString("m_pos_top")) {
                    QString val = stream.readElementText();
                    m_pos_top = val.toInt();
                }
                if (nnn == QString("m_pos_cld")) {
                    QString val = stream.readElementText();
                    m_pos_cld = val.toInt();
                }
                if (nnn == QString("params")) {
                    while (stream.readNextStartElement()) {
                        QString nnn = stream.name().toString();
                        QString val = stream.readElementText();
                        // Do we have this parameter name in our supported list?
                        int index = doesParamsContain(nnn);
                        if (index != -1) {
                            m_curParams[index] = val;
                        }
                    }
                }
                if (nnn == QString("presettable")) {
                    // Each preset is a start element
                    while (stream.readNextStartElement()) {
                        // Set up our defaults.
                        QString parameters[PARAMETERS_COUNT + 1];
                        for (int p = 0; p < PARAMETERS_COUNT; p++) {
                            parameters[p] = default_preset[p];
                        }
                        parameters[PARAMETERS_COUNT] = "ChildItem";
                        while (stream.readNextStartElement()) {
                            QString nnn = stream.name().toString();
                            QString val = stream.readElementText();
                            // Do we have this parameter name in our supported list?
                            int index = doesParamsContain(nnn);

                            if (index != -1) {
                                parameters[index] = val;
                            } else {
                                if (stream.name().toString() == "TYPE") {
                                    parameters[PARAMETERS_COUNT] = val;
                                }
                            }
                        }

                        // Use our parameters to extend our list of values.
                        for (int p = 0; p < PARAMETERS_COUNT + 1; p++) {
                            ptable_list[p].push_back(parameters[p]);
                        }
                        presets_added++;

                    }
                }
            }

            // Dump the table for review.
            if (debug) {
                std::list<std::list<std::string>> list_of_list_of_strings;
                for (auto & pp : ptable_list) {
                    std::list<std::string> list_of_strings;
                    for (int pr = 0; pr < presets_added; pr++) {
                        list_of_strings.push_back(pp[pr].toStdString());
                    }

                    list_of_list_of_strings.push_back(list_of_strings);
                }
            }

            for (auto & i : ptable_list)
            {
                QList<QString> parlist;
                for (int j = 0; j < presets_added; j++)
                {
                    QString sss = i[j];
                    parlist.append(sss);
                }
                m_preset_table.append(parlist);
            }
            xmlFile.close();
        }
    }
    if (debug) {
        std::list<std::list<std::string>> m_preset_table_actual;
        for (int outer = 0; outer < m_preset_table.count(); outer++) {
            std::list<std::string> tmp;
            for (int inner = 0; inner < m_preset_table[outer].count(); inner++) {
                tmp.push_back(m_preset_table[outer][inner].toStdString());
            }
            m_preset_table_actual.push_back(tmp);
        }
    }
    return validXmlFile;
}

void MainWindow::onCloseWindow()    // Close window
{
    this->close();
}

void MainWindow::setExpandIcon()
{
    ui->expandWindow->setProperty("expanded", isMaximized());
    ui->expandWindow->style()->polish(ui->expandWindow);
}

void MainWindow::onHideWindow()    // Hide window
{
    m_hideInTrayFlag ? hide() : showMinimized();
}

void MainWindow::onReport()
{
    auto *rpt = new Report(this, m_reportLog);
    rpt->show();
}

void MainWindow::onRestoreWindowState()
{
    m_expandWindowsState ? showMaximized() : showNormal();
}

void MainWindow::onActionAbout()   // About
{
    Notification about(this, MessConf::CloseOnly, tr("ABOUT"));
    about.exec();
}

void MainWindow::onActionDonate()   // Donate
{
    Notification donate(this, MessConf::AllBtns, tr("DONATE"));
    donate.exec();
}

void MainWindow::onSettings()
{
    Settings settings(this);
    settings.setParameters(&m_output_folder,
                           &m_temp_folder,
                           &m_protectFlag,
                           &m_multiInstances,
                           &m_showHdrFlag,
                           &m_timerInterval,
                           &m_threads,
                           &m_ffmpeg_prio,
                           &m_theme,
                           &m_prefixName,
                           &m_suffixName,
                           &m_prefxType,
                           &m_suffixType,
                           &m_hideInTrayFlag,
                           &m_language,
                           &m_fontSize,
                           &m_font,
                           &m_subtitles_fontSize,
                           &m_subtitles_font,
                           &m_subtitles_deselectall,
                           &m_subtitles_background,
                           &m_subtitles_color,
                           &m_subtitles_background_color,
                           &m_subtitles_background_alpha,
                           &m_subtitles_location);
    if (settings.exec() == Dialog::Accept) {
        m_pTimer->setInterval(m_timerInterval*1000);
        setTheme(m_theme);
        m_hideInTrayFlag ? m_pTrayIcon->show() : m_pTrayIcon->hide();
        if (m_row != -1)
            get_output_filename();
        if (settings.restart_needed) {
            showPopup(tr("You need to restart the program for the settings to take effect."),
                      PopupMessage::Icon::Warning);
        }
    }
}

void MainWindow::setFloating(const int index, const QPoint &offset, const QSize &size)
{
    if (!m_pDocks[index]->isVisible())
        m_pDocks[index]->setVisible(true);
    m_pDocks[index]->setFloating(true);
    m_pDocks[index]->move(this->geometry().center() + offset);
    m_pDocks[index]->resize(size * Helper::scaling());
}

void MainWindow::showMetadataEditor()
{
    setFloating(DockIndex::METADATA_DOCK, QPoint(200,-200), QSize(300,500));
}

void MainWindow::showAudioStreams()
{
    setFloating(DockIndex::STREAMS_DOCK, QPoint(220,-180), QSize(300,500));
    ui->tabWidgetStreams->setCurrentIndex(0);
}

void MainWindow::showSubtitles()
{
    setFloating(DockIndex::STREAMS_DOCK, QPoint(220,-180), QSize(300,500));
    ui->tabWidgetStreams->setCurrentIndex(1);
}

void MainWindow::clearAudioStreams()
{
    ui->streamAudio->deselectTitles();
}

void MainWindow::clearSubtitleStreams()
{
    ui->streamSubtitle->deselectTitles();
}

void MainWindow::showVideoSplitter()
{
    setFloating(DockIndex::SPLIT_DOCK, QPoint(240,-160), QSize(400,395));
}

void MainWindow::get_current_data() // Get current data
{
    m_dur = GETTEXT(m_row, T_DUR).toDouble();
    m_stream_size = GETTEXT(m_row, T_STREAMSIZE);
    m_width = GETTEXT(m_row, T_WIDTH);
    m_height = GETTEXT(m_row, T_HEIGHT);
    m_fmt = GETTEXT(m_row, FORMAT);
    m_fps = GETTEXT(m_row, FPS);

    const double fps_double = m_fps.toDouble();
    m_fr_count = static_cast<int>(round(m_dur * fps_double));
    m_startTime =  GETTEXT(m_row, T_STARTTIME).toDouble();
    m_endTime = GETTEXT(m_row, T_ENDTIME).toDouble();

    QString curCodec = m_fmt;
    QString curFps = m_fps;
    QString curRes = GETTEXT(m_row, RESOLUTION);
    QString curAr = GETTEXT(m_row, AR);
    QString curBitrate = GETTEXT(m_row, BITRATE);
    QString curColorSampling = GETTEXT(m_row, SUBSAMPLING);
    QString curDepth = GETTEXT(m_row, BITDEPTH);
    QString curSpace = GETTEXT(m_row, COLORSPACE);

    // FILENAME now contains the full path, extract path and filename separately
    QString fullPath = GETTEXT(m_row, FILENAME);
    QFileInfo fileInfo(fullPath);
    m_curPath = fileInfo.absolutePath();
    m_curFilename = fileInfo.fileName();
    m_input_file = fullPath;
    m_hdr[CUR_COLOR_RANGE] = GETTEXT(m_row, COLORRANGE);
    m_hdr[CUR_COLOR_PRIMARY] = GETTEXT(m_row, COLORPRIM);
    m_hdr[CUR_COLOR_MATRIX] = GETTEXT(m_row, COLORMATRIX);
    m_hdr[CUR_TRANSFER] = GETTEXT(m_row, TRANSFER);
    m_hdr[CUR_MAX_LUM] = GETTEXT(m_row, MAXLUM);
    m_hdr[CUR_MIN_LUM] = GETTEXT(m_row, MINLUM);
    m_hdr[CUR_MAX_CLL] = GETTEXT(m_row, MAXCLL);
    m_hdr[CUR_MAX_FALL] = GETTEXT(m_row, MAXFALL);
    m_hdr[CUR_MASTER_DISPLAY] = GETTEXT(m_row, MASTERDISPLAY);
    m_hdr[CUR_CHROMA_COORD] = GETTEXT(m_row, T_CHROMACOORD);
    m_hdr[CUR_WHITE_COORD] = GETTEXT(m_row, T_WHITECOORD);

    //*************** Set icons ********************//
    double halfTime = m_dur/2;
    QString tmb_file = setThumbnail(m_curFilename, halfTime, PreviewRes::RES_HIGH, PreviewDest::PREVIEW);
    QSize imageSize = QSize(85, 48) * Helper::scaling();
    if (m_rowHeight == ROWHEIGHT) {
        const QString icons[4][5] = {
            {"cil-hdr",       "cil-camera-roll",       "cil-hd",       "cil-4k",       "cil-file"},
            {"cil-hdr",       "cil-camera-roll",       "cil-hd",       "cil-4k",       "cil-file"},
            {"cil-hdr",       "cil-camera-roll",       "cil-hd",       "cil-4k",       "cil-file"},
            {"cil-hdr_black", "cil-camera-roll_black", "cil-hd_black", "cil-4k_black", "cil-file_black"}
        };
        int type = 4;
        if (m_hdr[CUR_MASTER_DISPLAY] != "")
            type = 0;
        else
        if (m_height.toInt() >=1 && m_height.toInt() < 720)
            type = 1;
        else
        if (m_height.toInt() >= 720 && m_height.toInt() < 2160)
            type = 2;
        else
        if (m_height.toInt() >= 2160)
            type = 3;

        tmb_file = QString(":/resources/icons/16x16/%1.png").arg(icons[m_theme][type]);
        imageSize = QSize(16, 16) * Helper::scaling();
    }
    QPixmap pxmp(tmb_file);
    QPixmap scaled = pxmp.scaled(imageSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    ui->tableWidget->item(m_row, ColumnIndex::FILENAME)->setIcon(QIcon(scaled));

    //*********** Set description ******************//
    ui->label_source->setText(m_curFilename);
    if (curCodec != "")
        curCodec += ", ";

    if (curRes != "")
        curRes += ", ";

    if (curFps != "")
        curFps += " fps, ";

    if (curAr != "")
        curAr += ", ";

    if (curSpace != "")
        curSpace += ", ";

    if (curColorSampling != "")
        curColorSampling += ", ";

    if (curDepth != "")
        curDepth += QString(" %1, ").arg(tr("bit"));

    if (curBitrate != "")
        curBitrate += QString(" %1").arg(tr("kbps"));

    QString sourceParam = QString("%1: %2").arg(tr("Video"), curCodec + curRes +
                                                curFps + curAr + curSpace +
                                                curColorSampling + curDepth + curBitrate);

    for (int i = 0; i < m_data[m_row].fields[Data::audioFormats].size(); i++) {
        const QString audioFormat = m_data[m_row].fields[Data::audioFormats][i];
        const QString audioLang = m_data[m_row].fields[Data::audioLangs][i];
        const QString audioCh = Helper::recalcChannels(m_data[m_row].fields[Data::audioChannels][i]) + " ch";
        sourceParam += QString("\n%1 %2: %3, %4, %5").arg(tr("Audio"), numToStr(i + 1),
                                                   audioFormat, audioCh, audioLang);
    }

    for (int i = 0; i < m_data[m_row].fields[Data::subtFormats].size(); i++) {
        const QString subtFormat = m_data[m_row].fields[Data::subtFormats][i];
        const QString subtLang = m_data[m_row].fields[Data::subtLangs][i];
        sourceParam += QString("\n%1 %2: %3, %4").arg(tr("Subtitle"), numToStr(i + 1),
                                                   subtFormat, subtLang);
    }

    if (curCodec == "Undef, ")
        sourceParam = tr("Undefined");

    ui->textBrowser_1->clear();
    ui->textBrowser_1->setText(sourceParam);
    get_output_filename();

    //*********** Set time widgets *****************//
    ui->sliderTimeline->setMaximum(m_fr_count);
    ui->lineEditStartTime->setText(Helper::timeConverter(m_startTime));
    ui->lineEditEndTime->setText(Helper::timeConverter(m_endTime));

    //********** Set video widgets *****************//
    auto linesEditMetadata = ui->frameTab_1->findChildren<QLineEdit *>();
    foreach (QLineEdit *lineEdit, linesEditMetadata)
        lineEdit->setEnabled(true);

    ui->lineVTitle->setText(m_data[m_row].videoMetadata[VIDEO_TITLE]);
    ui->lineVName->setText(m_data[m_row].videoMetadata[VIDEO_MOVIENAME]);
    ui->lineVYear->setText(m_data[m_row].videoMetadata[VIDEO_YEAR]);
    ui->lineVAuthor->setText(m_data[m_row].videoMetadata[VIDEO_AUTHOR]);
    ui->lineVPerf->setText(m_data[m_row].videoMetadata[VIDEO_PERFORMER]);
    ui->lineVDescript->setText(m_data[m_row].videoMetadata[VIDEO_DESCRIPTION]);

    foreach (QLineEdit *lineEdit, linesEditMetadata)
        lineEdit->setCursorPosition(0);

    //********** Set audio widgets *****************//
    if (!m_data[m_row].fields[Data::audioFormats].empty() ||
            !m_data[m_row].fields[Data::externAudioFormats].empty()) {
        m_pAudioLabel->setVisible(false);
        Tables t;
        int codecIndex = m_curParams[CurParamIndex::CODEC].toInt();
        int audioCodecIndex = m_curParams[CurParamIndex::AUDIO_CODEC].toInt();
        QString targetAudioCodec = t.arr_acodec[codecIndex][audioCodecIndex];
        bool usePresetSubtitleSettings = m_curParams[CurParamIndex::USE_PRESET_SUBTITLE_SETTINGS].toInt() == 1;
        ui->streamAudio->setList(extension, m_data[m_row], targetAudioCodec, usePresetSubtitleSettings);
    }

    //********* Set subtitle widgets ***************//
    if (!m_data[m_row].fields[Data::subtFormats].empty() ||
            !m_data[m_row].fields[Data::externSubtFormats].empty()) {
        m_pSubtitleLabel->setVisible(false);
        Tables t;
        int codecIndex = m_curParams[CurParamIndex::CODEC].toInt();
        int audioCodecIndex = m_curParams[CurParamIndex::AUDIO_CODEC].toInt();
        QString targetAudioCodec = t.arr_acodec[codecIndex][audioCodecIndex];
        bool usePresetSubtitleSettings = m_curParams[CurParamIndex::USE_PRESET_SUBTITLE_SETTINGS].toInt() == 1;
        ui->streamSubtitle->setList(extension, m_data[m_row], targetAudioCodec, usePresetSubtitleSettings);
    }
}

void MainWindow::setTheme(const int ind_theme)   // Set theme
{
    ui->frame_main->setProperty("scale", int(Helper::scaling() * 100));
    setStyleSheet(Helper::getCss(ind_theme));
    QString spinnerFile;
    switch (ind_theme) {
    case Theme::GRAY:
    case Theme::DARK:
    case Theme::WAVE:
        spinnerFile = ":/resources/icons/svg/spinner_black.svg";
        break;
    default:
    case Theme::LIGHT:
        spinnerFile = ":/resources/icons/svg/spinner.svg";
        break;
    }
    m_pAnimation->setFileName(spinnerFile);

    for (int i = ColumnIndex::COLORRANGE; i < ColumnIndex::MASTERDISPLAY + 1; i++) {
        m_showHdrFlag ? ui->tableWidget->showColumn(i) :
                        ui->tableWidget->hideColumn(i);
    }
    setExpandIcon();
}

void MainWindow::setStatus(const QString &status)
{
    auto *item = new QTableWidgetItem(status);
    item->setTextAlignment(Qt::AlignCenter);
    ui->tableWidget->setItem(m_row, ColumnIndex::STATUS, item);
}

void MainWindow::setWidgetsEnabled(bool state)    // Set widgets states
{
    ui->treeWidget->setEnabled(state);
    state ? m_pAnimation->stop() : m_pAnimation->start();
    m_pActAddFiles->setEnabled(state);
    m_pActRemoveFile->setEnabled(state);
    m_pActRemoveAllFiles->setEnabled(state);
    m_pActSettings->setEnabled(state);
    m_pActAddToTask->setEnabled(state);
    ui->lineEditCurTime->setEnabled(state);
    ui->lineEditStartTime->setEnabled(state);
    ui->lineEditEndTime->setEnabled(state);
    ui->framePrev->setEnabled(state);
    ui->frameNext->setEnabled(state);
    ui->sliderTimeline->setEnabled(state);
    ui->setStartTime->setEnabled(state);
    ui->setEndTime->setEnabled(state);
    ui->tableWidget->setEnabled(state);
    ui->sortUp->setEnabled(state);
    ui->sortDown->setEnabled(state);
    ui->addFiles->setEnabled(state);
    ui->removeFile->setEnabled(state);
    ui->removeAllFiles->setEnabled(state);

    ui->addPreset->setEnabled(state);
    ui->removePreset->setEnabled(state);
    ui->editPreset->setEnabled(state);
    ui->applyPreset->setEnabled(state);

    ui->comboBoxMode->setEnabled(state);
    ui->addFilesHot->setEnabled(state);
    ui->setOutFolder->setEnabled(state);
    ui->sliderResize->setEnabled(state);

    ui->clearMetadata->setEnabled(state);
    ui->undoMetadata->setEnabled(state);
    auto linesEditMetadata = ui->frameTab_1->findChildren<QLineEdit*>();
    foreach (QLineEdit *lineEdit, linesEditMetadata)
        lineEdit->setEnabled(state);

    ui->undoTitles->setEnabled(state);
    ui->addExtStream->setEnabled(state);
    ui->streamAudio->setEnabled(state);
    //ui->streamAudioExtern->setEnabled(state);
    ui->clearTitles->setEnabled(state);
    ui->streamSubtitle->setEnabled(state);

    ui->resetLabels->setEnabled(state);
    ui->settings->setEnabled(state);
    ui->filesWidget->setEnabled(state);

    if (state) {
        m_status_encode_btn = EncodingStatus::START;
        ui->start->setProperty("status", m_status_encode_btn);
        ui->start->style()->polish(ui->start);
        ui->start->setToolTip(tr("Encode"));
    }
}

void MainWindow::setProgressEnabled(bool state)
{
    ui->labelAnimation->setVisible(state);
    ui->label_Progress->setVisible(state);
    ui->label_Remaining->setVisible(state);
    ui->label_RemTime->setVisible(state);
    ui->progressBar->setVisible(state);
}

void MainWindow::changeEvent(QEvent *event)
{
    BaseWindow::changeEvent(event);
    if (event->type() == QEvent::WindowStateChange) {
        m_expandWindowsState = isMaximized();
        setExpandIcon();
    }
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        auto *keyEvent = dynamic_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return) {
            ui->frameMiddle->setFocus();
            return true;
        } else
            if (keyEvent->modifiers() == Qt::KeyboardModifier::AltModifier)
            {
                if (keyEvent->key() == Qt::Key_Z) {
                    ui->streamAudio->deselectTitles();
                }
                if (keyEvent->key() == Qt::Key_K) {
                    ui->streamSubtitle->deselectTitles();
                }
            }
        if (keyEvent->key() == Qt::Key_F10) {
            auto actList = m_pMenuBar->actions();
            if (!actList.empty()) {
                //actList.at(0)->activate(QAction::ActionEvent::Hover);
            }
            return true;
        }
    } else
    if (watched == m_pTableLabel) {
        if (event->type() == QEvent::MouseButtonPress) {
            auto* mouse_event = dynamic_cast<QMouseEvent*>(event);
            if (mouse_event->button() == Qt::LeftButton) {
                onAddFiles();
                return true;
            }
        }
    } else
    if (watched == ui->labelPreview) {
        if (event->type() == QEvent::Resize) {
            if (!m_preview_pixmap.isNull()) {
                QPixmap pix_scaled;
                pix_scaled = m_preview_pixmap.scaled(ui->framePreview->size(),
                                                   Qt::KeepAspectRatio, Qt::SmoothTransformation);
                ui->labelPreview->setPixmap(pix_scaled);
            }
        }
    }
    return BaseWindow::eventFilter(watched, event);
}

/************************************************
** Browser
************************************************/

void MainWindow::onViewMode(uchar ind)
{
    if (ind == 1) {
        ui->listFiles->setViewMode(QListView::IconMode);
        ui->listFiles->setWordWrap(true);
#ifdef Q_OS_UNIX
        ui->listFiles->setSpacing(10 * Helper::scaling());
        ui->listFiles->setGridSize(QSize(120, 120) * Helper::scaling());
#else
        ui->listFiles->setSpacing(0);
        ui->listFiles->setGridSize(QSize(100, 100) * Helper::scaling());
#endif
        ui->listFiles->setIconSize(QSize(80, 80) * Helper::scaling());
    } else {
        ui->listFiles->setViewMode(QListView::ListMode);
        ui->listFiles->setWordWrap(false);
#ifdef Q_OS_UNIX
        ui->listFiles->setSpacing(5 * Helper::scaling());
        ui->listFiles->setGridSize(QSize(200, 50) * Helper::scaling());
#else
        ui->listFiles->setSpacing(0);
        ui->listFiles->setGridSize(QSize(200, 45) * Helper::scaling());
#endif
        ui->listFiles->setIconSize(QSize(30, 40) * Helper::scaling());
    }
}

void MainWindow::setBrowser()
{
    m_pDirModel = new QFileSystemModel(this);
    m_pDirModel->setReadOnly(true);
    m_pDirModel->setFilter(QDir::NoDotAndDotDot | QDir::AllDirs);
    //connect(m_pDirModel, &QFileSystemModel::directoryLoaded, this, [this, ind, root](const QString &path) {
        //ui->treeDirs->expandAll();
        /*if (path == root) {
            const QModelIndex child = m_pDirModel->index(0, 0, ind);
            if (child.isValid()) {
                ui->treeDirs->setCurrentIndex(child);
                onTreeDirsClicked(child);
            }
        }*/
    //});
    ui->treeDirs->setModel(m_pDirModel);
    ui->treeDirs->setColumnWidth(0, 150);
    ui->treeDirs->hideColumn(1);
    ui->treeDirs->hideColumn(2);
    ui->treeDirs->hideColumn(3);
    ui->treeDirs->header()->hide();

    m_pFileModel = new QFileSystemModel(this);
    m_pFileModel->setReadOnly(true);
    m_pFileModel->setFilter(QDir::NoDotAndDotDot | QDir::Files);
    QStringList filters = {
        "*.avi", "*.m2ts", "*.m4v", "*.mkv", "*.mov", "*.mp4",
        "*.mpeg", "*.mpg", "*.mxf", "*.wmv", "*.ts", "*.webm"
    };
    m_pFileModel->setNameFilterDisables(false);
    m_pFileModel->setNameFilters(filters);

    ui->listFiles->setModel(m_pFileModel);
    ui->listFiles->setResizeMode(QListView::ResizeMode::Adjust);
    ui->listFiles->setFlow(QListView::Flow::LeftToRight);
    ui->listFiles->setTextElideMode(Qt::ElideMiddle);
    ui->listFiles->setMovement(QListView::Movement::Snap);
    ui->listFiles->setUniformItemSizes(true);
    ui->listFiles->setWrapping(true);
    ui->listFiles->setSelectionRectVisible(true);
    ui->listFiles->setSelectionMode(QAbstractItemView::SelectionMode::ContiguousSelection);
}

void MainWindow::onBack()
{
    auto getPreviousPath = [](const QString &path)->QString {
        const QString previousPath = QFileInfo(path).dir().absolutePath();
        if (QFileInfo::exists(previousPath)) {
            return previousPath;
        }
        return path;
    };
    const QString root = m_pDirModel->rootPath();
    const QString previousPath = getPreviousPath(root);
    if (previousPath != root) {
        ui->treeDirs->setRootIndex(m_pDirModel->setRootPath(previousPath));
        ui->listFiles->setRootIndex(m_pFileModel->setRootPath(previousPath));
    }
}

void MainWindow::onForward()
{
    ui->treeDirs->setRootIndex(m_pDirModel->setRootPath(m_openDir));
    ui->listFiles->setRootIndex(m_pFileModel->setRootPath(m_openDir));
}

void MainWindow::onTreeDirsClicked(const QModelIndex &index)
{
    const QString path = m_pDirModel->fileInfo(index).absoluteFilePath();
    ui->listFiles->setRootIndex(m_pFileModel->setRootPath(path));
}

void MainWindow::onTreeDirsDblClicked(const QModelIndex &index)
{
    const QString path = m_pDirModel->fileInfo(index).absoluteFilePath();
    ui->treeDirs->setRootIndex(m_pDirModel->setRootPath(path));
    ui->listFiles->setRootIndex(m_pFileModel->setRootPath(path));
}

void MainWindow::provideListContextMenu(const QPoint &pos)     // Call file items menu
{
    auto selectedList = ui->listFiles->selectionModel()->selectedIndexes();
    if (!selectedList.empty())
        m_pFilesItemMenu->exec(ui->listFiles->mapToGlobal(pos + QPoint(5, 5)));
}

void MainWindow::onAddToTask()
{
    QStringList list;
    foreach(auto index, ui->listFiles->selectionModel()->selectedIndexes())
        list.append(m_pFileModel->fileInfo(index).absoluteFilePath());
    if (!list.empty())
        openFiles(list);
}

/************************************************
** Encoder
************************************************/

void MainWindow::initEncoding()
{
    ui->textBrowser_log->clear();
    const QString globalTitle = ui->lineEditGlobalTitle->text();
    const int streamCutting = ui->switchCutting->currentIndex();

    // Avoid touching our defined colors.
    // Shim the alpha values here because the color picker clobbers them and we need reliable values.
    QColor subtitles_color = m_subtitles_color;
    subtitles_color.setAlpha(0);
    QColor subtitles_background_color = m_subtitles_background_color;
    subtitles_background_color.setAlpha(m_subtitles_background_alpha);

    m_pEncoder->initEncoding(m_temp_file,
                             m_input_file,
                             m_output_file,
                             m_width,
                             m_height,
                             m_fps,
                             m_startTime,
                             m_endTime,
                             m_dur,
                             extension,
                             globalTitle,
                             m_curParams,
                             m_hdr,
                             m_data[m_row],
                             &m_fr_count,
                             streamCutting,
                             m_subtitles_font,
                             m_subtitles_fontSize,
                             QString(subtitles_color.name(QColor::HexArgb).replace("#", "")),
                             m_subtitles_background,
                             QString(subtitles_background_color.name(QColor::HexArgb).replace("#", "")),
                             m_subtitles_location,
                             m_threads,
                             m_ffmpeg_prio
    );
}

void MainWindow::onEncodingMode(const QString &mode)
{
    ui->label_Progress->setText(mode);
}

void MainWindow::onEncodingStarted()
{
    setStatus(tr("Encoding"));
    setWidgetsEnabled(false);
    setProgressEnabled(true);
    encoding = true;
}

void MainWindow::onEncodingInitError(const QString &message)
{
    setWidgetsEnabled(true);
    showInfoMessage(message);
    encoding = false;
}

void MainWindow::onEncodingProgress(const int percent, const float rem_time)
{
    ui->progressBar->setValue(percent);
    ui->label_RemTime->setText(Helper::timeConverter(rem_time));
}

void MainWindow::onEncodingLog(const QString &log)
{
    ui->textBrowser_log->append(log);
}

void MainWindow::onEncodingCompleted()
{
    auto onTaskCompleted = [this]() {
        setWidgetsEnabled(true);
        const time_t end_t = time(nullptr);
        const float elps_t = (end_t >= m_strt_t) ? static_cast<float>(end_t - m_strt_t) : 0.0f;
        if (m_protectFlag)
            m_pTimer->stop();
        showPopup(tr("Task completed!\n\n Elapsed time: ") +
                        Helper::timeConverter(elps_t));
    };
    Print("Completed ...");
    setStatus(tr("Done!"));
    m_pAnimation->stop();
    setProgressEnabled(false);
    if (m_batch_mode) {
        const int row = ui->tableWidget->currentRow();
        const int numRows = ui->tableWidget->rowCount();
        if (numRows > (row + 1)) {
            ui->tableWidget->selectRow(row + 1);
            initEncoding();
        } else {
            onTaskCompleted();
        }
    } else {
        onTaskCompleted();
    }
    QList<QString> tempFiles = {
        "/ffmpeg2pass-0.log", "/ffmpeg2pass-0.log.mbtree",
        "/x265_2pass.log.temp", "/x265_2pass.log.cutree.temp",
        "/x265_2pass.log", "/x265_2pass.log.cutree"
    };
    foreach (auto &file, tempFiles)
        QDir().remove(QDir::homePath() + file);
    encoding = false;
}

void MainWindow::onEncodingAborted()
{
    Print("Stop execute ...");
    if (m_protectFlag)
        m_pTimer->stop();
    setStatus(tr("Stop"));
    setWidgetsEnabled(true);
    setProgressEnabled(false);
    showPopup(tr("The current encoding process has been canceled!\n"));
    encoding = false;
}

void MainWindow::onEncodingError(const QString &error_message, bool popup)
{
    if (!popup) {
        Print("Error...");
        if (m_protectFlag)
            m_pTimer->stop();
        setStatus(tr("Error!"));
        setWidgetsEnabled(true);
        const QString msg = (error_message != "") ? tr("An error occurred: ") + error_message :
                                                    tr("Unexpected error occurred!");
        showInfoMessage(msg);
    } else {
        showPopup(error_message);
    }
    encoding = false;
}

void MainWindow::pause()    // Pause encoding
{
    if (m_protectFlag)
        m_pTimer->stop();
    if (m_pEncoder->getEncodingState() != QProcess::NotRunning) {
        setStatus(tr("Pause"));
        m_pAnimation->stop();
        m_pEncoder->pauseEncoding();
    }
}

void MainWindow::resume()   // Resume encoding
{
    if (m_protectFlag)
        m_pTimer->start();
    if (m_pEncoder->getEncodingState() != QProcess::NotRunning) {
        setStatus(tr("Encoding"));
        m_pAnimation->start();
        m_pEncoder->resumeEncoding();
    }
}

void MainWindow::repeatHandler_Type_1()  // Repeat handler
{
    //Print("Repeat handler...");
    onStart();
    showInfoMessage(tr("Pause"), true);
    onStart();
}

/************************************************
** Task Window
************************************************/

void MainWindow::onAddFiles() // Add files
{
    QStringList result;
    Helper::openFileDialog(Helper::FileDialogType::OPENVFILES,
                           tr("Open Files"),
                           m_openDir,
                           result);
    if (!result.isEmpty())
        openFiles(result);
}

void MainWindow::onRemoveFile()  // Remove file from table
{
    const int row = ui->tableWidget->currentRow();
    if (row != -1) {
        ui->tableWidget->blockSignals(true);
        ui->tableWidget->removeRow(row);
        Helper::eraseRow(m_data, row);
        ui->tableWidget->blockSignals(false);
        onTableSelectionChanged();
    }
}

void MainWindow::onRemoveAllFiles()  // Remove all files from table
{
    const int rowsCount = ui->tableWidget->rowCount();
    if(rowsCount <= 0)
        return;
    ui->tableWidget->blockSignals(true);
    for(int row = rowsCount - 1; row > -1; row--) {
        ui->tableWidget->removeRow(row);
        Helper::eraseRow(m_data, row);
    }
    ui->tableWidget->blockSignals(false);
    onTableSelectionChanged();
}

void MainWindow::onSort(const bool up)
{
    const int rowsCount = ui->tableWidget->rowCount();
    for (int i = 0; i < rowsCount; i++) {
        auto *id = new QTableWidgetItem(numToStr(i));
        ui->tableWidget->setItem(i, ColumnIndex::T_ID, id);
    }
    Qt::SortOrder srt = (up) ? Qt::AscendingOrder : Qt::DescendingOrder;
    ui->tableWidget->sortByColumn(ColumnIndex::FILENAME, srt);
    QVector<int> order(0);
    for (int i = 0; i < rowsCount; i++)
        order.push_back(GETTEXT(i, T_ID).toInt());
    Helper::reorder(m_data, order);
}

void MainWindow::onSortDown()    // Sort table
{
    onSort(false);
}

void MainWindow::onSortUp()    // Sort table
{
    onSort(true);
}

void MainWindow::onStart()  // Encode button
{
    switch (m_status_encode_btn) {
    case EncodingStatus::START: {
        Print("Status encode btn: start");
        if (ui->tableWidget->rowCount() == 0) {
            showPopup(tr("Select input file first!"), PopupMessage::Icon::Warning);
            return;
        }
        if (m_pos_cld == -1) {
            showPopup(tr("Select preset first!"), PopupMessage::Icon::Warning);
            return;
        }
        m_status_encode_btn = EncodingStatus::PAUSE;
        m_strt_t = time(nullptr);
        if (m_protectFlag)
            m_pTimer->start();
        initEncoding();
        break;
    }
    case EncodingStatus::PAUSE: {
        Print("Status encode btn: pause");
        pause();
        m_status_encode_btn = EncodingStatus::RESUME;
        break;
    }
    case EncodingStatus::RESUME: {
        Print("Status encode btn: resume");
        resume();
        m_status_encode_btn = EncodingStatus::PAUSE;
        break;
    }
    default:
        return;
    }
    QString status[] = {tr("Pause"), tr("Resume"), tr("Pause")};
    ui->start->setToolTip(status[m_status_encode_btn]);
    ui->start->setProperty("status", m_status_encode_btn);
    ui->start->style()->polish(ui->start);
}

void MainWindow::onStop()    // Stop
{
    Print("Call Stop ...");
    if (m_pEncoder->getEncodingState() != QProcess::NotRunning) {
        bool confirm = showDialogMessage(tr("Stop encoding?"));
        if (confirm)
            m_pEncoder->stopEncoding();
    }
}

void MainWindow::changePriority(int new_prio)
{
    // Do we have a process running....
    long long pid = m_pEncoder->getPid();
    if (pid != -1) {
        QOperatingSystemVersion ostype = QOperatingSystemVersion::current();
        if (ostype.type() == QOperatingSystemVersion::Windows) {
#if defined(Q_OS_WIN64)
            set_process_prio_win(pid, new_prio);
#endif
        }
        else
        {
            // Linux...
            QString nicelevel;
            switch (new_prio)
            {
                case Constants::lowest:
                    nicelevel = "19";
                    break;
                case Constants::low:
                    nicelevel = "9";
                    break;
                case Constants::normal:
                default:
                    nicelevel = "0";
                    break;
                case Constants::high:
                    nicelevel = "-9";
                    break;
                case Constants::highest:
                    nicelevel = "-19";
                    break;
            }
            auto program = "renice";
            // ffmpeg threads are spun up and show up as separate process IDs.
            // They don't show up in /proc, but show up under the parent pid task folder.
            auto dir = QDir("/proc/" + QString::number(pid) + "/task/");
            QStringList entries = dir.entryList();

            for (QString& entry : entries) {
                bool ok = false;
                int pid_ = entry.toInt(&ok);
                if (ok) {
                    auto arguments = QStringList{"-n", nicelevel, "-p", entry};
                    try {
                        QProcess::startDetached(program, arguments);
                    }
                    catch (...) {
                        Print("renice command not found!!!");
                    }
                }
            }
        }
    }
}

// Completely untested.
#if defined(Q_OS_WIN64)
void MainWindow::set_process_prio_win(long long pid, int _prio)
{
    // Get the process handle
    HANDLE hProcess = OpenProcess(PROCESS_SET_INFORMATION, FALSE, pid);
    if (hProcess == nullptr) {
        qWarning("Failed to open process");
        return;
    }

    auto nicelevel = NORMAL_PRIORITY_CLASS;
    switch (_prio)
    {
        case Constants::lowest:
            nicelevel = IDLE_PRIORITY_CLASS;
            break;
        case Constants::low:
            nicelevel = BELOW_NORMAL_PRIORITY_CLASS;
            break;
        case Constants::normal:
        default:
            nicelevel = NORMAL_PRIORITY_CLASS;
            break;
        case Constants::high:
            nicelevel = ABOVE_NORMAL_PRIORITY_CLASS;
            break;
        case Constants::highest:
            nicelevel = HIGH_PRIORITY_CLASS;
            break;
    }

    // Set the priority class
    if (!SetPriorityClass(hProcess, nicelevel)) {
        qWarning("Failed to set process priority");
    }

    // Close the process handle
    CloseHandle(hProcess);
}
#endif

void MainWindow::openFiles(const QStringList &openFileNames)    // Open files
{
    Progress prg(this, tr("OPENING FILES"));
    prg.setModal(true);
    setProgressEnabled(false);
    MediaInfo MI;
    for (int i = 0; i < openFileNames.size(); i++) {
        const QString& file = openFileNames.at(i);
        const QString inputFolder = QFileInfo(file).absolutePath();
        const QString inputFile = QFileInfo(file).fileName();
        if (MI.Open(file.toStdWString()) == 1) {
            if (i == 0) {
                m_openDir = inputFolder;
                prg.show();
            }
            prg.setText(inputFile);
            prg.setPercent(0);
            const int numRows = ui->tableWidget->rowCount();
            ui->tableWidget->setRowCount(numRows + 1);
            QString fmt_qstr = VINFO(0, "Format");
            QString width_qstr = VINFO(0, "Width");
            QString height_qstr = VINFO(0, "Height");
            QString size = (width_qstr != "") ? width_qstr + "x" + height_qstr : "Undef";
            double duration_double = 0.001 * VINFO(0, "Duration").toDouble();
            QString durationTime = Helper::timeConverter(static_cast<float>(duration_double));
            QString fps_qstr = VINFO(0, "FrameRate");
            const QString status("");
            int bitrate_int = static_cast<int>(0.001 * VINFO(0, "BitRate").toDouble());
            QString coeff = VINFO(0, "matrix_coefficients");
            const QString mtrx_coeff = coeff.replace(" ", "").replace(".", "").replace("on-", "").replace("onstant", "");
            QString luminance_qstr = VINFO(0, "MasteringDisplay_Luminance");
            QString luminance_rep = luminance_qstr.replace("min: ", "").replace("max: ", "").replace(" cd/m2", "");
            QString min_luminance = luminance_rep.split(", ")[0];
            QString max_luminance = luminance_rep.replace(min_luminance, "").replace(", ", "");
            QString color_prim = VINFO(0, "MasteringDisplay_ColorPrimaries");
            int len = color_prim.length();
            QString white_coord = "";
            QString chroma_coord = "";
            if (len > 15) {
                QStringList mdcp = color_prim.split(",");
                QString r = mdcp[0].replace("R: ", "").replace("x=", "").replace(" ", ",").replace("y=", "").replace("000", "0");
                QString g = mdcp[1].replace(" G: ", "").replace("x=", "").replace(" ", ",").replace("y=", "").replace("000", "0");
                QString b = mdcp[2].replace(" B: ", "").replace("x=", "").replace(" ", ",").replace("y=", "").replace("000", "0");
                white_coord = mdcp[3].replace(" White point: ", "").replace("x=", "").replace(" ", ",").replace("y=", "").replace("000", "0");
                chroma_coord = r + "," + g + "," + b;
                color_prim = "Undefined";
            }

            if (fmt_qstr == "")
                fmt_qstr = "Undef";
            if (fps_qstr == "")
                fps_qstr = "Undef";
            if (durationTime == "00:00:00")
                durationTime = "Undef";

            // Check video codec compatibility with current container selection
            if (!fmt_qstr.isEmpty() && fmt_qstr != "Undef") {
                const QString currentContainer = m_curParams[CurParamIndex::CONTAINER];
                if (!Helper::isVideoSupported(currentContainer, fmt_qstr)) {
                    const QString warningMsg = tr("Warning: Video codec '%1' may not be compatible with container '%2'.\n"
                                                 "Consider using MKV for maximum codec compatibility, or MP4 for H.264/H.265.\n"
                                                 "This may cause encoding errors.")
                                               .arg(fmt_qstr, currentContainer);
                    showInfoMessage(warningMsg);
                }
            }

            const QString arr_items[] = {
                inputFolder + "/" + inputFile, // Combine path and filename for display
                fmt_qstr,
                size,
                durationTime,
                fps_qstr,
                VINFO(0, "DisplayAspectRatio"),
                status,
                tr("Default"), // PRESET column - will be updated when preset is applied
                numToStr(bitrate_int),
                VINFO(0, "ChromaSubsampling"),
                VINFO(0, "BitDepth"),
                VINFO(0, "ColorSpace"),
                VINFO(0, "colour_range"),
                VINFO(0, "colour_primaries").replace(".", ""),
                mtrx_coeff,
                VINFO(0, "transfer_characteristics").replace(".", ""),
                max_luminance,
                min_luminance,
                VINFO(0, "MaxCLL").replace(" cd/m2", ""),
                VINFO(0, "MaxFALL").replace(" cd/m2", ""),
                color_prim,
                inputFolder, // Keep separate path for internal use
                QString::number(duration_double, 'f', 3),
                chroma_coord,
                white_coord,
                VINFO(0, "StreamSize"),
                width_qstr,
                height_qstr,
                VINFO(0, "Title"),
                GETINFO(Stream_General, 0, "Movie"),
                GETINFO(Stream_General, 0, "YEAR"),
                GETINFO(Stream_General, 0, "AUTHOR"),
                GETINFO(Stream_General, 0, "Performer"),
                GETINFO(Stream_General, 0, "Description")
            };

            m_data.resize(numRows + 1);
            m_data[numRows].clear();
            for (int j = 28; j < 34; j++)
                m_data[numRows].videoMetadata.push_back(arr_items[j]);
            
            // Initialize with default preset parameters
            m_data[numRows].presetParams = default_preset.toVector();
            m_data[numRows].presetName = tr("Default");

            for (int column = ColumnIndex::FILENAME; column <= ColumnIndex::T_HEIGHT; column++) {
                auto *item = new QTableWidgetItem(arr_items[column]);
                if (column >= ColumnIndex::FORMAT && column <= ColumnIndex::MASTERDISPLAY)
                    item->setTextAlignment(Qt::AlignCenter);
                ui->tableWidget->setItem(numRows, column, item);
            }

            auto *startTime = new QTableWidgetItem("0");
            auto *endTime = new QTableWidgetItem("0");
            ui->tableWidget->setItem(numRows, ColumnIndex::T_STARTTIME, startTime);
            ui->tableWidget->setItem(numRows, ColumnIndex::T_ENDTIME, endTime);

            for (int j = 0; j < MAX_AUDIO_STREAMS; j++) {
                QString audioFormat = AINFO(size_t(j), "Format");
                const int smplrt_int = static_cast<int>(AINFO(size_t(j), "SamplingRate").toFloat() / 1000);
                const QString smplrt = (smplrt_int != 0) ? numToStr(smplrt_int) : "";
                if (!audioFormat.isEmpty()) {
                    audioFormat += QString("  %1 kHz").arg(smplrt);
                    m_data[numRows].checks[Data::audioChecks].push_back(Helper::isAudioSupported(extension, audioFormat));
                    m_data[numRows].fields[Data::audioFormats].push_back(audioFormat);
                    m_data[numRows].fields[Data::audioChannels].push_back(AINFO(size_t(j), "Channels"));
                    m_data[numRows].fields[Data::audioChLayouts].push_back(AINFO(size_t(j), "ChannelLayout"));
                    m_data[numRows].fields[Data::audioDuration].push_back(AINFO(size_t(j), "Duration"));
                    m_data[numRows].fields[Data::audioLangs].push_back(AINFO(size_t(j), "Language"));
                    m_data[numRows].fields[Data::audioTitles].push_back(AINFO(size_t(j), "Title"));
                    const QString deflt = AINFO(size_t(j), "Default");
                    m_data[numRows].checks[Data::audioDef].push_back(deflt == "Yes");
                } else {
                    break;
                }
            }

            for (int j = 0; j < MAX_SUBTITLES; j++) {
                const QString subtitleFormat = SINFO(size_t(j), "Format");
                if (!subtitleFormat.isEmpty()) {
                    bool select;
                    if (m_subtitles_deselectall) {
                        select = false;
                    }
                    else
                    {
                        select = Helper::isSubtitleSupported(extension, subtitleFormat);
                    }
                    m_data[numRows].checks[Data::subtChecks].push_back(select);
                    m_data[numRows].fields[Data::subtFormats].push_back(subtitleFormat);
                    m_data[numRows].fields[Data::subtDuration].push_back(SINFO(size_t(j), "Duration"));
                    m_data[numRows].fields[Data::subtLangs].push_back(SINFO(size_t(j), "Language"));
                    m_data[numRows].fields[Data::subtTitles].push_back(SINFO(size_t(j), "Title"));
                    const QString deflt = SINFO(size_t(j), "Default");
                    m_data[numRows].checks[Data::subtDef].push_back(deflt == "Yes");
                    m_data[numRows].checks[Data::subtBurn].push_back(false);
                } else {
                    break;
                }
            }
            MI.Close();
            prg.setPercent(50);
            ui->tableWidget->selectRow(ui->tableWidget->rowCount() - 1);
            
            // Update file-level incompatibility status
            updateFileIncompatibilityStatus(numRows);
            
            Helper::nonBlockDelay(50);
            prg.setPercent(100);
        } else {
            showInfoMessage(tr("File: \'%1\' cannot be opened!").arg(inputFile));
        }
    }
}

void MainWindow::onTableSelectionChanged()
{
    setProgressEnabled(false);
    ui->labelSplitPreview->clear();
    ui->sliderTimeline->blockSignals(true);
    ui->sliderTimeline->setValue(0);
    ui->sliderTimeline->blockSignals(false);
    ui->lineEditCurTime->clear();
    ui->lineEditStartTime->clear();
    ui->lineEditEndTime->clear();

    //*********** Disable audio widgets ***************//
    ui->streamAudio->clearList();
    //ui->streamAudioExtern->clearList();
    m_pAudioLabel->setVisible(true);

    //*********** Disable subtitle widgets *************//
    ui->streamSubtitle->clearList();
    m_pSubtitleLabel->setVisible(true);

    m_row = ui->tableWidget->currentRow();
    if (m_row != -1) {
        m_pTableLabel->hide();
        get_current_data();
        
        // Load per-file preset parameters if available
        if (m_row < m_data.size() && !m_data[m_row].presetParams.isEmpty()) {
            for (int k = 0; k < PARAMETERS_COUNT && k < m_data[m_row].presetParams.size(); k++) {
                m_curParams[k] = m_data[m_row].presetParams[k];
            }
        }
    } else {
        //************* Reset widgets ******************//
        m_pTableLabel->show();
        m_preview_pixmap = QPixmap();
        ui->labelPreview->clear();
        ui->textBrowser_1->clear();
        ui->textBrowser_2->clear();
        ui->labelPreview->setText(tr("Preview"));
        ui->label_source->setText("");
        ui->label_output->setText("");
        ui->sliderTimeline->setMaximum(0);

        //************* Reset widgets ******************//
        QList<QLineEdit *> linesEditMetadata = ui->frameTab_1->findChildren<QLineEdit *>();
        foreach (QLineEdit *lineEdit, linesEditMetadata) {
            lineEdit->clear();
            lineEdit->setEnabled(false);
        }

        //********** Reset video variables *************//
        m_dur = 0.0;
        m_input_file = "";
        m_stream_size = "";
        m_width = "";
        m_height = "";
        m_fmt = "";
        m_fps = "";
        m_fr_count = 0;
        m_startTime = 0.0;
        m_endTime = 0.0;

        //************** Reset data ********************//
        for (auto & i : m_hdr)
            i = "";
    }
}

void MainWindow::resizeTableRows(int rows_height)
{
    QHeaderView *vertHeader = ui->tableWidget->verticalHeader();
    vertHeader->setSectionResizeMode(QHeaderView::Fixed);
    vertHeader->setDefaultSectionSize(rows_height * Helper::scaling());
    if (rows_height == ROWHEIGHT) {
        ui->tableWidget->setIconSize(QSize(16, 16) * Helper::scaling());
    } else {
        const int rows_width = static_cast<int>(round(1.777f*rows_height * Helper::scaling()));
        ui->tableWidget->setIconSize(QSize(rows_width, rows_height * Helper::scaling()));
    }
    const int numRows = ui->tableWidget->rowCount();
    if (numRows > 0) {
        int row = ui->tableWidget->currentRow();
        ui->tableWidget->clearSelection();
        if (numRows > 1) {
            for (int i = 0; i < numRows; i++)
                ui->tableWidget->selectRow(i);
        }
        ui->tableWidget->selectRow(row);
    }
}

void MainWindow::resetView()
{
    const QString defaultSettings(":/resources/data/default_settings.ini");
    QSettings stn(defaultSettings, QSettings::IniFormat, this);
    stn.beginGroup("Settings");
    stn.remove("");
    stn.endGroup();
    if (stn.value("Version").toInt() == SETTINGS_VERSION) {
        // Restore DocksContainer
        stn.beginGroup("DocksContainer");
        m_pDocksContainer->restoreState(stn.value("DocksContainer/state").toByteArray());
        stn.endGroup();
    }

    readXMLSettingsFile(":/resources/data/default_settings.xml");

    QList<int> dockSizesX = {};
    QList<int> dockSizesY = {};
    float coeffX[DOCKS_COUNT] = {0.39f, 0.39f, 0.61f, 0.23f, 0.23f, 0.23f, 0.23f, 0.39f};
    float coeffY[DOCKS_COUNT] = {0.16f, 0.5f, 0.1f, 0.9f, 0.9f, 0.9f, 0.9f, 0.3f};
    for (int i = 0; i < DOCKS_COUNT; i++) {
        int dockWidth = static_cast<int>(coeffX[i] * this->width());
        int dockHeight = static_cast<int>(coeffY[i] * this->height());
        dockSizesX.append(dockWidth);
        dockSizesY.append(dockHeight);
    }
    setDocksParameters(dockSizesX, dockSizesY);
}

void MainWindow::provideContextMenu(const QPoint &pos)     // Call table items menu
{
    QTableWidgetItem *item = ui->tableWidget->itemAt(0, pos.y());
    if (item)
        m_pItemMenu->exec(ui->tableWidget->mapToGlobal(pos + QPoint(0, 35)));
}

void MainWindow::dragEnterEvent(QDragEnterEvent* event)     // Drag enter event
{
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void MainWindow::dropEvent(QDropEvent* event)     // Drag & Drop
{
    const QMimeData *mimeData = event->mimeData();
    if (mimeData->hasUrls()) {
        QStringList formats;
        QStringList pathList;
        QList<QUrl> urlList = mimeData->urls();
        for (int i = 0; i < urlList.size(); ++i) {
            pathList.append(urlList.at(i).toLocalFile());
            formats.append(QMimeDatabase().mimeTypeForFile(pathList.at(i)).name());
        }
        if (!formats.filter("audio/").empty() || !formats.filter("video/").empty()) {
            openFiles(pathList);
        }
    }
}

void MainWindow::onComboModeChanged(int index)
{
    m_batch_mode = (index != 0);
}

void MainWindow::onSliderResizeChanged(int value)
{
    m_rowHeight = value;
    resizeTableRows(value);
}

/************************************************
** Source Window
************************************************/

QString MainWindow::setThumbnail(QString curFilename,
                             const double time,
                             const int quality,
                             const int destination)
{
    QStringList qualityParam;
    if (quality == PreviewRes::RES_LOW)
    {
        qualityParam.append("-vf");
        qualityParam.append("scale=144:-1,format=pal8,dctdnoiz=4.5");
    }
    else {
        qualityParam.append("-vf");
        qualityParam.append("scale=480:-1");
        qualityParam.append("-compression_level");
        qualityParam.append("10");
        qualityParam.append("-pred");
        qualityParam.append("mixed");
        qualityParam.append("-pix_fmt");
        qualityParam.append("rgb24");
        qualityParam.append("-sws_flags");
        qualityParam.append("+accurate_rnd+full_chroma_int");
    }
    const QString time_qstr = QString::number(time, 'f', 3);
    const QString tmb_name = curFilename.replace(".", "_").replace(" ", "_") + time_qstr;
    QString tmb_file = THUMBNAILPATH + QString("/%1.png").arg(tmb_name);
    QFile tmb(tmb_file);
    if (!tmb.exists()) {
        QStringList cmd;
        cmd.append("-hide_banner");
        cmd.append("-probesize");
        cmd.append("100M");
        cmd.append("-analyzeduration");
        cmd.append("50M");
        if (destination == PreviewDest::PREVIEW) {
            cmd.append("-skip_frame");
            cmd.append("nokey");
        }
        cmd.append("-ss");
        cmd.append(time_qstr);
        cmd.append("-i");
        cmd.append(m_input_file);
        cmd.append(qualityParam);
        cmd.append("-vframes");
        cmd.append("1");
        cmd.append("-y");
        cmd.append(tmb_file);
        m_pProcessThumbCreation = new QProcess(this);
        m_pProcessThumbCreation->start("ffmpeg", cmd);
        m_pProcessThumbCreation->waitForFinished();
        delete m_pProcessThumbCreation, m_pProcessThumbCreation = nullptr;
    }
    if (!tmb.exists())
        tmb_file = ":/resources/images/no_preview.png";
    m_preview_pixmap = QPixmap(tmb_file);
    QPixmap pix_scaled;
    if (destination == PreviewDest::PREVIEW) {
        pix_scaled = m_preview_pixmap.scaled(ui->framePreview->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        ui->labelPreview->setPixmap(pix_scaled);
    } else
    if (destination == PreviewDest::SPLITTER) {
        pix_scaled = m_preview_pixmap.scaled(ui->labelSplitPreview->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        ui->labelSplitPreview->setPixmap(pix_scaled);
    }
    return tmb_file;
}

void MainWindow::repeatHandler_Type_2()
{
    if (m_row != -1)
        setThumbnail(m_curFilename, m_curTime, PreviewRes::RES_HIGH, PreviewDest::SPLITTER);
}

/************************************************
** Output Window
************************************************/

void MainWindow::get_output_filename()  // Get output data
{
    if (m_row < 0 || m_row >= m_data.size()) {
        return; // Invalid row
    }
    
    // Use per-file preset parameters if available, otherwise use global
    QVector<QString> *params = &m_curParams;
    if (m_row < m_data.size() && !m_data[m_row].presetParams.isEmpty()) {
        params = &m_data[m_row].presetParams;
    }
    
    ui->textBrowser_2->clear();
    ui->textBrowser_2->setText((*params)[CurParamIndex::OUTPUT_PARAM]);
    const int CE_CODEC = (*params)[CurParamIndex::CODEC].toInt();
    const int CE_CONTAINER = (*params)[CurParamIndex::CONTAINER].toInt();
    Tables t;
    extension = t.arr_container[CE_CODEC][CE_CONTAINER].toLower();
    QString suffix;
    if (m_suffixType == 0) {
        QString row_qstr = numToStr(m_row);
        std::ostringstream sstr;
        sstr << std::setw(4) << std::setfill('0') << row_qstr.toStdString();
        std::string counter = sstr.str();
        suffix = m_suffixName + QString::fromStdString(counter);
    } else {
        QTime ct = QTime::currentTime();
        QDate cd = QDate::currentDate();
        QString ct_qstr = ct.toString();
        QString cd_qstr = cd.toString("MM.dd.yyyy");
        suffix = QString("_") + ct_qstr.remove(":") + QString("_") + cd_qstr.remove(".");
    }

    QString prefix = m_prefixName;
    if (m_prefxType == 0) {
        std::wstring file_name_wstr = m_curFilename.toStdWString();
        std::wstring::size_type separator = file_name_wstr.rfind('.');
        const QString file_without_ext = (separator != std::wstring::npos) ?
                    QString::fromStdWString(file_name_wstr.substr(0, separator)) : m_curFilename;
        prefix = file_without_ext;
    }

    const QString _output_file_name = QString("%1%2.%3").arg(prefix, suffix, extension);
    ui->label_output->setText(_output_file_name);
    m_output_file = (m_output_folder == "") ? m_curPath : m_output_folder;
    m_output_file += QString("/") + _output_file_name;

    m_temp_file = (m_temp_folder == "") ? m_curPath : m_temp_folder;

    // Now we construct our temporary file name.
    // To avoid collisions from multi-session, we append the system time in seconds
    // since the epoch.
    // Get the current time as a time_point
    auto now = std::chrono::system_clock::now();

    // Convert to time_t (epoch time in seconds)
    std::time_t epochTime = std::chrono::system_clock::to_time_t(now);
    std::string s_time = std::to_string(epochTime);
    m_temp_file += QString("/" + prefix + "_" + QString::fromStdString(s_time) + ".mkv");
}

void MainWindow::onSetOutFolder()
{
    QStringList result;
    Helper::openFileDialog(Helper::FileDialogType::SELECTFOLDER,
                           tr("Select output folder"),
                           m_output_folder,
                           result);
    if (!result.isEmpty()) {
        m_output_folder = result.at(0);
        if (m_row != -1)
            get_output_filename();
    }
}

/************************************************
** Metadata Window
************************************************/

void MainWindow::onClearMetadata()
{
    if (m_row != -1) {
        auto linesEditMetadata = ui->frameTab_1->findChildren<QLineEdit*>();
        foreach (QLineEdit *lineEdit, linesEditMetadata)
            MainWindowPrivate::clearLineEdit(lineEdit);
        ui->frameMiddle->setFocus();
    }
}

void MainWindow::onUndoMetadata()
{
    if (m_row != -1) {
        auto linesEditMetadata = ui->frameTab_1->findChildren<QLineEdit*>();
        foreach (QLineEdit *lineEdit, linesEditMetadata)
            MainWindowPrivate::undoLineEdit(lineEdit);
        ui->frameMiddle->setFocus();
    }
}

/************************************************
** Streams Window
************************************************/

void MainWindow::onAddExtStream()
{
    if (m_row != -1) {
        const int ind = ui->tabWidgetStreams->currentIndex();
        QStringList result;
        Helper::openFileDialog(ind == 0 ? Helper::FileDialogType::OPENAFILES :
                                          Helper::FileDialogType::OPENSFILES,
                               tr("Open Files"),
                               m_openDir,
                               result);
        if (!result.isEmpty()) {
            MediaInfo MI;
            foreach (auto &path, result) {
                if (MI.Open(path.toStdWString()) == 1) {
                    const int vcnt = GINFO(0, "VideoCount").toInt();
                    if (ind == 0) {
                        const int acnt = GINFO(0, "AudioCount").toInt();
                        if (vcnt == 0 && acnt == 1) {
                            QString audioFormat = AINFO(0, "Format");
                            const int smplrt_int = static_cast<int>(AINFO(0, "SamplingRate").toFloat() / 1000);
                            const QString smplrt = (smplrt_int != 0) ? numToStr(smplrt_int) : "";
                            if (!audioFormat.isEmpty()) {
                                audioFormat += QString("  %1 kHz").arg(smplrt);
                                m_data[m_row].checks[Data::externAudioChecks].push_back(Helper::isAudioSupported(m_curParams[CurParamIndex::CONTAINER], audioFormat));
                                m_data[m_row].fields[Data::externAudioFormats].push_back(audioFormat);
                                m_data[m_row].fields[Data::externAudioChannels].push_back(AINFO(0, "Channels"));
                                m_data[m_row].fields[Data::externAudioChLayouts].push_back(AINFO(0, "ChannelsLayouts"));
                                m_data[m_row].fields[Data::externAudioDuration].push_back(AINFO(0, "Duration"));
                                m_data[m_row].fields[Data::externAudioLangs].push_back(AINFO(0, "Language"));
                                m_data[m_row].fields[Data::externAudioTitles].push_back(AINFO(0, "Title"));
                                m_data[m_row].fields[Data::externAudioPath].push_back(path);
                                m_data[m_row].checks[Data::externAudioDef].push_back(false);
                            }
                        } else {
                            showInfoMessage(tr("File: \'%1\' is not audio file!").arg(path));
                        }
                    } else {
                        const int scnt = GINFO(0, "TextCount").toInt();
                        if (vcnt == 0 && scnt == 1) {
                            const QString subtitleFormat = SINFO(0, "Format");
                            if (!subtitleFormat.isEmpty()) {
                                m_data[m_row].checks[Data::externSubtChecks].push_back(Helper::isSubtitleSupported(m_curParams[CurParamIndex::CONTAINER], subtitleFormat));
                                m_data[m_row].fields[Data::externSubtFormats].push_back(subtitleFormat);
                                m_data[m_row].fields[Data::externSubtDuration].push_back(SINFO(0, "Duration"));
                                m_data[m_row].fields[Data::externSubtLangs].push_back(SINFO(0, "Language"));
                                m_data[m_row].fields[Data::externSubtTitles].push_back(SINFO(0, "Title"));
                                m_data[m_row].fields[Data::externSubtPath].push_back(path);
                                m_data[m_row].checks[Data::externSubtDef].push_back(false);
                                m_data[m_row].checks[Data::externSubtBurn].push_back(false);
                            }
                        } else {
                            showInfoMessage(tr("File: \'%1\' is not subtitle file!").arg(path));
                        }
                    }
                    MI.Close();
                } else {
                    showInfoMessage(tr("File: \'%1\' cannot be opened!").arg(path));
                }
            }
            if (!m_data[m_row].fields[Data::externAudioFormats].empty()) {
                m_pAudioLabel->setVisible(false);
                Tables t;
                int codecIndex = m_curParams[CurParamIndex::CODEC].toInt();
                int audioCodecIndex = m_curParams[CurParamIndex::AUDIO_CODEC].toInt();
                QString targetAudioCodec = t.arr_acodec[codecIndex][audioCodecIndex];
                bool usePresetSubtitleSettings = m_curParams[CurParamIndex::USE_PRESET_SUBTITLE_SETTINGS].toInt() == 1;
                ui->streamAudio->setList(extension, m_data[m_row], targetAudioCodec, usePresetSubtitleSettings);
            }
            if (!m_data[m_row].fields[Data::externSubtFormats].empty()) {
                m_pSubtitleLabel->setVisible(false);
                Tables t;
                int codecIndex = m_curParams[CurParamIndex::CODEC].toInt();
                int audioCodecIndex = m_curParams[CurParamIndex::AUDIO_CODEC].toInt();
                QString targetAudioCodec = t.arr_acodec[codecIndex][audioCodecIndex];
                bool usePresetSubtitleSettings = m_curParams[CurParamIndex::USE_PRESET_SUBTITLE_SETTINGS].toInt() == 1;
                ui->streamSubtitle->setList(extension, m_data[m_row], targetAudioCodec, usePresetSubtitleSettings);
            }
        }
    }
}

void MainWindow::onDeselectTitles()
{
    if (m_row != -1) {
        const int ind = ui->tabWidgetStreams->currentIndex();
        ind == 0 ? ui->streamAudio->deselectTitles() :
        ui->streamSubtitle->deselectTitles();
    }
}

void MainWindow::onClearTitles()
{
    if (m_row != -1) {
        const int ind = ui->tabWidgetStreams->currentIndex();
        ind == 0 ? ui->streamAudio->clearTitles() :
                   ui->streamSubtitle->clearTitles();
    }
}

void MainWindow::onUndoTitles()
{
    if (m_row != -1) {
        const int ind = ui->tabWidgetStreams->currentIndex();
        ind == 0 ? ui->streamAudio->undoTitles() :
                   ui->streamSubtitle->undoTitles();
    }
}

/************************************************
** Split Window
************************************************/

void MainWindow::onSliderTimelineChanged(int value)
{
    if (m_row != -1) {
        const double fps_double = m_fps.toDouble();
        m_curTime = (fps_double != 0.0) ? round(1000.0 * (static_cast<double>(value)
                                                         / fps_double)) / 1000.0 : 0.0;
        ui->lineEditCurTime->setText(Helper::timeConverter(m_curTime));
        m_pTimerSetThumbnail->stop();
        m_pTimerSetThumbnail->start();
    }
}

void MainWindow::onFramePrev()
{
    const int value = ui->sliderTimeline->value();
    if (value > 0)
        ui->sliderTimeline->setValue(value - 1);
}

void MainWindow::onFrameNext()
{
    const int value = ui->sliderTimeline->value();
    if (value < m_fr_count)
        ui->sliderTimeline->setValue(value + 1);
}

void MainWindow::onSetStartTime()
{
    if (m_row != -1) {
        m_startTime = (m_startTime > m_endTime && m_endTime != 0.0) ? m_endTime : m_curTime;
        ui->lineEditStartTime->setText(Helper::timeConverter(m_startTime));
        auto *item = new QTableWidgetItem(QString::number(m_startTime, 'f', 3));
        ui->tableWidget->setItem(m_row, ColumnIndex::T_STARTTIME, item);
    }
}

void MainWindow::onSetEndTime()
{
    if (m_row != -1) {
        m_endTime = (m_curTime < m_startTime) ? m_startTime : m_curTime;
        ui->lineEditEndTime->setText(Helper::timeConverter(m_endTime));
        auto *item = new QTableWidgetItem(QString::number(m_endTime, 'f', 3));
        ui->tableWidget->setItem(m_row, ColumnIndex::T_ENDTIME, item);
    }
}

void MainWindow::onResetLabels()
{
    if (m_row != -1) {
        ui->labelSplitPreview->clear();
        ui->sliderTimeline->blockSignals(true);
        ui->sliderTimeline->setValue(0);
        ui->sliderTimeline->blockSignals(false);

        m_curTime = 0.0;
        ui->lineEditCurTime->setText(Helper::timeConverter(m_curTime));

        m_startTime = 0.0;
        ui->lineEditStartTime->setText(Helper::timeConverter(m_startTime));
        auto *item_st = new QTableWidgetItem(QString::number(m_startTime, 'f', 3));
        ui->tableWidget->setItem(m_row, ColumnIndex::T_STARTTIME, item_st);

        m_endTime = 0.0;
        ui->lineEditEndTime->setText(Helper::timeConverter(m_endTime));
        auto *item_end = new QTableWidgetItem(QString::number(m_endTime, 'f', 3));
        ui->tableWidget->setItem(m_row, ColumnIndex::T_ENDTIME, item_end);
    }
}

/************************************************
** Preset Window
************************************************/

void MainWindow::setDefaultPresets() // Set default presets
{
    Print("Set defaults...");
    readXMLPresetFile(":/resources/data/default_presets.xml");
}

void MainWindow::onApplyPreset()  // Apply preset
{
    if (ui->treeWidget->currentIndex().row() < 0) {
        showPopup(tr("Select preset first!\n"));
        return;
    }
    
    // Get selected rows
    QList<int> selectedRows;
    QModelIndexList selectedIndexes = ui->tableWidget->selectionModel()->selectedRows();
    
    if (selectedIndexes.isEmpty()) {
        showPopup(tr("Select files first!\n"));
        return;
    }
    
    for (const QModelIndex &index : selectedIndexes) {
        selectedRows.append(index.row());
    }
    
    QTreeWidgetItem *item = ui->treeWidget->currentItem();
    QTreeWidgetItem *parentItem = item->parent();
    if (parentItem) {
        // Item is child - get preset parameters
        QVector<QString> presetParams(PARAMETERS_COUNT);
        for (int k = 0; k < PARAMETERS_COUNT; k++)
            presetParams[k] = item->text(k+7);
        
        QString presetName = item->text(0);  // Get preset name
        
        // Apply preset to all selected files
        for (int row : selectedRows) {
            if (row < m_data.size()) {
                m_data[row].presetParams = presetParams;
                m_data[row].presetName = presetName;
                
                // Update the preset column in the table
                QTableWidgetItem *presetItem = ui->tableWidget->item(row, ColumnIndex::PRESET_COL);
                if (presetItem) {
                    presetItem->setText(presetName);
                    
                    // Add tooltip showing preset details
                    Tables t;
                    int codecIndex = presetParams[CurParamIndex::CODEC].toInt();
                    QString container = t.arr_container[codecIndex][presetParams[CurParamIndex::CONTAINER].toInt()];
                    QString tooltip = tr("Preset: %1\nCodec: %2\nContainer: %3")
                                     .arg(presetName)
                                     .arg(t.arr_codec[codecIndex][0])
                                     .arg(container);
                    presetItem->setToolTip(tooltip);
                }
            }
        }
        
        // Update current global parameters from the preset for UI consistency
        for (int k = 0; k < PARAMETERS_COUNT; k++)
            m_curParams[k] = presetParams[k];
            
        m_pos_top = ui->treeWidget->indexOfTopLevelItem(parentItem);
        m_pos_cld = parentItem->indexOfChild(item);
        
        if (m_row != -1)
            get_output_filename();
        
        showPopup(tr("Applied preset '%1' to %2 file(s)").arg(presetName).arg(selectedRows.size()));
    } else {
        // Item is parent...
        showPopup(tr("Select preset first!\n"));
        return;
    }
    
    // Update incompatibility status for all files since container format may have changed
    for (int i = 0; i < ui->tableWidget->rowCount(); i++) {
        updateFileIncompatibilityStatus(i);
    }
}

void MainWindow::onRemovePreset()  // Remove preset
{
    if (ui->treeWidget->currentIndex().row() < 0)
        return;
    QTreeWidgetItem *item = ui->treeWidget->currentItem();
    QTreeWidgetItem *parentItem = item->parent();
    if (parentItem) {
        // Item is child...
        bool confirm = showDialogMessage(tr("Delete?"));
        if (confirm) {
            const int index_top = ui->treeWidget->indexOfTopLevelItem(parentItem);
            const int index_child = parentItem->indexOfChild(item);
            updateCurPresetPos(index_top, index_child);

            QTreeWidgetItem *takenItem = parentItem->takeChild(index_child);
            Q_ASSERT(takenItem == item);
            delete takenItem;
            updatePresetTable();
        }

    } else {
        // Item is parent...
        if (item->childCount() == 0) {
            const int index_top = ui->treeWidget->indexOfTopLevelItem(item);
            const int index_child = -1;
            updateCurPresetPos(index_top, index_child);

            QTreeWidgetItem *takenItem = ui->treeWidget->takeTopLevelItem(index_top);
            Q_ASSERT(takenItem==item);
            delete takenItem;
            updatePresetTable();

        } else {
            showPopup(tr("Delete presets first!\n"), PopupMessage::Icon::Warning);
        }
    }
}

void MainWindow::onEditPreset()  // Edit preset
{
    if (ui->treeWidget->currentIndex().row() < 0) {
        showPopup(tr("Select preset first!\n"));
        return;
    }
    QTreeWidgetItem *item = ui->treeWidget->currentItem();
    QTreeWidgetItem *parentItem = item->parent();
    if (parentItem) {
        // Item is child...
        for (int k = 0; k < PARAMETERS_COUNT; k++)
            m_newParams[k] = item->text(k+7);
        Preset presetWindow(this, &m_newParams, m_theme);
        if (presetWindow.exec() == Dialog::Accept) {
            for (int k = 0; k < PARAMETERS_COUNT; k++)
                item->setText(k+7, m_newParams[k]);
            updateInfoFields(m_newParams[1],
                             m_newParams[2],
                             m_newParams[3],
                             m_newParams[4],
                             m_newParams[11],
                             m_newParams[12],
                             m_newParams[21],
                             item, true);
            int index_top = ui->treeWidget->indexOfTopLevelItem(parentItem);
            int index_child = parentItem->indexOfChild(item);
            if (m_pos_top == index_top && m_pos_cld == index_child) {
                for (int k = 0; k < PARAMETERS_COUNT; k++)
                    m_curParams[k] = item->text(k+7);
                if (m_row != -1)
                    get_output_filename();
            }
            updatePresetTable();
        }
    } else {
        // Item is parent...
        showPopup(tr("Select preset first!\n"));
    }
}

void MainWindow::onAddSection()  // Add section
{
    QFont parentFont;
    parentFont.setBold(true);
    parentFont.setItalic(true);
    auto *root = new QTreeWidgetItem();
    root->setText(0, "New section");
    root->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
    root->setFont(0, parentFont);
    setPresetIcon(root, true);
    ui->treeWidget->addTopLevelItem(root);
    ui->treeWidget->setCurrentItem(root);
    root->setFirstColumnSpanned(true);
    updatePresetTable();
}

class params
{
    enum paramIndex  {pDescription, pCodec, };
    QVector<QString> default_values = {
            "Emergency, Res: Source, Fps: Source, YUV, 4:2:2, 10 bit, HDR: Enabled, Audio: PCM 16 bit, MOV",
            "18", "0", "0", "Auto", "Auto", "Auto", "0", "0", "0", "0", "0", "0", "0", "", "", "", "", "0",
            "From source", "From source", "0", "0", "Auto", "0", "0", "0", "0", "0", "0", "Emergency", "0",
            "0", "0"
    };

    QVector<QString> values;

    public: params() {
        clear();
    }

    public: void clear() {
        copy_values_from(default_values);
    }

    public: void copy_values_from(QVector<QString> copy_from) {
        values = QVector<QString>(std::move(copy_from));
        values.detach();
    }
};

void MainWindow::onAddPreset()  // Add preset
{
    if (ui->treeWidget->currentIndex().row() < 0) {
        showPopup(tr("First add a section!\n"), PopupMessage::Icon::Warning);
        return;
    }

    QVector<QString> cur_param = default_preset.toVector();
    cur_param[OUTPUT_PARAM] = "Emergency, Res: Source, Fps: Source, YUV, 4:2:2, 10 bit, HDR: Enabled, Audio: PCM 16 bit, MOV";
    cur_param[SUBTITLE_FONT] = m_subtitles_font;
    cur_param[SUBTITLE_FONT_SIZE] = numToStr(m_subtitles_fontSize);
    cur_param[SUBTITLE_FONT_COLOR] = m_subtitles_color.name();
    cur_param[SUBTITLE_BACKGROUND] = numToStr(m_subtitles_background);
    cur_param[SUBTITLE_BACKGROUND_COLOR] = m_subtitles_background_color.name();
    cur_param[SUBTITLE_BACKGROUND_ALPHA] = m_subtitles_background_alpha ? "1" : "0";
    cur_param[SUBTITLE_LOCATION] = numToStr(m_subtitles_location);
    auto *item = ui->treeWidget->currentItem();
    auto *parentItem = item->parent();
    auto *child = new QTreeWidgetItem();
    for (int k = 0; k < PARAMETERS_COUNT; k++)
        child->setText(k + 7, cur_param[k]);
    updateInfoFields(cur_param[1], cur_param[2], cur_param[3], cur_param[4],
                     cur_param[11], cur_param[12], cur_param[21], child, true);
    setItemStyle(child);
    if (parentItem) {
        // Item is child...
        parentItem->addChild(child);
        const int index_top = ui->treeWidget->indexOfTopLevelItem(parentItem);
        const int index_child = parentItem->indexOfChild(child);
        updateCurPresetPos(index_top, index_child);
    } else {
        // Item is parent...
        item->addChild(child);
        ui->treeWidget->expandItem(item);
        const int index_top = ui->treeWidget->indexOfTopLevelItem(item);
        const int index_child = item->indexOfChild(child);
        updateCurPresetPos(index_top, index_child);
    }
    updatePresetTable();
}

void MainWindow::onRenamePreset()
{
    QTreeWidgetItem *item = ui->treeWidget->currentItem();
    if (item->parent()) {
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
        ui->treeWidget->editItem(item, 0);
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    } else {
        ui->treeWidget->editItem(item, 0);
    }
}

void MainWindow::setItemStyle(QTreeWidgetItem *item) const
{
    QFont font = qApp->font();
    font.setItalic(true);
    QColor foregroundChildColor;
    switch (m_theme) {
    case Theme::GRAY:
    case Theme::DARK:
    case Theme::WAVE:
        foregroundChildColor.setRgb(qRgb(150, 150, 150));
        break;
    case Theme::LIGHT:
        foregroundChildColor.setRgb(qRgb(20, 20, 30));
        break;
    }
    item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    item->setTextAlignment(0, Qt::AlignLeft | Qt::AlignVCenter);
    item->setForeground(0, foregroundChildColor);
    item->setFont(0, font);
    for (int column = 1; column < 7; column++) {
        item->setTextAlignment(column, Qt::AlignCenter);
        item->setForeground(column, foregroundChildColor);
        item->setFont(column, font);
    }
}

void MainWindow::updateCurPresetPos(const int index_top, const int index_child)
{
    Q_UNUSED(index_top)
    Q_UNUSED(index_child)
    m_pos_top = -1;
    m_pos_cld = -1;
    m_curParams[CurParamIndex::OUTPUT_PARAM] = tr("Preset not selected");
    if (m_row != -1)
        get_output_filename();
}

void MainWindow::updateInfoFields(const QString &codec_qstr,
                              const QString &mode_qstr,
                              const QString &container_qstr,
                              const QString &bqr_qstr,
                              const QString &pass_qstr,
                              const QString &preset_qstr,
                              const QString &acodec_qstr,
                              QTreeWidgetItem *item,
                              const bool defaultNameFlag)
{
    Tables t;
    const int codec = codec_qstr.toInt();
    if (defaultNameFlag) {
        item->setText(0, t.arr_codec[codec][0]);
        const QString newPresetName = item->text(0);
        item->setText(30 + 7, newPresetName);
    }
    item->setText(1, t.getCurrentMode(codec, mode_qstr.toInt()));
    item->setText(2, bqr_qstr);
    item->setText(3, t.arr_preset[codec][preset_qstr.toInt()]);
    item->setText(4, t.getCurrentPass(codec, pass_qstr.toInt()));
    item->setText(5, t.arr_acodec[codec][acodec_qstr.toInt()]);
    item->setText(6, t.arr_container[codec][container_qstr.toInt()]);
}

void MainWindow::updatePresetTable()
{
    int CHILD_COUNT = 0;
    const int TOP_LEVEL_ITEMS_COUNT = ui->treeWidget->topLevelItemCount();
    for (int i = 0; i < TOP_LEVEL_ITEMS_COUNT; i++)
        CHILD_COUNT += ui->treeWidget->topLevelItem(i)->childCount();

    const int ROWS_COUNT = TOP_LEVEL_ITEMS_COUNT + CHILD_COUNT;  // Count of all rows
    for (int i = 0; i < PARAMETERS_COUNT + 1; i++)
      m_preset_table[i].resize(ROWS_COUNT);

    int row = 0;
    for (int top = 0; top < TOP_LEVEL_ITEMS_COUNT; top++) {
        m_preset_table[0][row] = ui->treeWidget->topLevelItem(top)->text(0);
        m_preset_table[PARAMETERS_COUNT][row] = "TopLevelItem";
        CHILD_COUNT = ui->treeWidget->topLevelItem(top)->childCount();
        for (int child = 0; child < CHILD_COUNT; child++) {
            row++;
            for (int column = 0; column < PARAMETERS_COUNT; column++)
                m_preset_table[column][row] = ui->treeWidget->topLevelItem(top)->child(child)->text(column+7);
            m_preset_table[PARAMETERS_COUNT][row] = "ChildItem";
        }
        row++;
    }
    Print(m_preset_table[0].size() << " x " << m_preset_table.size());
}

void MainWindow::setPresetIcon(QTreeWidgetItem *item, bool collapsed) const
{
    QString file;
    switch (m_theme) {
    case Theme::GRAY:
    case Theme::DARK:
    case Theme::WAVE:
        file = (collapsed) ? QString::fromUtf8("folder.svg") :
                             QString::fromUtf8("folder_open.svg");
        break;
    case Theme::LIGHT:
        file = (collapsed) ? QString::fromUtf8("folder_light.svg") :
                             QString::fromUtf8("folder_open_light.svg");
        break;
    }
    QIcon icon = QIcon(QPixmap(":/resources/icons/svg/" + file));
    item->setIcon(0, icon);
}

void MainWindow::onTreeCollapsed(QTreeWidgetItem *item)
{
    if (item)
        setPresetIcon(item, true);
}

void MainWindow::onTreeExpanded(QTreeWidgetItem *item)
{
    if (item)
        setPresetIcon(item, false);
}

void MainWindow::onTreeChanged(QTreeWidgetItem *item, int column)
{
    if (item->isSelected() && column == 0) {
        if (item->parent()) {
            const QString newPresetName = item->text(0);
            item->setText(30 + 7, newPresetName);
        }
        updatePresetTable();
    }
}

void MainWindow::onTreeDblClicked(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column)
    if (item->parent())
        onApplyPreset();
}

void MainWindow::providePresetContextMenu(const QPoint &pos) // Call tree items menu
{
    QTreeWidgetItem *item = ui->treeWidget->itemAt(pos);
    if (item) {
        const QPoint globPos = ui->treeWidget->mapToGlobal(pos + QPoint(0, 35));
        item->parent() ? m_pPresetMenu->exec(globPos) : m_pSectionMenu->exec(globPos);
    }
}

/************************************************
** Messages
************************************************/

bool MainWindow::showDialogMessage(const QString &message)
{
    Message msg(this, MessType::DIALOG, message);
    if (msg.exec() == Dialog::Accept)
        return true;
    return false;
}

void MainWindow::showPopup(const QString &text, PopupMessage::Icon icon)
{
    auto *msg = new PopupMessage(this, icon, text);
    msg->show();
    addReport(text, static_cast<ReportLog::Icon>(icon));
}

void MainWindow::addReport(const QString &log, ReportLog::Icon icon)
{
    const int ind = m_reportLog.size();
    m_reportLog.resize(ind + 1);
    const QString time = QTime::currentTime().toString();
    m_reportLog[ind].reportIcon = icon;
    m_reportLog[ind].reportTime = time;
    m_reportLog[ind].reportMsg = log;
}

void MainWindow::showInfoMessage(const QString &message, const bool timer_mode)
{
    auto showMessage = [this, message, timer_mode]() {
        ReportLog::Icon icon = timer_mode ? ReportLog::Icon::Info :
                                            ReportLog::Icon::Critical;
        addReport(message, icon);
        Message msg(this, MessType::INFO, message, timer_mode);
        msg.exec();
    };
    if (isHidden()) {
        if (m_hideInTrayFlag && !timer_mode) {
            m_pTrayIcon->showMessage(message, tr("Task"), QSystemTrayIcon::Information, 151000);
        } else
        if (timer_mode) {
            show();
            showMessage();
        }
    } else {
        showMessage();
    }
}

/************************************************
** Extract
************************************************/

void MainWindow::onExtract(QStreamView::Content type, int num)
{
    const float duration = (type == QStreamView::Content::Audio) ?
                           m_data[m_row].fields[Data::audioDuration].at(num).toFloat() :
                           m_data[m_row].fields[Data::subtDuration].at(num).toFloat();
    StreamData data;
    data.cont_type = ContentType(type);
    data.input_file = m_input_file;
    data.output_file = m_output_file;
    data.duration = 0.001f * duration;
    data.stream = num;
    StreamConverter ext(this,
                        StreamConverter::Mode::Extract,
                        &data,
                        m_theme,
                        m_ffmpeg_prio);
    if (ext.exec() == QDialog::Accepted) {
        showPopup(tr("Task completed!\n"));
    }
}

void MainWindow::updateFileIncompatibilityStatus(int fileRow)
{
    if (fileRow < 0 || fileRow >= ui->tableWidget->rowCount() || fileRow >= m_data.size())
        return;

    // Get preset parameters for this specific file, fallback to global if not set
    QVector<QString> *params = &m_curParams;
    if (fileRow < m_data.size() && !m_data[fileRow].presetParams.isEmpty()) {
        params = &m_data[fileRow].presetParams;
    }
    
    // Get current container extension and preset parameters
    Tables t;
    QString extension = t.arr_container[(*params)[CurParamIndex::CODEC].toInt()][(*params)[CurParamIndex::CONTAINER].toInt()].toLower();
    int codecIndex = (*params)[CurParamIndex::CODEC].toInt();
    int audioCodecIndex = (*params)[CurParamIndex::AUDIO_CODEC].toInt();
    QString targetAudioCodec = t.arr_acodec[codecIndex][audioCodecIndex];
    bool usePresetSubtitleSettings = (*params)[CurParamIndex::USE_PRESET_SUBTITLE_SETTINGS].toInt() == 1;
    
    bool hasIncompatibleStreams = false;
    bool hasSelectedIncompatibleStreams = false;
    
    // Check audio streams
    for (int i = 0; i < m_data[fileRow].fields[Data::audioFormats].size(); i++) {
        const QString& audioFormat = m_data[fileRow].fields[Data::audioFormats][i];
        if (Helper::isAudioIncompatible(extension, audioFormat, targetAudioCodec)) {
            hasIncompatibleStreams = true;
            if (i < m_data[fileRow].checks[Data::audioChecks].size() && 
                m_data[fileRow].checks[Data::audioChecks][i]) {
                hasSelectedIncompatibleStreams = true;
                break;
            }
        }
    }
    
    // Check subtitle streams if no selected incompatible audio streams yet
    if (!hasSelectedIncompatibleStreams) {
        for (int i = 0; i < m_data[fileRow].fields[Data::subtFormats].size(); i++) {
            const QString& subtitleFormat = m_data[fileRow].fields[Data::subtFormats][i];
            if (Helper::isSubtitleIncompatible(extension, subtitleFormat, usePresetSubtitleSettings)) {
                hasIncompatibleStreams = true;
                if (i < m_data[fileRow].checks[Data::subtChecks].size() && 
                    m_data[fileRow].checks[Data::subtChecks][i]) {
                    hasSelectedIncompatibleStreams = true;
                    break;
                }
            }
        }
    }
    
    // Apply styling to the filename cell
    QTableWidgetItem* filenameItem = ui->tableWidget->item(fileRow, ColumnIndex::FILENAME);
    if (filenameItem) {
        if (hasSelectedIncompatibleStreams) {
            // Yellow background for files with selected incompatible streams
            filenameItem->setBackground(QColor("#fff3cd"));
            filenameItem->setIcon(style()->standardIcon(QStyle::SP_MessageBoxWarning));
            filenameItem->setToolTip(tr("WARNING: This file has incompatible streams selected for output. This will cause encoding issues!"));
        } else if (hasIncompatibleStreams) {
            // Only warning icon for files with unselected incompatible streams
            filenameItem->setBackground(QColor());
            filenameItem->setIcon(style()->standardIcon(QStyle::SP_MessageBoxWarning));
            filenameItem->setToolTip(tr("This file contains streams that are incompatible with the current preset container format."));
        } else {
            // Reset styling for compatible files
            filenameItem->setBackground(QColor());
            filenameItem->setIcon(QIcon());
            filenameItem->setToolTip("");
        }
    }
}


/************************************************
** Column Visibility Management
************************************************/

void MainWindow::setupColumnVisibilityMenus()
{
    // Initialize column names array
    m_columnNames = {
        tr("File path"), tr("Format"), tr("Resolution"), tr("Duration"), tr("FPS"), tr("AR"), tr("Status"), tr("Preset"),
        tr("Bitrate"), tr("Subsampling"), tr("Bit depth"), tr("Color space"), tr("Color range"), tr("Color prim"),
        tr("Color mtrx"), tr("Transfer"), tr("Max lum"), tr("Min lum"), tr("Max CLL"), tr("Max Fall"), tr("Master display"),
        tr("Path"), tr("Duration (technical)"), tr("Chroma coord"), tr("White coord"), tr("Stream size"), 
        tr("Width (technical)"), tr("Height (technical)"), tr("Start Time"), tr("End Time"), tr("ID")
    };

    // Create Columns menu
    m_pColumnsMenu = new QMenu(tr("Columns"), this);
    
    // Create header context menu
    m_pHeaderContextMenu = new QMenu(this);
    
    // Create column visibility actions
    for (int i = 0; i < m_columnNames.size(); ++i) {
        auto *action = new QAction(m_columnNames[i], this);
        action->setCheckable(true);
        action->setData(i);
        connect(action, &QAction::triggered, this, &MainWindow::onToggleColumnVisibility);
        
        m_columnActions.append(action);
        m_pColumnsMenu->addAction(action);
        m_pHeaderContextMenu->addAction(action);
    }
}

void MainWindow::provideHeaderContextMenu(const QPoint& pos)
{
    updateColumnVisibilityMenus();
    m_pHeaderContextMenu->exec(ui->tableWidget->horizontalHeader()->mapToGlobal(pos));
}

void MainWindow::onToggleColumnVisibility()
{
    auto *action = qobject_cast<QAction*>(sender());
    if (!action) return;
    
    int column = action->data().toInt();
    bool visible = action->isChecked();
    
    ui->tableWidget->setColumnHidden(column, !visible);
    
    // Save the setting
    CONFIG.setBool(QString("table/column_visible_%1").arg(column), visible);
}

void MainWindow::loadColumnVisibilitySettings()
{
    for (int i = 0; i < 31; ++i) {
        bool visible = CONFIG.getBool(QString("table/column_visible_%1").arg(i), 
                                    i < 9); // First 9 columns visible by default
        ui->tableWidget->setColumnHidden(i, !visible);
    }
    
    // Only update menus if they've been initialized
    if (!m_columnActions.isEmpty()) {
        updateColumnVisibilityMenus();
    }
}

void MainWindow::saveColumnVisibilitySettings()
{
    for (int i = 0; i < 31; ++i) {
        bool visible = !ui->tableWidget->isColumnHidden(i);
        CONFIG.setBool(QString("table/column_visible_%1").arg(i), visible);
    }
}

void MainWindow::updateColumnVisibilityMenus()
{
    for (int i = 0; i < m_columnActions.size(); ++i) {
        bool visible = !ui->tableWidget->isColumnHidden(i);
        m_columnActions[i]->setChecked(visible);
    }
}
