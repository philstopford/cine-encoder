/***********************************************************************

                          C I N E   E N C O D E R
                                JULY, 2020
                            COPYRIGHT (C) 2020

 FILE: uiconnectionhelper.cpp
 COMMENT: Helper class implementation for common UI patterns
 LICENSE: GNU General Public License v3.0

***********************************************************************/

#include "uiconnectionhelper.h"
#include <QObject>

void UIConnectionHelper::connectTabButtons(const QList<QPushButton*>& tabButtons,
                                         QObject* receiver,
                                         std::function<void(int)> tabSwitchFunc)
{
    for (int i = 0; i < tabButtons.size(); i++) {
        if (tabButtons[i]) {
            QObject::connect(tabButtons[i], &QPushButton::clicked, receiver, [i, tabButtons, tabSwitchFunc]() {
                // Disable all other tabs, enable current one
                for (int j = 0; j < tabButtons.size(); j++) {
                    if (tabButtons[j]) {
                        tabButtons[j]->setEnabled(i != j);
                    }
                }
                // Call the tab switch function
                tabSwitchFunc(i);
            });
        }
    }
}