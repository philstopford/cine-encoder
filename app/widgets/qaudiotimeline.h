#ifndef QAUDIOTIMELINE_H
#define QAUDIOTIMELINE_H
#include <QWidget>
class QAudioTimeline : public QWidget {
    Q_OBJECT
public:
    explicit QAudioTimeline(QWidget *parent = nullptr);
    void setTrackCount(int count);
    void setOffset(int track, double milliseconds);
    void setTrim(int track, double startSeconds, double endSeconds);
    void setDuration(double seconds);
    void setWaveform(int track, const QVector<float> &samples);
    void setFitWidth(bool fit);
    void setZoom(double zoom);
signals:
    void offsetDragged(int track, double milliseconds);
    void trimDragged(int track, double startSeconds, double endSeconds);
protected:
    void paintEvent(QPaintEvent *) override;
    void mousePressEvent(QMouseEvent *) override;
    void mouseMoveEvent(QMouseEvent *) override;
    void mouseReleaseEvent(QMouseEvent *) override;
private:
    QVector<double> m_offsets;
    int m_dragTrack = -1;
    double m_dragStart = 0;
    QVector<double> m_starts, m_ends;
    QVector<QVector<float>> m_waveforms;
    double m_duration = 8.0;
    int m_dragEdge = 0;
    bool m_fitWidth = true;
    double m_zoom = 1.0;
};
#endif
