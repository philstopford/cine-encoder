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
    br->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    br->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    lt->addWidget(br, 0, 1);
    br->setText(text);
    br->setStyleSheet("color: #303030");

    // Calculate optimal size based on text content
    QFontMetrics fm(br->font());
    const int maxWidth = 400 * Helper::scaling(); // Maximum popup width
    const int minWidth = 275 * Helper::scaling(); // Minimum popup width
    const int iconSpace = 48 * Helper::scaling(); // Space for icon and margins
    const int margins = 24 * Helper::scaling(); // Total margins
    
    // Calculate text dimensions
    QRect textRect = fm.boundingRect(QRect(0, 0, maxWidth - iconSpace - margins, 0), 
                                    Qt::TextWordWrap, text);
    
    int optimalWidth = qMax(minWidth, qMin(maxWidth, textRect.width() + iconSpace + margins));
    int optimalHeight = qMax(115 * Helper::scaling(), 
                            textRect.height() + 80 * Helper::scaling()); // 80 for icon area + margins
    
    // Set the calculated size
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
