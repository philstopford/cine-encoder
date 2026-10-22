/***********************************************************************

                          C I N E   E N C O D E R
                                JULY, 2020
                            COPYRIGHT (C) 2020

 FILE: main.cpp
 COMMENT:
 LICENSE: GNU General Public License v3.0

***********************************************************************/

#include <QApplication>
#include <QCoreApplication>
#include <QSplashScreen>
#include <QElapsedTimer>
#include <QFontDatabase>
#include <QStyleFactory>
#include <QMessageBox>
#include <QTranslator>
#include <iostream>
#include <QXmlStreamReader>
#include <QLockFile>
#include <QDir>
#include "mainwindow.h"
#include "helper.h"
#include "ffmpegcompatibility.h"


QString readXMLSettingFromFile(const QString& tagToFind);

int main(int argc, char *argv[])
{
#if defined(Q_OS_MACOS)
    const QString currentPath = QString::fromLocal8Bit(qgetenv("PATH"));
    const QString extraPaths = QStringLiteral("/opt/homebrew/bin:/usr/local/bin");
    if (!currentPath.contains(QStringLiteral("/opt/homebrew/bin"))) {
        qputenv("PATH", (extraPaths + QStringLiteral(":") + currentPath).toLocal8Bit());
    }
#elif defined(Q_OS_UNIX)
    qputenv("LC_ALL", "en_US.UTF8");
    qputenv("QT_QPA_PLATFORM", "xcb");
    qputenv("QT_LOGGING_RULES", "*.debug=false;qt.qpa.*=false");
#endif
    QCoreApplication::setAttribute(Qt::AA_UseStyleSheetPropagationInWidgetStyles, true);
    QCoreApplication::setOrganizationName(QString::fromUtf8("CineEncoder"));
    QCoreApplication::setApplicationName(QString::fromUtf8("Cine Encoder"));
    QApplication::setApplicationDisplayName("Cine Encoder");
    QCoreApplication::setApplicationVersion("3.7");
    QGuiApplication::setDesktopFileName("CineEncoder");
    QApplication app(argc, argv);
    QApplication::setWindowIcon(QIcon(":/resources/icons/svg/cine-encoder.svg"));
    QApplication::setStyle(QStyleFactory::create("Fusion"));
    /*const int id = QFontDatabase::addApplicationFont(":/resources/fonts/interregular.otf");
    QString sysFamily = app.font().family();
    if (!QFontDatabase::applicationFontFamilies(id).isEmpty())
        sysFamily = QFontDatabase::applicationFontFamilies(id).at(0);*/

    /******************* Read Settings ****************************/
    QString settingValue = readXMLSettingFromFile("allow_duplicates");
    if (settingValue.isEmpty()) {
        settingValue = "0";
    }
    const bool allowDuplicates = settingValue.toInt();

    settingValue = readXMLSettingFromFile("font_size");
    if (settingValue.isEmpty()) {
        settingValue = QString(numToStr(FONTSIZE));
    }
    const int fntSize = settingValue.toInt();

    const QString fntFamily = readXMLSettingFromFile("font");
    const QString currLang = readXMLSettingFromFile("language");

    /**************** Check for duplicates ************************/
    QLockFile lockFile(QDir::tempPath() + "/cine_encoder.lock");
    if (!allowDuplicates) {
        if (!lockFile.tryLock(100)) {
            QMessageBox msgBox(nullptr);
            msgBox.setWindowTitle("Cine Encoder");
            msgBox.setWindowIcon(QIcon(":/resources/icons/64x64/cine-encoder.png"));
            msgBox.setIcon(QMessageBox::Critical);
            msgBox.setText(QObject::tr("The program is already running!"));
            msgBox.exec();
            return 1;
        }
    }

    /******************* Set Translate ****************************/
    QTranslator trns;
    if (currLang != "en" && trns.load(QString(":/resources/translation/translation_%1.qm").arg(currLang))) {
        QApplication::installTranslator(&trns);
    }

    const auto subtitleFilterStatus = FFmpegCompatibility::subtitleFilterStatus();
    if (subtitleFilterStatus != FFmpegCompatibility::SubtitleFilterStatus::Available) {
        QString detail;
        if (subtitleFilterStatus == FFmpegCompatibility::SubtitleFilterStatus::Missing) {
            detail = QObject::tr("The installed FFmpeg does not include the subtitles filter. "
                                 "Hard-burning text subtitles will be unavailable.");
        } else {
            detail = QObject::tr("Cine Encoder could not query the installed FFmpeg. "
                                 "Hard-burning text subtitles may be unavailable.");
        }
#if defined(Q_OS_MACOS)
        detail += QObject::tr("\n\nInstall Homebrew's full build and make it active:\n"
                              "brew install ffmpeg-full\n"
                              "brew unlink ffmpeg && brew link --overwrite ffmpeg-full");
#endif
        QMessageBox::warning(nullptr, QObject::tr("FFmpeg subtitle support unavailable"), detail);
    }

    /********************* Set Font ******************************/
    QFont fnt = QApplication::font();
    if (!fntFamily.isEmpty()) {
        fnt.setFamily(fntFamily);
    }
    fnt.setPointSize(fntSize);
#if !defined(Q_OS_MACOS)
    fnt.setWeight(QFont::Medium);
#endif
    QApplication::setFont(fnt);

    /******************* Set Splash *******************************/
    const int SPLASH_DISPLAY_MS = 1000;
    const QPixmap pixmap(":/resources/images/splash.png");
    const QPixmap scaled = pixmap.scaled(pixmap.size() * Helper::scaling(),
                                         Qt::KeepAspectRatio, Qt::FastTransformation);
    auto *splash = new QSplashScreen(scaled);
    splash->show();
    QApplication::processEvents();

    QElapsedTimer time;
    time.start();
    while (time.elapsed() < SPLASH_DISPLAY_MS) {
        QApplication::processEvents();
    }

    /******************* Set Window *******************************/
    Helper::detectEnv();
    MainWindow window;
    window.show();
    splash->finish(&window);
    delete splash;
    return QApplication::exec();
}

QString readXMLSettingFromFile(const QString& tagToFind) {
    QString value;
    QFile xmlFile(XMLSETTINGSFILE);
    bool settingsXMLFileValid = true;
    int settingsVer = 0;
    if (!xmlFile.open(QFile::ReadOnly | QFile::Text)) { // Open file in read only mode
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
                if (tagName == tagToFind) {
                    value = stream.readElementText();
                    xmlFile.close();
                    return value;
                }
            }
        }
    }
    xmlFile.close();
    return value;
}
