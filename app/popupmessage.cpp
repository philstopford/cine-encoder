/***********************************************************************

                          C I N E   E N C O D E R
                                JULY, 2020
                            COPYRIGHT (C) 2020

 FILE: popupmessage.cpp
 COMMENT:
 LICENSE: GNU General Public License v3.0

***********************************************************************/

#include "popupmessage.h"
#include "ui_popupmessage.h"
#include "helper.h"
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QLayout>
#include <QTextBrowser>
#include <QPushButton>
#include <QTimer>

#define OFFSET (QPoint(374, -20) * Helper::scaling())


PopupMessage::PopupMessage(QWidget *parent, Icon icon, const QString &text) :
    BaseDialog(parent, false),
    ui(new Ui::PopupMessage),
    m_activated(false)
{
    setWindowFlags((windowFlags() & ~Qt::Dialog & ~Qt::SubWindow) | Qt::Tool);
    setWindowModality(Qt::NonModal);
    ui_widget = new QWidget(this);
    layout()->addWidget(ui_widget);
    ui->setupUi(ui_widget);
    setMaskWidget(ui_widget);
    ui_widget->setAttribute(Qt::WA_TranslucentBackground);
    ui_widget->setAutoFillBackground(true);

    QPixmap pxm(":/resources/icons/svg/popup.svg");
    ui->imageLabel->setPixmap(pxm.scaled(ui_widget->size() * Helper::scaling(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    auto *lt = new QGridLayout(ui->imageLabel);
    lt->setContentsMargins(12 * Helper::scaling(),
                           34 * Helper::scaling(),
                           6 * Helper::scaling(),
                           6 * Helper::scaling());
    lt->setSpacing(6 * Helper::scaling());
    ui->imageLabel->setLayout(lt);

    auto *lab = new QLabel(ui->imageLabel);
    lab->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    lab->setFixedSize(QSize(30, 30)* Helper::scaling());

    QString iconPath(":/resources/icons/svg/info.svg");
    switch (icon) {
    case Icon::Warning:
        iconPath = QString(":/resources/icons/svg/warning.svg");
        break;
    case Icon::Critical:
        iconPath = QString(":/resources/icons/svg/error.svg");
        break;
    default:
        break;
    }
    QPixmap logo(iconPath);
    lab->setPixmap(logo.scaled(QSize(30,30) * Helper::scaling(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    lt->addWidget(lab, 0, 0, Qt::AlignTop);

    auto *br = new QTextBrowser(ui->imageLabel);
    br->setEnabled(false);
    br->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    br->setWordWrapMode(QTextOption::WordWrap);
    br->setLineWrapMode(QTextEdit::WidgetWidth);
    br->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    br->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    br->setPlainText(text);
    br->setStyleSheet("color: #303030; background-color: transparent; border: none;");
    lt->addWidget(br, 0, 1);
    
    // Calculate optimal size based on text content with proper measurement
    const int maxWidth = 450 * Helper::scaling(); // Increased maximum popup width
    const int minWidth = 275 * Helper::scaling(); // Minimum popup width
    const int iconSpace = 48 * Helper::scaling(); // Space for icon and margins
    const int layoutMargins = 24 * Helper::scaling(); // Total layout margins
    
    // Calculate available space for text
    const int availableTextWidth = maxWidth - iconSpace - layoutMargins;
    
    // Create a temporary QTextDocument to measure text properly
    QTextDocument tempDoc;
    tempDoc.setDefaultFont(br->font());
    tempDoc.setTextWidth(availableTextWidth);
    tempDoc.setPlainText(text);
    
    // Get the actual size needed for the text
    QSizeF docSize = tempDoc.size();
    const int textPadding = 16 * Helper::scaling(); // Padding for better appearance
    
    // Calculate optimal dimensions
    int optimalWidth = qMax(minWidth, qMin(maxWidth, 
                           static_cast<int>(docSize.width()) + iconSpace + layoutMargins + textPadding));
    int optimalHeight = qMax(static_cast<int>(115 * Helper::scaling()), 
                            static_cast<int>(docSize.height()) + static_cast<int>(80 * Helper::scaling()) + textPadding);
    
    // Set explicit size for the text browser to ensure proper text fitting
    const int textBrowserWidth = optimalWidth - iconSpace - layoutMargins;
    br->setMinimumWidth(textBrowserWidth);
    br->setMaximumWidth(textBrowserWidth);
    
    // Set the calculated size for the popup
    ui_widget->setMinimumSize(optimalWidth, optimalHeight);
    ui_widget->setMaximumSize(optimalWidth, optimalHeight);
    resize(optimalWidth, optimalHeight);

    auto *tmr = new QTimer(this);
    tmr->setSingleShot(false);
    tmr->setInterval(250);
    connect(tmr, &QTimer::timeout, this, &PopupMessage::moveWidget);
    tmr->start();
}

PopupMessage::~PopupMessage()
{
    delete ui;
}

bool PopupMessage::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress) {
        this->deleteLater();
        return true;
    }
    return BaseDialog::eventFilter(obj, event);
}

void PopupMessage::showEvent(QShowEvent *event)
{
    BaseDialog::showEvent(event);
    if (!m_activated) {
        m_activated = true;
        moveWidget();
        //showEffect(EffectType::Arise);
        QTimer::singleShot(5000, this, [this]() {
            showEffect(EffectType::Fade);
        });
    }
}

void PopupMessage::moveWidget()
{
    if (parentWidget()) {
        auto cw = parentWidget()->findChild<QWidget*>("centralwidget");
        if (cw)
            move(cw->mapToGlobal(cw->geometry().topRight()) - OFFSET);
    }
}

void PopupMessage::showEffect(const EffectType efType)
{
    if (efType == EffectType::Arise) {
        /*anm->setDuration(800);
        anm->setStartValue(0);
        anm->setEndValue(1);
        anm->setEasingCurve(QEasingCurve::InCurve);*/
    } else
    if (efType == EffectType::Fade) {
        auto *m_pGrEffect = new QGraphicsOpacityEffect(ui_widget);
        ui_widget->setGraphicsEffect(m_pGrEffect);
        auto *anm = new QPropertyAnimation(m_pGrEffect, "opacity");
        anm->setDuration(2000);
        anm->setStartValue(1);
        anm->setEndValue(0);
        connect(anm, &QPropertyAnimation::finished, this, [this](){
            this->deleteLater();
        });
        anm->start(QPropertyAnimation::DeleteWhenStopped);
    }
}
