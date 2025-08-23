/********************************************************************************
** Form generated from reading UI file 'message.ui'
**
** Created by: Qt User Interface Compiler version 6.4.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MESSAGE_H
#define UI_MESSAGE_H

#include <QtCore/QLocale>
#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Message
{
public:
    QGridLayout *gridLayout;
    QWidget *widget_main;
    QGridLayout *gridLayout_3;
    QFrame *frame_main;
    QGridLayout *gridLayout_2;
    QFrame *frame_top;
    QGridLayout *gridLayout_7;
    QLabel *label_title;
    QSpacerItem *verticalSpacer;
    QLabel *labelWindowIcon;
    QSpacerItem *horizontalSpacer;
    QPushButton *closeWindow;
    QSpacerItem *horizontalSpacer_9;
    QSpacerItem *horizontalSpacer_2;
    QSpacerItem *horizontalSpacer_4;
    QFrame *frame;
    QGridLayout *gridLayout_4;
    QPushButton *buttonCancel;
    QSpacerItem *horizontalSpacer_3;
    QPushButton *buttonApply;
    QSpacerItem *spacerCancel;
    QSpacerItem *spacerApply;
    QHBoxLayout *horizontalLayout;
    QTextBrowser *textBrowser;

    void setupUi(QWidget *Message)
    {
        if (Message->objectName().isEmpty())
            Message->setObjectName("Message");
        Message->resize(300, 165);
        gridLayout = new QGridLayout(Message);
        gridLayout->setSpacing(0);
        gridLayout->setObjectName("gridLayout");
        gridLayout->setContentsMargins(1, 1, 1, 1);
        widget_main = new QWidget(Message);
        widget_main->setObjectName("widget_main");
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(widget_main->sizePolicy().hasHeightForWidth());
        widget_main->setSizePolicy(sizePolicy);
        widget_main->setMinimumSize(QSize(0, 20));
        gridLayout_3 = new QGridLayout(widget_main);
        gridLayout_3->setSpacing(0);
        gridLayout_3->setObjectName("gridLayout_3");
        gridLayout_3->setContentsMargins(1, 1, 1, 1);
        frame_main = new QFrame(widget_main);
        frame_main->setObjectName("frame_main");
        sizePolicy.setHeightForWidth(frame_main->sizePolicy().hasHeightForWidth());
        frame_main->setSizePolicy(sizePolicy);
        frame_main->setMinimumSize(QSize(0, 20));
        frame_main->setFrameShape(QFrame::Shape::NoFrame);
        frame_main->setFrameShadow(QFrame::Shadow::Raised);
        gridLayout_2 = new QGridLayout(frame_main);
        gridLayout_2->setSpacing(0);
        gridLayout_2->setObjectName("gridLayout_2");
        gridLayout_2->setContentsMargins(0, 0, 0, 0);
        frame_top = new QFrame(frame_main);
        frame_top->setObjectName("frame_top");
        sizePolicy.setHeightForWidth(frame_top->sizePolicy().hasHeightForWidth());
        frame_top->setSizePolicy(sizePolicy);
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
        label_title = new QLabel(frame_top);
        label_title->setObjectName("label_title");

        gridLayout_7->addWidget(label_title, 0, 4, 1, 1);

        verticalSpacer = new QSpacerItem(20, 10, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_7->addItem(verticalSpacer, 1, 7, 1, 1);

        labelWindowIcon = new QLabel(frame_top);
        labelWindowIcon->setObjectName("labelWindowIcon");
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(labelWindowIcon->sizePolicy().hasHeightForWidth());
        labelWindowIcon->setSizePolicy(sizePolicy1);
        labelWindowIcon->setMinimumSize(QSize(20, 20));
        labelWindowIcon->setMaximumSize(QSize(20, 20));
        labelWindowIcon->setPixmap(QPixmap(QString::fromUtf8(":/resources/icons/64x64/cine-encoder.png")));
        labelWindowIcon->setScaledContents(true);

        gridLayout_7->addWidget(labelWindowIcon, 0, 1, 1, 1);

        horizontalSpacer = new QSpacerItem(5, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_7->addItem(horizontalSpacer, 0, 3, 1, 1);

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

        gridLayout_7->addWidget(closeWindow, 0, 7, 1, 1);

        horizontalSpacer_9 = new QSpacerItem(5, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Minimum);

        gridLayout_7->addItem(horizontalSpacer_9, 0, 5, 1, 1);

        horizontalSpacer_2 = new QSpacerItem(12, 20, QSizePolicy::Policy::Fixed, QSizePolicy::Minimum);

        gridLayout_7->addItem(horizontalSpacer_2, 0, 0, 1, 1);

        horizontalSpacer_4 = new QSpacerItem(8, 5, QSizePolicy::Policy::Fixed, QSizePolicy::Minimum);

        gridLayout_7->addItem(horizontalSpacer_4, 0, 2, 1, 1);


        gridLayout_2->addWidget(frame_top, 0, 0, 1, 1);

        frame = new QFrame(frame_main);
        frame->setObjectName("frame");
        sizePolicy.setHeightForWidth(frame->sizePolicy().hasHeightForWidth());
        frame->setSizePolicy(sizePolicy);
        frame->setMinimumSize(QSize(0, 20));
        frame->setFrameShape(QFrame::Shape::NoFrame);
        frame->setFrameShadow(QFrame::Shadow::Raised);
        gridLayout_4 = new QGridLayout(frame);
        gridLayout_4->setObjectName("gridLayout_4");
        gridLayout_4->setHorizontalSpacing(0);
        gridLayout_4->setVerticalSpacing(12);
        gridLayout_4->setContentsMargins(9, 12, 9, 18);
        buttonCancel = new QPushButton(frame);
        buttonCancel->setObjectName("buttonCancel");
        buttonCancel->setEnabled(true);
        QSizePolicy sizePolicy3(QSizePolicy::Maximum, QSizePolicy::Maximum);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(buttonCancel->sizePolicy().hasHeightForWidth());
        buttonCancel->setSizePolicy(sizePolicy3);
        buttonCancel->setMinimumSize(QSize(90, 25));
        buttonCancel->setMaximumSize(QSize(90, 25));
        buttonCancel->setBaseSize(QSize(90, 25));
        buttonCancel->setAutoDefault(false);
        buttonCancel->setFlat(false);

        gridLayout_4->addWidget(buttonCancel, 3, 5, 1, 1);

        horizontalSpacer_3 = new QSpacerItem(20, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Minimum);

        gridLayout_4->addItem(horizontalSpacer_3, 3, 4, 1, 1);

        buttonApply = new QPushButton(frame);
        buttonApply->setObjectName("buttonApply");
        buttonApply->setEnabled(true);
        sizePolicy3.setHeightForWidth(buttonApply->sizePolicy().hasHeightForWidth());
        buttonApply->setSizePolicy(sizePolicy3);
        buttonApply->setMinimumSize(QSize(90, 25));
        buttonApply->setMaximumSize(QSize(90, 25));
        buttonApply->setBaseSize(QSize(90, 25));
        buttonApply->setAutoDefault(false);
        buttonApply->setFlat(false);

        gridLayout_4->addWidget(buttonApply, 3, 3, 1, 1);

        spacerCancel = new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_4->addItem(spacerCancel, 3, 6, 1, 1);

        spacerApply = new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_4->addItem(spacerApply, 3, 0, 1, 1);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(18);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(0, -1, 0, -1);
        textBrowser = new QTextBrowser(frame);
        textBrowser->setObjectName("textBrowser");
        textBrowser->setEnabled(true);
        textBrowser->setMouseTracking(true);
        textBrowser->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
        textBrowser->setAcceptDrops(true);
        textBrowser->setAutoFillBackground(true);
        textBrowser->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        textBrowser->setFrameShape(QFrame::Shape::NoFrame);
        textBrowser->setFrameShadow(QFrame::Shadow::Sunken);
        textBrowser->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);
        textBrowser->setOpenLinks(true);

        horizontalLayout->addWidget(textBrowser);


        gridLayout_4->addLayout(horizontalLayout, 2, 0, 1, 7);


        gridLayout_2->addWidget(frame, 1, 0, 2, 1);


        gridLayout_3->addWidget(frame_main, 0, 0, 1, 1);


        gridLayout->addWidget(widget_main, 0, 0, 1, 1);


        retranslateUi(Message);

        QMetaObject::connectSlotsByName(Message);
    } // setupUi

    void retranslateUi(QWidget *Message)
    {
        Message->setWindowTitle(QCoreApplication::translate("Message", "Cine Encoder", nullptr));
        label_title->setText(QCoreApplication::translate("Message", "CINE  ENCODER", nullptr));
        labelWindowIcon->setText(QString());
        closeWindow->setText(QString());
        buttonCancel->setText(QCoreApplication::translate("Message", "CANCEL", nullptr));
        buttonApply->setText(QCoreApplication::translate("Message", "OK", nullptr));
        textBrowser->setHtml(QCoreApplication::translate("Message", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><meta charset=\"utf-8\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"hr { height: 1px; border-width: 0; }\n"
"li.unchecked::marker { content: \"\\2610\"; }\n"
"li.checked::marker { content: \"\\2612\"; }\n"
"</style></head><body style=\" font-family:'Noto Sans'; font-size:10pt; font-weight:400; font-style:normal;\">\n"
"<p align=\"center\" style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><br /></p></body></html>", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Message: public Ui_Message {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MESSAGE_H
