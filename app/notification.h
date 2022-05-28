/***********************************************************************

                          C I N E   E N C O D E R
                                JULY, 2020
                            COPYRIGHT (C) 2020

 FILE: donate.h
 COMMENT:
 LICENSE: GNU General Public License v3.0

***********************************************************************/

#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <QtGlobal>
#ifdef Q_OS_WIN
    #include "platform_win/basewindow.h"
#else
    #include "platform_unix/basewindow.h"
#endif


enum class MessConf : uint8_t {
    CloseOnly, AllBtns
};

namespace Ui
{
    class Notification;
}

class Notification : public BaseWindow
{
    Q_OBJECT
public:
    explicit Notification(QWidget *parent, MessConf mess_conf, const QString &title);
    ~Notification();

private slots:
    void onButtonPayPal();
    void onButtonBitcoin();
    void onCloseWindow();

private:
    void setMessage();
    Ui::Notification *ui;
    MessConf m_mess_conf;
};

#endif // NOTIFICATION_H