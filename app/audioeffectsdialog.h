#ifndef AUDIOEFFECTSDIALOG_H
#define AUDIOEFFECTSDIALOG_H
#include <QDialog>
#include "constants.h"
class QComboBox; class QCheckBox; class QDoubleSpinBox; class QLineEdit;
class QProcess; class QMediaPlayer; class QAudioOutput;
class QVideoWidget; class QAudioTimeline; class QTimer; class QPushButton; class QSlider; class QDoubleSpinBox; class QLabel;
class AudioEffectsDialog : public QDialog {
    Q_OBJECT
public:
    AudioEffectsDialog(Constants::Data &data, const QString &source, QWidget *parent = nullptr);
private slots:
    void trackChanged(int index);
    void acceptChanges();
    void preview();
private:
    Constants::Data &m_data;
    QString m_source;
    QComboBox *m_track;
    QCheckBox *m_enabled;
    QDoubleSpinBox *m_delay, *m_tempo, *m_start, *m_end;
    QLineEdit *m_filters;
    QProcess *m_previewProcess = nullptr;
    QMediaPlayer *m_player = nullptr;
    QAudioOutput *m_audioOutput = nullptr;
    QAudioOutput *m_videoAudioOutput = nullptr;
    QMediaPlayer *m_videoPlayer = nullptr;
    QVideoWidget *m_video = nullptr;
    QAudioTimeline *m_timeline = nullptr;
    QTimer *m_renderDebounce = nullptr;
    QProcess *m_waveformProcess = nullptr;
    int m_previewSequence = 0;
    QPushButton *m_loop = nullptr;
    QSlider *m_videoPosition = nullptr;
    QDoubleSpinBox *m_loopStart = nullptr;
    QDoubleSpinBox *m_loopEnd = nullptr;
    QSlider *m_zoom = nullptr;
    QLabel *m_timeLabel = nullptr;
    int m_current = -1;
    void load(int index);
    void save();
    void loadWaveform(int track);
};
#endif
