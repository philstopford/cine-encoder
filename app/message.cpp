/***********************************************************************

                          C I N E   E N C O D E R
                                JULY, 2020
                            COPYRIGHT (C) 2020

 FILE: message.cpp
 COMMENT:
 LICENSE: GNU General Public License v3.0

***********************************************************************/

#include "message.h"
#include "ui_message.h"
#include "helper.h"
#include <QSoundEffect>
#include <utility>

#define ELAPSED_TIME 25


Message::Message(QWidget *parent, MessType mess_type,
                 QString message, const bool timer_flag) :
    BaseWindow(parent, true),
    ui(new Ui::Message),
    m_message(std::move(message)),
    m_mess_type(mess_type),
    m_elps_t(0),
    m_windowActivated(false),
    m_timer_flag(timer_flag)
{
    ui->setupUi(centralWidget());
    setTitleBar(ui->frame_top);
    ui->frame_main->setProperty("scale", int(Helper::scaling() * 100));
    connect(ui->closeWindow, &QPushButton::clicked, this, &Message::onCloseWindow);
    connect(ui->buttonApply, &QPushButton::clicked, this, &Message::onButtonApply);
    Icon icon = Icon::Info;
    if (m_mess_type == MessType::INFO) {
        ui->spacerCancel->changeSize(0,0);
        ui->buttonCancel->hide();
        ui->buttonCancel->setFixedWidth(0);
        icon = Icon::Warning;
    } else {
        connect(ui->buttonCancel, &QPushButton::clicked, this, &Message::onCloseWindow);
    }    
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
    ui->labelWindowIcon->setPixmap(logo.scaled(QSize(20, 20), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

Message::~Message()
{
    delete ui;
}

void Message::onButtonApply()
{
    acceptDialog();
}

void Message::onCloseWindow()
{
    closeDialog();
}

void Message::showEvent(QShowEvent *event)
{
    BaseWindow::showEvent(event);
    if (!m_windowActivated) {
        m_windowActivated = true;
        setMessage();
        
        // Calculate optimal size based on text content
        QFontMetrics fm(ui->textBrowser->font());
        const int maxWidth = 600 * Helper::scaling();
        const int minWidth = 300 * Helper::scaling();
        const int margins = 60 * Helper::scaling(); // Total margins including title bar
        const int buttonHeight = 80 * Helper::scaling(); // Space for buttons and spacing
        
        // Calculate text dimensions with better text measurement
        const int availableWidth = maxWidth - margins;
        QRect textRect = fm.boundingRect(QRect(0, 0, availableWidth, 0), 
                                        Qt::TextWordWrap | Qt::AlignCenter, m_message);
        
        // Add some padding to ensure text fits properly
        const int textPadding = 20 * Helper::scaling();
        int optimalWidth = qMax(minWidth, qMin(maxWidth, textRect.width() + margins + textPadding));
        int optimalHeight = qMax(static_cast<int>(165 * Helper::scaling()), 
                                textRect.height() + buttonHeight + textPadding);
        
        resize(optimalWidth, optimalHeight);
        
        // Center the dialog
        QSizeF size(this->size());
        QPoint center = QPointF(size.width()/2, size.height()/2).toPoint();
        move(parentWidget()->geometry().center() - center);
    }
}

void Message::setMessage()
{
    QFont font;
    font.setPointSize(10);
    ui->label_title->setFont(font);
    if (m_mess_type == MessType::INFO && m_timer_flag) {
        show_message();
        m_elps_t = ELAPSED_TIME;
        m_timer = new QTimer(this);
        m_timer->setInterval(1000);
        connect(m_timer, &QTimer::timeout, this, &Message::repeatHandler);
        m_timer->start();
    } else
    if (m_mess_type == MessType::INFO && !m_timer_flag) {
#if defined (Q_OS_WIN64)
    {
        QSoundEffect effect;
        effect.setSource(QUrl::fromLocalFile("./cine-encoder.wav"));
        effect.play();
    }
#elif defined (Q_OS_UNIX)
        QSoundEffect se;
        se.setSource(QUrl::fromLocalFile("/usr/share/sounds/cine-encoder.wav"));
        se.play();
#endif
        show_message();
    } else {
        show_message();
    }
}

void Message::show_message()
{
    ui->textBrowser->clear();
    ui->textBrowser->setAlignment(Qt::AlignCenter);
    
    // Ensure proper word wrapping is enabled
    ui->textBrowser->setWordWrapMode(QTextOption::WordWrap);
    ui->textBrowser->setLineWrapMode(QTextEdit::WidgetWidth);
    
    // Use setPlainText instead of append for better text handling
    ui->textBrowser->setPlainText(m_message);
    
    QTextCursor textCursor = ui->textBrowser->textCursor();
    textCursor.movePosition(QTextCursor::Start);
    ui->textBrowser->setTextCursor(textCursor);
}

void Message::repeatHandler()
{
    if (m_elps_t == 0)
        close();
    m_message = QString("%1\n\n %2: %3").arg(tr("Pause"), tr("Resume after:"),
                Helper::timeConverter(float(m_elps_t)));
    show_message();
    m_elps_t--;
}
