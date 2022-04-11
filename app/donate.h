/***********************************************************************

                          C I N E   E N C O D E R
                                JULY, 2020
                            COPYRIGHT (C) 2020

 FILE: donate.h
 COMMENT:
 LICENSE: GNU General Public License v3.0

***********************************************************************/

#ifndef DONATE_H
#define DONATE_H

#include <QDesktopServices>
#include <QUrl>
#include <QMouseEvent>
#include "framelesswindow.h"


namespace Ui
{
    class Donate;
}


class Donate : public FramelessWindow
{
    Q_OBJECT

public:

    explicit Donate(QWidget *parent = nullptr);
    ~Donate();

    void setParameters();

private slots:

    void on_buttonCancel_clicked();
    void on_buttonPayPal_clicked();
    void on_buttonBitcoin_clicked();
    void on_closeWindow_clicked();
    bool eventFilter(QObject *watched, QEvent *event);

private:

    Ui::Donate *ui;

    bool _clickPressedFlag;

    QPoint _mouseClickCoordinate;
};

#endif // DONATE_H
