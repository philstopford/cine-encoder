/********************************************************************************
** Form generated from reading UI file 'progress.ui'
**
** Created by: Qt User Interface Compiler version 6.4.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PROGRESS_H
#define UI_PROGRESS_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Progress
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
    QSpacerItem *horizontalSpacer;
    QSpacerItem *horizontalSpacer_9;
    QFrame *frame;
    QGridLayout *gridLayout_4;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QLabel *label_filename;
    QProgressBar *progressBar_opening;

    void setupUi(QWidget *Progress)
    {
        if (Progress->objectName().isEmpty())
            Progress->setObjectName("Progress");
        Progress->resize(302, 112);
        Progress->setMinimumSize(QSize(302, 112));
        gridLayout = new QGridLayout(Progress);
        gridLayout->setSpacing(0);
        gridLayout->setObjectName("gridLayout");
        gridLayout->setContentsMargins(1, 1, 1, 1);
        widget_main = new QWidget(Progress);
        widget_main->setObjectName("widget_main");
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(widget_main->sizePolicy().hasHeightForWidth());
        widget_main->setSizePolicy(sizePolicy);
        gridLayout_3 = new QGridLayout(widget_main);
        gridLayout_3->setSpacing(0);
        gridLayout_3->setObjectName("gridLayout_3");
        gridLayout_3->setContentsMargins(1, 1, 1, 1);
        frame_main = new QFrame(widget_main);
        frame_main->setObjectName("frame_main");
        sizePolicy.setHeightForWidth(frame_main->sizePolicy().hasHeightForWidth());
        frame_main->setSizePolicy(sizePolicy);
        frame_main->setMinimumSize(QSize(0, 0));
        frame_main->setFrameShape(QFrame::Shape::NoFrame);
        frame_main->setFrameShadow(QFrame::Shadow::Raised);
        gridLayout_2 = new QGridLayout(frame_main);
        gridLayout_2->setSpacing(0);
        gridLayout_2->setObjectName("gridLayout_2");
        gridLayout_2->setContentsMargins(0, 0, 0, 12);
        frame_top = new QFrame(frame_main);
        frame_top->setObjectName("frame_top");
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(frame_top->sizePolicy().hasHeightForWidth());
        frame_top->setSizePolicy(sizePolicy1);
        frame_top->setMinimumSize(QSize(0, 31));
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

        gridLayout_7->addWidget(label_title, 0, 1, 1, 1);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_7->addItem(horizontalSpacer, 0, 0, 1, 1);

        horizontalSpacer_9 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_7->addItem(horizontalSpacer_9, 0, 2, 1, 1);


        gridLayout_2->addWidget(frame_top, 0, 0, 1, 1);

        frame = new QFrame(frame_main);
        frame->setObjectName("frame");
        sizePolicy.setHeightForWidth(frame->sizePolicy().hasHeightForWidth());
        frame->setSizePolicy(sizePolicy);
        frame->setMinimumSize(QSize(0, 0));
        frame->setFrameShape(QFrame::Shape::NoFrame);
        frame->setFrameShadow(QFrame::Shadow::Raised);
        gridLayout_4 = new QGridLayout(frame);
        gridLayout_4->setObjectName("gridLayout_4");
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        label = new QLabel(frame);
        label->setObjectName("label");
        QSizePolicy sizePolicy2(QSizePolicy::Minimum, QSizePolicy::Preferred);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
        label->setSizePolicy(sizePolicy2);

        horizontalLayout->addWidget(label);

        label_filename = new QLabel(frame);
        label_filename->setObjectName("label_filename");
        sizePolicy1.setHeightForWidth(label_filename->sizePolicy().hasHeightForWidth());
        label_filename->setSizePolicy(sizePolicy1);

        horizontalLayout->addWidget(label_filename);


        gridLayout_4->addLayout(horizontalLayout, 0, 0, 1, 1);

        progressBar_opening = new QProgressBar(frame);
        progressBar_opening->setObjectName("progressBar_opening");
        progressBar_opening->setValue(0);
        progressBar_opening->setAlignment(Qt::AlignmentFlag::AlignCenter);

        gridLayout_4->addWidget(progressBar_opening, 1, 0, 1, 1);


        gridLayout_2->addWidget(frame, 1, 0, 2, 1);


        gridLayout_3->addWidget(frame_main, 0, 0, 1, 1);


        gridLayout->addWidget(widget_main, 0, 0, 1, 1);


        retranslateUi(Progress);

        QMetaObject::connectSlotsByName(Progress);
    } // setupUi

    void retranslateUi(QWidget *Progress)
    {
        Progress->setWindowTitle(QCoreApplication::translate("Progress", "PROGRESS", nullptr));
        label_title->setText(QCoreApplication::translate("Progress", "PROGRESS", nullptr));
        label->setText(QCoreApplication::translate("Progress", "Filename:", nullptr));
        label_filename->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class Progress: public Ui_Progress {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PROGRESS_H
