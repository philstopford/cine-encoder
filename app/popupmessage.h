/***********************************************************************

                          C I N E   E N C O D E R
                                JULY, 2020
                            COPYRIGHT (C) 2020

 FILE: popupmessage.h
 COMMENT:
 LICENSE: GNU General Public License v3.0

***********************************************************************/

#ifndef POPUPMESSAGE_H
#define POPUPMESSAGE_H

#include "basedialog.h"

#include <QEvent>


namespace Ui
{
    class PopupMessage;
}

class PopupMessage : public BaseDialog
{
    Q_OBJECT
public:
    enum class Icon : uchar {
        Info, Warning, Critical
    };
    explicit PopupMessage(QWidget *parent, Icon icon, const QString &text);
    ~PopupMessage() override;

private:
    enum class EffectType : uchar {
        Arise, Fade
    };
    bool eventFilter(QObject*, QEvent*) final;
    void showEvent(QShowEvent*) final;
    void moveWidget();
    void showEffect(EffectType efType);
    Ui::PopupMessage *ui;
    QWidget *ui_widget;
    bool m_activated;
};

#endif // POPUPMESSAGE_H
