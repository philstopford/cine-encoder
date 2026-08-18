#include "qabvideowidget.h"

#include <QPainter>
#include <QMouseEvent>
#include <QWheelEvent>

QABVideoWidget::QABVideoWidget(QWidget *parent) : QWidget(parent)
{
    setMinimumSize(480, 270);
    setAutoFillBackground(true);
}

void QABVideoWidget::setOriginalFrame(const QImage &frame, qint64 timestampMs)
{
    m_pendingOriginal = frame; m_originalTimestamp = timestampMs;
    if (m_pendingProcessed.isNull() || timestampMs < 0 || m_processedTimestamp < 0 ||
        qAbs(timestampMs - m_processedTimestamp) <= 20) {
        m_original = m_pendingOriginal;
        if (!m_pendingProcessed.isNull()) m_processed = m_pendingProcessed;
        update();
    }
}
void QABVideoWidget::setProcessedFrame(const QImage &frame, qint64 timestampMs)
{
    m_pendingProcessed = frame; m_processedTimestamp = timestampMs;
    if (m_pendingOriginal.isNull() || timestampMs < 0 || m_originalTimestamp < 0 ||
        qAbs(timestampMs - m_originalTimestamp) <= 20) {
        m_processed = m_pendingProcessed;
        if (!m_pendingOriginal.isNull()) m_original = m_pendingOriginal;
        update();
    }
}
void QABVideoWidget::setDivider(int percent) { m_divider = qBound(0, percent, 100); update(); }
void QABVideoWidget::resetZoom() { m_zoomPercent = 100; m_pan = QPoint(); emit zoomChanged(m_zoomPercent); update(); }

void QABVideoWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::MiddleButton) {
        m_panning = true;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        m_dragStart = event->position().toPoint();
#else
        m_dragStart = event->pos();
#endif
        m_panStart = m_pan; setCursor(Qt::ClosedHandCursor); event->accept(); return;
    }
    if (event->button() != Qt::LeftButton) return;
    const QImage &reference = !m_original.isNull() ? m_original : m_processed;
    if (reference.isNull()) return;
    const QSize base = reference.size().scaled(size(), Qt::KeepAspectRatio);
    const int imageWidth = base.width() * m_zoomPercent / 100;
    const int left = (width() - imageWidth) / 2 + m_pan.x();
    const int divider = left + imageWidth * m_divider / 100;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    const int mouseX = qRound(event->position().x());
#else
    const int mouseX = event->pos().x();
#endif
    if (qAbs(mouseX - divider) <= 18) {
        m_draggingDivider = true; setCursor(Qt::SplitHCursor); event->accept();
    }
}

void QABVideoWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (m_panning) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        const QPoint current = event->position().toPoint();
#else
        const QPoint current = event->pos();
#endif
        m_pan = m_panStart + current - m_dragStart; update(); event->accept(); return;
    }
    if (!m_draggingDivider) return;
    const QImage &reference = !m_original.isNull() ? m_original : m_processed;
    if (reference.isNull()) return;
    const int imageWidth = reference.size().scaled(size(), Qt::KeepAspectRatio).width() * m_zoomPercent / 100;
    const int left = (width() - imageWidth) / 2 + m_pan.x();
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    const int mouseX = qRound(event->position().x());
#else
    const int mouseX = event->pos().x();
#endif
    setDivider(imageWidth > 0 ? (mouseX - left) * 100 / imageWidth : 50);
    emit dividerMoved(m_divider); event->accept();
}

void QABVideoWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::MiddleButton) { m_panning = false; unsetCursor(); event->accept(); return; }
    if (event->button() == Qt::LeftButton) { m_draggingDivider = false; unsetCursor(); event->accept(); }
}

void QABVideoWidget::wheelEvent(QWheelEvent *event)
{
    const int delta = event->angleDelta().y();
    if (!delta) return;
    m_zoomPercent = qBound(25, m_zoomPercent + (delta > 0 ? 10 : -10), 400);
    emit zoomChanged(m_zoomPercent); update(); event->accept();
}

void QABVideoWidget::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.fillRect(rect(), Qt::black);
    const QImage &reference = !m_original.isNull() ? m_original : m_processed;
    if (reference.isNull()) return;
    QSize target = reference.size().scaled(size(), Qt::KeepAspectRatio);
    target = target * m_zoomPercent / 100;
    const QRect area((width() - target.width()) / 2 + m_pan.x(), (height() - target.height()) / 2 + m_pan.y(),
                     target.width(), target.height());
    p.drawImage(area, m_original.isNull() ? m_processed : m_original);
    if (!m_processed.isNull()) {
        p.save();
        const int split = area.left() + area.width() * m_divider / 100;
        p.setClipRect(QRect(split, area.top(), area.right() - split + 1, area.height()));
        p.drawImage(area, m_processed);
        p.restore();
        p.setPen(QPen(Qt::white, 2));
        p.drawLine(split, area.top(), split, area.bottom());
    }
    auto drawLabel = [&p](const QString &text, int x) {
        const QFontMetrics metrics(p.font());
        const QRect box(x, 12, metrics.horizontalAdvance(text) + 18, metrics.height() + 8);
        p.fillRect(box, QColor(0, 0, 0, 170));
        p.setPen(Qt::white);
        p.drawText(box, Qt::AlignCenter, text);
    };
    drawLabel(tr("ORIGINAL"), area.left() + 12);
    const QString processedLabel = tr("PROCESSED");
    drawLabel(processedLabel, area.right() - p.fontMetrics().horizontalAdvance(processedLabel) - 30);
}
