/***********************************************************************

                          C I N E   E N C O D E R
                                JULY, 2020
                            COPYRIGHT (C) 2020

 FILE: uiconnectionhelper.h
 COMMENT: Helper class for common UI connection patterns
 LICENSE: GNU General Public License v3.0

***********************************************************************/

#ifndef UICONNECTIONHELPER_H
#define UICONNECTIONHELPER_H

#include <QObject>
#include <QPushButton>
#include <QComboBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QList>
#include <functional>

/**
 * @brief Helper class for common UI connection patterns
 * 
 * This class provides static methods to reduce repetitive UI connection code
 * and separate UI setup concerns from business logic.
 */
class UIConnectionHelper
{
public:
    // Connect array of buttons to array of methods using function pointers
    template<typename T>
    static void connectButtons(QPushButton* buttons[], 
                             void (T::*methods[])(), 
                             int count, 
                             T* receiver);

    // Connect array of combo boxes to array of methods with int parameter
    template<typename T>
    static void connectComboBoxes(QComboBox* comboBoxes[],
                                void (T::*methods[])(int),
                                int count,
                                T* receiver);

    // Connect array of combo boxes to array of methods with string parameter
    template<typename T>
    static void connectComboBoxesText(QComboBox* comboBoxes[],
                                    void (T::*methods[])(const QString&),
                                    int count,
                                    T* receiver);

    // Connect tab buttons with automatic enable/disable logic
    static void connectTabButtons(const QList<QPushButton*>& tabButtons,
                                QObject* receiver,
                                std::function<void(int)> tabSwitchFunc);

    // Generic connection helper for reducing boilerplate
    template<typename Sender, typename Signal, typename Receiver, typename Slot>
    static void connectSafely(Sender* sender, Signal signal, Receiver* receiver, Slot slot);

private:
    UIConnectionHelper() = default; // Static class
};

// Template implementations
template<typename T>
void UIConnectionHelper::connectButtons(QPushButton* buttons[], 
                                      void (T::*methods[])(), 
                                      int count, 
                                      T* receiver)
{
    for (int i = 0; i < count; i++) {
        if (buttons[i] && methods[i]) {
            QObject::connect(buttons[i], &QPushButton::clicked, receiver, methods[i]);
        }
    }
}

template<typename T>
void UIConnectionHelper::connectComboBoxes(QComboBox* comboBoxes[],
                                         void (T::*methods[])(int),
                                         int count,
                                         T* receiver)
{
    for (int i = 0; i < count; i++) {
        if (comboBoxes[i] && methods[i]) {
            QObject::connect(comboBoxes[i], 
                           static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
                           receiver, methods[i]);
        }
    }
}

template<typename T>
void UIConnectionHelper::connectComboBoxesText(QComboBox* comboBoxes[],
                                             void (T::*methods[])(const QString&),
                                             int count,
                                             T* receiver)
{
    for (int i = 0; i < count; i++) {
        if (comboBoxes[i] && methods[i]) {
            QObject::connect(comboBoxes[i], &QComboBox::currentTextChanged, receiver, methods[i]);
        }
    }
}

template<typename Sender, typename Signal, typename Receiver, typename Slot>
void UIConnectionHelper::connectSafely(Sender* sender, Signal signal, Receiver* receiver, Slot slot)
{
    if (sender && receiver) {
        QObject::connect(sender, signal, receiver, slot);
    }
}

#endif // UICONNECTIONHELPER_H