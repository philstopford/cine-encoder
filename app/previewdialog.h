#ifndef PREVIEWDIALOG_H
#define PREVIEWDIALOG_H

#include <QDialog>
#include <QProcess>
#include <QStringList>
#include <QtGlobal>

class QLabel;
class QMediaPlayer;
class QPushButton;
class QSlider;
class QDoubleSpinBox;
class QSpinBox;
class QCheckBox;
class QEffectStack;
class QTemporaryDir;
class QABVideoWidget;
class QWidget;
class QImage;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
class QAudioOutput;
#endif

struct PreviewRequest
{
    QString inputFile;
    QStringList videoFilters;
    QStringList ffmpegInputArguments;
    QStringList ffmpegOutputArguments;
    double startSeconds = 0.0;
    double sourceDurationSeconds = 0.0;
    int durationSeconds = 8;
    QString title;
};

// General-purpose preview surface. Callers supply an FFmpeg request, making
// this usable for effects, subtitle burn-in, scaling and future settings.
class PreviewDialog : public QDialog
{
    Q_OBJECT
public:
    explicit PreviewDialog(const PreviewRequest &request, QWidget *parent = nullptr);
    ~PreviewDialog() override;
    QStringList filters() const;

private slots:
    void render();
    void renderFinished(int exitCode, QProcess::ExitStatus status);
    void togglePlayback();
    void seek(int value);
    void updateWipe(int value);
    void stepFrame(int direction);
    void updatePosition(qint64 position);
    void originalStatusChanged(int status);
    void processedStatusChanged(int status);
    void tryEnablePlayback();
    void seekPlayers(qint64 position);
    void setOriginalFrame(const QImage &frame, qint64 timestampMs);
    void setProcessedFrame(const QImage &frame, qint64 timestampMs);

private:
    PreviewRequest m_request;
    QTemporaryDir *m_tempDir;
    QProcess *m_renderer;
    QMediaPlayer *m_originalPlayer;
    QMediaPlayer *m_processedPlayer;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QAudioOutput *m_audioOutput;
#endif
    QABVideoWidget *m_video;
    QSlider *m_wipe;
    QPushButton *m_previousFrame;
    QPushButton *m_nextFrame;
    QLabel *m_status;
    QPushButton *m_play;
    QSlider *m_position;
    QDoubleSpinBox *m_start;
    QSpinBox *m_duration;
    QPushButton *m_render;
    QCheckBox *m_applyDenoise;
    QEffectStack *m_effectStack;
    QStringList m_workingFilters;
    QString m_outputFile;
    int m_renderSequence = 0;
    bool m_sourcesReady = false;
    bool m_syncing = false;
    qint64 m_originalPtsOffset = -1;
    qint64 m_processedPtsOffset = -1;
};

#endif
