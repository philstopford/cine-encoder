#ifndef QABVIDEOWIDGET_H
#define QABVIDEOWIDGET_H

#include <QImage>
#include <QWidget>

class QABVideoWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QABVideoWidget(QWidget *parent = nullptr);
    void setOriginalFrame(const QImage &frame, qint64 timestampMs = -1);
    void setProcessedFrame(const QImage &frame, qint64 timestampMs = -1);
    void setDivider(int percent);
    int zoomPercent() const { return m_zoomPercent; }
    void resetZoom();

signals:
    void dividerMoved(int percent);
    void zoomChanged(int percent);

protected:
    void paintEvent(QPaintEvent *) override;
    void mousePressEvent(QMouseEvent *) override;
    void mouseMoveEvent(QMouseEvent *) override;
    void mouseReleaseEvent(QMouseEvent *) override;
    void wheelEvent(QWheelEvent *) override;

private:
    QImage m_original;
    QImage m_processed;
    QImage m_pendingOriginal;
    QImage m_pendingProcessed;
    qint64 m_originalTimestamp = -1;
    qint64 m_processedTimestamp = -1;
    int m_divider = 50;
    int m_zoomPercent = 100;
    bool m_draggingDivider = false;
    bool m_panning = false;
    QPoint m_pan;
    QPoint m_panStart;
    QPoint m_dragStart;
};

#endif
