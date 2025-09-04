#include "report.h"
#include "ui_report.h"
#include "helper.h"
#include <QMouseEvent>
#include <QLayout>
#include <QToolTip>
#include <QTimer>
#include <QHeaderView>
#include <iostream>

#define OFFSET (QPoint(530, -30) * Helper::scaling())


Report::Report(QWidget *parent, const QVector<ReportLog> &reportLog) :
    BaseDialog(parent, false),
    ui(new Ui::Report),
    m_activated(false)
{
    setWindowFlags((windowFlags() & ~Qt::Dialog & ~Qt::SubWindow) | Qt::Popup);
    setWindowModality(Qt::NonModal);
    setAttribute(Qt::WA_DeleteOnClose);
    auto *ui_widget = new QWidget(this);
    layout()->addWidget(ui_widget);
    ui->setupUi(ui_widget);
    setMaskWidget(ui_widget);
    ui_widget->setAutoFillBackground(true);
    ui_widget->setObjectName("frame_main");
    ui_widget->setProperty("scale", int(Helper::scaling() * 100));

    setEnabled(true);
    connect(ui->closeReport, &QPushButton::clicked, this, [this]() {
        close();
    });

    auto *tmr = new QTimer(this);
    tmr->setSingleShot(false);
    tmr->setInterval(250);
    connect(tmr, &QTimer::timeout, this, &Report::moveWidget);
    tmr->start();

    QFont fnt = ui->reportTable->horizontalHeader()->font();
    fnt.setBold(true);
    fnt.setItalic(true);
    ui->reportTable->setEnabled(true);
    ui->reportTable->horizontalHeader()->setFont(fnt);
    ui->reportTable->horizontalHeader()->setFixedHeight(28 * Helper::scaling());
    ui->reportTable->horizontalHeader()->setVisible(true);
    ui->reportTable->verticalHeader()->setVisible(true);
    ui->reportTable->setShowGrid(false);
    ui->reportTable->setColumnWidth(0, 80 * Helper::scaling());
    
    // Enable text wrapping and improve row sizing
    ui->reportTable->setWordWrap(true);
    ui->reportTable->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->reportTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    
    foreach (const ReportLog &a, reportLog) {
        const int ind = ui->reportTable->rowCount();
        ui->reportTable->setRowCount(ind + 1);

        auto *__item_time = new QTableWidgetItem(a.reportTime);
        __item_time->setTextAlignment(Qt::AlignCenter);
        ui->reportTable->setItem(ind, 0, __item_time);

        // Keep the full message for better readability
        QString msg = a.reportMsg;
        auto *__item_msg = new QTableWidgetItem(msg);
        __item_msg->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        __item_msg->setData(Qt::DisplayRole, msg);
        __item_msg->setToolTip(msg); // Keep tooltip for very long messages
        ui->reportTable->setItem(ind, 1, __item_msg);

        QString iconPath(":/resources/icons/svg/info.svg");
        switch (a.reportIcon) {
        case ReportLog::Icon::Warning:
            iconPath = QString(":/resources/icons/svg/warning.svg");
            break;
        case ReportLog::Icon::Critical:
            iconPath = QString(":/resources/icons/svg/error.svg");
            break;
        default:
            break;
        }
        QPixmap pxmp(iconPath);
        QPixmap scaled = pxmp.scaled(QSize(30,30) * Helper::scaling(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        ui->reportTable->item(ind, 1)->setIcon(QIcon(scaled));
    }
    const int ind = ui->reportTable->rowCount();
    if (ind > 0)
        ui->reportTable->selectRow(ind - 1);
    ui->reportTable->setMouseTracking(true);
    connect(ui->reportTable, &QTableWidget::itemEntered, this, &Report::onItemEntered);
}

Report::~Report()
{
    delete ui;
}

void Report::showEvent(QShowEvent *event)
{
    BaseDialog::showEvent(event);
    if (!m_activated) {
        m_activated = true;
        
        // Calculate optimal size based on content
        const int maxWidth = 800 * Helper::scaling();
        const int minWidth = 400 * Helper::scaling();
        const int maxHeight = 600 * Helper::scaling();
        const int minHeight = 280 * Helper::scaling();
        
        // Calculate required height based on number of rows and content
        int rowCount = ui->reportTable->rowCount();
        if (rowCount > 0) {
            ui->reportTable->resizeRowsToContents();
            int totalRowHeight = 0;
            for (int i = 0; i < rowCount; ++i) {
                totalRowHeight += ui->reportTable->rowHeight(i);
            }
            int headerHeight = ui->reportTable->horizontalHeader()->height();
            int margins = 60 * Helper::scaling(); // Margins and other UI elements
            
            int optimalHeight = qBound(minHeight, totalRowHeight + headerHeight + margins, maxHeight);
            resize(minWidth, optimalHeight);
        } else {
            resize(minWidth, minHeight);
        }
        
        moveWidget();
    }
}

void Report::onItemEntered(QTableWidgetItem *item)
{
    if (item) {
        if (item->column() == 1)
            item->setToolTip(item->text());
    }
}

void Report::moveWidget()
{
    if (parentWidget()) {
        auto cw = parentWidget()->findChild<QWidget*>("centralwidget");
        if (cw)
            move(cw->mapToGlobal(cw->geometry().topRight()) - OFFSET);
    }
}
