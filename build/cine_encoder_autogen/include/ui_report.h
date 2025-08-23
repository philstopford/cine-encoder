/********************************************************************************
** Form generated from reading UI file 'report.ui'
**
** Created by: Qt User Interface Compiler version 6.4.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_REPORT_H
#define UI_REPORT_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Report
{
public:
    QGridLayout *gridLayout;
    QPushButton *closeReport;
    QSpacerItem *horizontalSpacer;
    QLabel *reportTopLabel;
    QSpacerItem *horizontalSpacer_2;
    QSpacerItem *horizontalSpacer_3;
    QTableWidget *reportTable;

    void setupUi(QWidget *Report)
    {
        if (Report->objectName().isEmpty())
            Report->setObjectName("Report");
        Report->resize(400, 280);
        Report->setMinimumSize(QSize(400, 280));
        Report->setMaximumSize(QSize(1920, 1080));
        gridLayout = new QGridLayout(Report);
        gridLayout->setSpacing(6);
        gridLayout->setObjectName("gridLayout");
        gridLayout->setContentsMargins(9, 6, 9, 9);
        closeReport = new QPushButton(Report);
        closeReport->setObjectName("closeReport");
        closeReport->setMinimumSize(QSize(16, 16));
        closeReport->setMaximumSize(QSize(16, 16));

        gridLayout->addWidget(closeReport, 0, 4, 1, 1);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer, 0, 3, 1, 1);

        reportTopLabel = new QLabel(Report);
        reportTopLabel->setObjectName("reportTopLabel");

        gridLayout->addWidget(reportTopLabel, 0, 2, 1, 1);

        horizontalSpacer_2 = new QSpacerItem(16, 5, QSizePolicy::Policy::Fixed, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer_2, 0, 0, 1, 1);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer_3, 0, 1, 1, 1);

        reportTable = new QTableWidget(Report);
        if (reportTable->columnCount() < 2)
            reportTable->setColumnCount(2);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        reportTable->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        reportTable->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        reportTable->setObjectName("reportTable");
        reportTable->setEditTriggers(QAbstractItemView::EditTrigger::NoEditTriggers);
        reportTable->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
        reportTable->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
        reportTable->setShowGrid(false);
        reportTable->setRowCount(0);
        reportTable->setColumnCount(2);
        reportTable->horizontalHeader()->setVisible(true);
        reportTable->horizontalHeader()->setStretchLastSection(true);

        gridLayout->addWidget(reportTable, 1, 0, 1, 5);


        retranslateUi(Report);

        QMetaObject::connectSlotsByName(Report);
    } // setupUi

    void retranslateUi(QWidget *Report)
    {
        Report->setWindowTitle(QCoreApplication::translate("Report", "Report", nullptr));
        closeReport->setText(QString());
        reportTopLabel->setText(QCoreApplication::translate("Report", "Event Log", nullptr));
        QTableWidgetItem *___qtablewidgetitem = reportTable->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QCoreApplication::translate("Report", "Time", nullptr));
        QTableWidgetItem *___qtablewidgetitem1 = reportTable->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QCoreApplication::translate("Report", "Event", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Report: public Ui_Report {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_REPORT_H
