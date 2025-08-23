/********************************************************************************
** Form generated from reading UI file 'popupmessage.ui'
**
** Created by: Qt User Interface Compiler version 6.4.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_POPUPMESSAGE_H
#define UI_POPUPMESSAGE_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PopupMessage
{
public:
    QGridLayout *gridLayout;
    QLabel *imageLabel;

    void setupUi(QWidget *PopupMessage)
    {
        if (PopupMessage->objectName().isEmpty())
            PopupMessage->setObjectName("PopupMessage");
        PopupMessage->resize(275, 115);
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(PopupMessage->sizePolicy().hasHeightForWidth());
        PopupMessage->setSizePolicy(sizePolicy);
        PopupMessage->setMinimumSize(QSize(275, 115));
        PopupMessage->setWindowTitle(QString::fromUtf8("Message"));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/resources/icons/svg/cine-encoder.svg"), QSize(), QIcon::Normal, QIcon::Off);
        PopupMessage->setWindowIcon(icon);
        gridLayout = new QGridLayout(PopupMessage);
        gridLayout->setSpacing(0);
        gridLayout->setObjectName("gridLayout");
        gridLayout->setContentsMargins(0, 0, 0, 0);
        imageLabel = new QLabel(PopupMessage);
        imageLabel->setObjectName("imageLabel");
        sizePolicy.setHeightForWidth(imageLabel->sizePolicy().hasHeightForWidth());
        imageLabel->setSizePolicy(sizePolicy);

        gridLayout->addWidget(imageLabel, 0, 0, 1, 1);


        retranslateUi(PopupMessage);

        QMetaObject::connectSlotsByName(PopupMessage);
    } // setupUi

    void retranslateUi(QWidget *PopupMessage)
    {
        imageLabel->setText(QString());
        (void)PopupMessage;
    } // retranslateUi

};

namespace Ui {
    class PopupMessage: public Ui_PopupMessage {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_POPUPMESSAGE_H
