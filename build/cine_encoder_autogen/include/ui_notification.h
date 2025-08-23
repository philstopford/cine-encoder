/********************************************************************************
** Form generated from reading UI file 'notification.ui'
**
** Created by: Qt User Interface Compiler version 6.4.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_NOTIFICATION_H
#define UI_NOTIFICATION_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Notification
{
public:
    QGridLayout *gridLayout;
    QWidget *widget_main;
    QGridLayout *gridLayout_3;
    QFrame *frame_main;
    QGridLayout *gridLayout_2;
    QFrame *frame;
    QGridLayout *gridLayout_4;
    QPushButton *buttonBitcoin;
    QSpacerItem *spacerBitcoin;
    QSpacerItem *spacerPayPal;
    QSpacerItem *spacerClose;
    QPushButton *buttonPayPal;
    QPushButton *buttonCancel;
    QSpacerItem *horizontalSpacer_6;
    QTextBrowser *textBrowser;
    QFrame *frame_top;
    QGridLayout *gridLayout_7;
    QPushButton *closeWindow;
    QSpacerItem *horizontalSpacer_2;
    QSpacerItem *horizontalSpacer;
    QSpacerItem *verticalSpacer;
    QSpacerItem *horizontalSpacer_9;
    QLabel *label_title;
    QLabel *labelWindowIcon;

    void setupUi(QWidget *Notification)
    {
        if (Notification->objectName().isEmpty())
            Notification->setObjectName("Notification");
        Notification->resize(369, 363);
        Notification->setMinimumSize(QSize(369, 363));
        gridLayout = new QGridLayout(Notification);
        gridLayout->setSpacing(0);
        gridLayout->setObjectName("gridLayout");
        gridLayout->setContentsMargins(1, 1, 1, 1);
        widget_main = new QWidget(Notification);
        widget_main->setObjectName("widget_main");
        gridLayout_3 = new QGridLayout(widget_main);
        gridLayout_3->setSpacing(0);
        gridLayout_3->setObjectName("gridLayout_3");
        gridLayout_3->setContentsMargins(1, 1, 1, 1);
        frame_main = new QFrame(widget_main);
        frame_main->setObjectName("frame_main");
        frame_main->setMinimumSize(QSize(0, 0));
        frame_main->setMaximumSize(QSize(3840, 2160));
        frame_main->setFrameShape(QFrame::Shape::NoFrame);
        frame_main->setFrameShadow(QFrame::Shadow::Raised);
        gridLayout_2 = new QGridLayout(frame_main);
        gridLayout_2->setObjectName("gridLayout_2");
        gridLayout_2->setContentsMargins(0, 0, 0, 9);
        frame = new QFrame(frame_main);
        frame->setObjectName("frame");
        frame->setMinimumSize(QSize(0, 250));
        frame->setFrameShape(QFrame::Shape::NoFrame);
        frame->setFrameShadow(QFrame::Shadow::Raised);
        gridLayout_4 = new QGridLayout(frame);
        gridLayout_4->setObjectName("gridLayout_4");
        gridLayout_4->setHorizontalSpacing(0);
        gridLayout_4->setVerticalSpacing(9);
        gridLayout_4->setContentsMargins(12, 12, 12, 12);
        buttonBitcoin = new QPushButton(frame);
        buttonBitcoin->setObjectName("buttonBitcoin");
        buttonBitcoin->setEnabled(true);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(buttonBitcoin->sizePolicy().hasHeightForWidth());
        buttonBitcoin->setSizePolicy(sizePolicy);
        buttonBitcoin->setMinimumSize(QSize(90, 25));
        buttonBitcoin->setMaximumSize(QSize(90, 25));
        buttonBitcoin->setAutoDefault(false);
        buttonBitcoin->setFlat(true);

        gridLayout_4->addWidget(buttonBitcoin, 1, 3, 1, 1);

        spacerBitcoin = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_4->addItem(spacerBitcoin, 1, 2, 1, 1);

        spacerPayPal = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_4->addItem(spacerPayPal, 1, 0, 1, 1);

        spacerClose = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_4->addItem(spacerClose, 1, 4, 1, 1);

        buttonPayPal = new QPushButton(frame);
        buttonPayPal->setObjectName("buttonPayPal");
        buttonPayPal->setEnabled(true);
        sizePolicy.setHeightForWidth(buttonPayPal->sizePolicy().hasHeightForWidth());
        buttonPayPal->setSizePolicy(sizePolicy);
        buttonPayPal->setMinimumSize(QSize(90, 25));
        buttonPayPal->setMaximumSize(QSize(90, 25));
        buttonPayPal->setAutoDefault(false);
        buttonPayPal->setFlat(true);

        gridLayout_4->addWidget(buttonPayPal, 1, 1, 1, 1);

        buttonCancel = new QPushButton(frame);
        buttonCancel->setObjectName("buttonCancel");
        buttonCancel->setEnabled(true);
        sizePolicy.setHeightForWidth(buttonCancel->sizePolicy().hasHeightForWidth());
        buttonCancel->setSizePolicy(sizePolicy);
        buttonCancel->setMinimumSize(QSize(90, 25));
        buttonCancel->setMaximumSize(QSize(90, 25));
        buttonCancel->setAutoDefault(false);
        buttonCancel->setFlat(true);

        gridLayout_4->addWidget(buttonCancel, 1, 5, 1, 1);

        horizontalSpacer_6 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_4->addItem(horizontalSpacer_6, 1, 6, 1, 1);

        textBrowser = new QTextBrowser(frame);
        textBrowser->setObjectName("textBrowser");
        textBrowser->setHtml(QString::fromUtf8("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><meta charset=\"utf-8\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"hr { height: 1px; border-width: 0; }\n"
"li.unchecked::marker { content: \"\\2610\"; }\n"
"li.checked::marker { content: \"\\2612\"; }\n"
"</style></head><body style=\" font-family:'Noto Sans'; font-size:10pt; font-weight:400; font-style:normal;\">\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-family:'Cantarell';\"><br /></p></body></html>"));

        gridLayout_4->addWidget(textBrowser, 0, 0, 1, 7);


        gridLayout_2->addWidget(frame, 1, 0, 1, 3);

        frame_top = new QFrame(frame_main);
        frame_top->setObjectName("frame_top");
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(frame_top->sizePolicy().hasHeightForWidth());
        frame_top->setSizePolicy(sizePolicy1);
        frame_top->setMinimumSize(QSize(0, 31));
        frame_top->setMaximumSize(QSize(3840, 31));
        frame_top->setMouseTracking(false);
        frame_top->setAcceptDrops(false);
        frame_top->setFrameShape(QFrame::Shape::NoFrame);
        frame_top->setFrameShadow(QFrame::Shadow::Raised);
        gridLayout_7 = new QGridLayout(frame_top);
        gridLayout_7->setSpacing(0);
        gridLayout_7->setObjectName("gridLayout_7");
        gridLayout_7->setSizeConstraint(QLayout::SizeConstraint::SetDefaultConstraint);
        gridLayout_7->setContentsMargins(0, 0, 0, 0);
        closeWindow = new QPushButton(frame_top);
        closeWindow->setObjectName("closeWindow");
        closeWindow->setEnabled(true);
        QSizePolicy sizePolicy2(QSizePolicy::Minimum, QSizePolicy::Expanding);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(closeWindow->sizePolicy().hasHeightForWidth());
        closeWindow->setSizePolicy(sizePolicy2);
        closeWindow->setMinimumSize(QSize(40, 31));
        closeWindow->setMaximumSize(QSize(40, 31));
        closeWindow->setIconSize(QSize(16, 16));

        gridLayout_7->addWidget(closeWindow, 0, 6, 1, 1);

        horizontalSpacer_2 = new QSpacerItem(12, 20, QSizePolicy::Policy::Fixed, QSizePolicy::Minimum);

        gridLayout_7->addItem(horizontalSpacer_2, 0, 0, 1, 1);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_7->addItem(horizontalSpacer, 0, 2, 1, 1);

        verticalSpacer = new QSpacerItem(20, 10, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_7->addItem(verticalSpacer, 1, 6, 1, 1);

        horizontalSpacer_9 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_7->addItem(horizontalSpacer_9, 0, 4, 1, 1);

        label_title = new QLabel(frame_top);
        label_title->setObjectName("label_title");

        gridLayout_7->addWidget(label_title, 0, 3, 1, 1);

        labelWindowIcon = new QLabel(frame_top);
        labelWindowIcon->setObjectName("labelWindowIcon");
        labelWindowIcon->setMinimumSize(QSize(20, 20));
        labelWindowIcon->setMaximumSize(QSize(20, 20));
        labelWindowIcon->setPixmap(QPixmap(QString::fromUtf8(":/resources/icons/64x64/cine-encoder.png")));
        labelWindowIcon->setScaledContents(true);

        gridLayout_7->addWidget(labelWindowIcon, 0, 1, 1, 1);


        gridLayout_2->addWidget(frame_top, 0, 0, 1, 3);


        gridLayout_3->addWidget(frame_main, 0, 0, 1, 1);


        gridLayout->addWidget(widget_main, 0, 0, 1, 1);


        retranslateUi(Notification);

        QMetaObject::connectSlotsByName(Notification);
    } // setupUi

    void retranslateUi(QWidget *Notification)
    {
        Notification->setWindowTitle(QCoreApplication::translate("Notification", "NOTIFICATION", nullptr));
        buttonBitcoin->setText(QCoreApplication::translate("Notification", "Bitcoin", nullptr));
        buttonPayPal->setText(QCoreApplication::translate("Notification", "PayPal", nullptr));
        buttonCancel->setText(QCoreApplication::translate("Notification", "CLOSE", nullptr));
        closeWindow->setText(QString());
        label_title->setText(QCoreApplication::translate("Notification", "NOTIFICATION", nullptr));
        labelWindowIcon->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class Notification: public Ui_Notification {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_NOTIFICATION_H
