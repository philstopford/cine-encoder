#include "audioeffectsdialog.h"
#include "widgets/qaudiotimeline.h"
#include <QComboBox>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QLabel>
#include <QPushButton>
#include <QProcess>
#include <QTemporaryFile>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QMessageBox>
#include <QDir>
#include <QUrl>
#include <QVideoWidget>
#include <QTimer>
#include <QHBoxLayout>
#include <QtEndian>
#include <cmath>
#include <cstring>
#include <QSlider>
#include <QSignalBlocker>
#include <QScrollArea>

AudioEffectsDialog::AudioEffectsDialog(Constants::Data &data, const QString &source, QWidget *parent) : QDialog(parent), m_data(data), m_source(source) {
    setWindowTitle(tr("Audio timing and effects")); resize(900, 700);
    auto *root = new QVBoxLayout(this);
    m_video = new QVideoWidget(this); m_video->setMinimumHeight(260); root->addWidget(m_video, 2);
    m_videoPlayer = new QMediaPlayer(this); m_videoPlayer->setVideoOutput(m_video); m_videoAudioOutput = new QAudioOutput(this); m_videoAudioOutput->setVolume(0.0); m_videoPlayer->setAudioOutput(m_videoAudioOutput); m_videoPlayer->setSource(QUrl::fromLocalFile(m_source));
    auto *transport = new QHBoxLayout; auto *play = new QPushButton(tr("Play"), this); m_loop = new QPushButton(tr("Loop preview"), this); m_loop->setCheckable(true); transport->addWidget(play); transport->addWidget(m_loop);
    m_loopStart=new QDoubleSpinBox(this); m_loopEnd=new QDoubleSpinBox(this); for(auto *s:{m_loopStart,m_loopEnd}){s->setRange(0,999999);s->setDecimals(3);s->setSuffix(" s");} m_loopEnd->setValue(8); transport->addWidget(new QLabel(tr("Loop:"),this)); transport->addWidget(m_loopStart); transport->addWidget(new QLabel("–",this)); transport->addWidget(m_loopEnd); transport->addStretch(); root->addLayout(transport);
    m_videoPosition=new QSlider(Qt::Horizontal,this); m_videoPosition->setRange(0,0); root->addWidget(m_videoPosition);
    m_timeLabel=new QLabel("0.000 s",this); root->addWidget(m_timeLabel);
    auto *layout = new QFormLayout; root->addLayout(layout);
    m_track = new QComboBox(this);
    for (int i = 0; i < data.fields[Constants::Data::audioFormats].size(); ++i)
        m_track->addItem(tr("Internal %1: %2").arg(i + 1).arg(data.fields[Constants::Data::audioTitles][i]), i);
    for (int i = 0; i < data.fields[Constants::Data::externAudioFormats].size(); ++i)
        m_track->addItem(tr("External %1: %2").arg(i + 1).arg(data.fields[Constants::Data::externAudioTitles][i]), 1000 + i);
    m_timeline = new QAudioTimeline(this); m_timeline->setTrackCount(data.fields[Constants::Data::audioFormats].size()); m_timeline->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    auto *timelineScroll=new QScrollArea(this); timelineScroll->setWidgetResizable(true); timelineScroll->setWidget(m_timeline); timelineScroll->setMinimumHeight(180); root->addWidget(timelineScroll,3);
    auto *viewRow=new QHBoxLayout; viewRow->addWidget(new QLabel(tr("Timeline zoom"),this)); m_zoom=new QSlider(Qt::Horizontal,this); m_zoom->setRange(25,2000);m_zoom->setValue(100); auto *fit=new QPushButton(tr("Fit width"),this); viewRow->addWidget(m_zoom,1);viewRow->addWidget(fit);root->addLayout(viewRow);
    m_enabled = new QCheckBox(tr("Enable processing"), this);
    m_delay = new QDoubleSpinBox(this); m_delay->setRange(-86400000, 86400000); m_delay->setDecimals(1); m_delay->setSuffix(" ms");
    m_tempo = new QDoubleSpinBox(this); m_tempo->setRange(0.01, 100.0); m_tempo->setDecimals(6); m_tempo->setSingleStep(.001); m_tempo->setValue(1.0);
    m_start = new QDoubleSpinBox(this); m_start->setRange(0, 999999); m_start->setDecimals(3); m_start->setSuffix(" s");
    m_end = new QDoubleSpinBox(this); m_end->setRange(0, 999999); m_end->setDecimals(3); m_end->setSuffix(" s");
    m_filters = new QLineEdit(this); m_filters->setPlaceholderText("volume=1.5,loudnorm=I=-16");
    layout->addRow(tr("Track"), m_track); layout->addRow(m_enabled); layout->addRow(tr("Sync offset"), m_delay);
    layout->addRow(tr("Speed factor"), m_tempo); layout->addRow(tr("Crop start"), m_start); layout->addRow(tr("Crop end (0 = full)"), m_end); layout->addRow(tr("FFmpeg filters"), m_filters);
    auto *previewButton = new QPushButton(tr("Preview 8 seconds"), this); layout->addRow(previewButton);
    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this); layout->addRow(buttons);
    connect(m_track, &QComboBox::currentIndexChanged, this, &AudioEffectsDialog::trackChanged);
    connect(play, &QPushButton::clicked, this, [this,play] { if (m_videoPlayer->playbackState()==QMediaPlayer::PlayingState) { m_videoPlayer->pause(); play->setText(tr("Play")); } else { m_videoPlayer->play(); play->setText(tr("Pause")); } });
    connect(m_loop, &QPushButton::toggled, this, [this](bool on) { if(on) m_videoPlayer->setLoops(QMediaPlayer::Infinite); else m_videoPlayer->setLoops(QMediaPlayer::Once); });
    connect(m_videoPosition,&QSlider::sliderMoved,this,[this](int v){m_videoPlayer->setPosition(v);m_timeLabel->setText(QString::number(v/1000.0,'f',3)+" s");});
    connect(m_videoPlayer,&QMediaPlayer::durationChanged,this,[this](qint64 d){m_videoPosition->setRange(0,int(qMin<qint64>(d,INT_MAX)));m_loopEnd->setMaximum(d/1000.0);});
    connect(m_videoPlayer,&QMediaPlayer::positionChanged,this,[this](qint64 p){if(!m_videoPosition->isSliderDown())m_videoPosition->setValue(int(p));m_timeLabel->setText(QString::number(p/1000.0,'f',3)+" s");if(m_loop->isChecked()&&p>=qRound64(m_loopEnd->value()*1000))m_videoPlayer->setPosition(qRound64(m_loopStart->value()*1000));});
    connect(m_zoom,&QSlider::valueChanged,this,[this](int v){m_timeline->setFitWidth(false);m_timeline->setZoom(v/100.0);}); connect(fit,&QPushButton::clicked,this,[this]{m_timeline->setFitWidth(true);});
    connect(m_timeline, &QAudioTimeline::offsetDragged, this, [this](int track,double value) { if(track==m_current){m_delay->setValue(value); save(); if(!m_renderDebounce){m_renderDebounce=new QTimer(this);m_renderDebounce->setSingleShot(true);connect(m_renderDebounce,&QTimer::timeout,this,&AudioEffectsDialog::preview);} m_renderDebounce->start(250);} });
    connect(m_timeline, &QAudioTimeline::trimDragged, this, [this](int track,double start,double end) { if(track==m_current){m_start->setValue(start);m_end->setValue(end);save();} });
    connect(buttons, &QDialogButtonBox::accepted, this, &AudioEffectsDialog::acceptChanges); connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(previewButton, &QPushButton::clicked, this, &AudioEffectsDialog::preview);
    if (m_track->count()) load(0);
}
void AudioEffectsDialog::save() { if (m_current < 0) return; auto &p = m_current < 1000 ? m_data.audioProcessing[m_current] : m_data.externAudioProcessing[m_current - 1000]; p.enabled=m_enabled->isChecked(); p.delayMs=m_delay->value(); p.tempo=m_tempo->value(); p.cropStartSeconds=m_start->value(); p.cropEndSeconds=m_end->value(); p.filters=m_filters->text().split(',', Qt::SkipEmptyParts); for (auto &f:p.filters) f=f.trimmed(); }
void AudioEffectsDialog::load(int i) { if (m_current >= 0) save(); m_current=m_track->itemData(i).toInt(); auto &p=m_current<1000?m_data.audioProcessing[m_current]:m_data.externAudioProcessing[m_current-1000]; m_enabled->setChecked(p.enabled);m_delay->setValue(p.delayMs);m_tempo->setValue(p.tempo);m_start->setValue(p.cropStartSeconds);m_end->setValue(p.cropEndSeconds);m_filters->setText(p.filters.join(',')); if(m_timeline && m_current<1000){m_timeline->setOffset(m_current,p.delayMs);m_timeline->setTrim(m_current,p.cropStartSeconds,p.cropEndSeconds);loadWaveform(m_current);} }
void AudioEffectsDialog::loadWaveform(int track) {
    if (m_source.isEmpty() || track < 0) return;
    if (m_waveformProcess) m_waveformProcess->kill();
    m_waveformProcess = new QProcess(this);
    connect(m_waveformProcess,&QProcess::finished,this,[this,track](int,QProcess::ExitStatus){
        const QByteArray raw=m_waveformProcess->readAllStandardOutput(); const int count=raw.size()/int(sizeof(float)); QVector<float> values; const int bins=1800; values.fill(0.0f,qMin(count,bins));
        if(count>0){for(int i=0;i<count;++i){float v=0;memcpy(&v,raw.constData()+i*sizeof(float),sizeof(float));int b=qMin(values.size()-1,i*values.size()/count);values[b]=qMax(values[b],float(qAbs(v)));} float mx=0;for(float v:values)mx=qMax(mx,v);if(mx>0)for(float &v:values)v=qBound(.03f,v/mx,1.0f);}
        m_timeline->setWaveform(track,values);
    });
    m_waveformProcess->start("ffmpeg",{"-v","error","-i",m_source,"-map",QString("0:a:%1").arg(track),"-ac","1","-ar","8000","-f","f32le","-"});
}
void AudioEffectsDialog::trackChanged(int i) { load(i); }
void AudioEffectsDialog::acceptChanges() { save(); accept(); }
void AudioEffectsDialog::preview() {
    save();
    if (m_current < 0 || m_current >= 1000 || m_source.isEmpty()) { QMessageBox::information(this, tr("Preview"), tr("Preview is available for internal audio tracks with a source file.")); return; }
    const auto &p = m_data.audioProcessing[m_current];
    const QString file = QDir::tempPath() + QString("/cineencoder-audio-preview-%1.wav").arg(++m_previewSequence);
    if (m_player) {
        m_player->stop();
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        m_player->setSource(QUrl());
#endif
    }
    const double start=m_loopStart ? m_loopStart->value() : 0.0; const double duration=qMax(0.1,(m_loopEnd ? m_loopEnd->value() : start+8)-start);
    m_videoPlayer->setPosition(qRound64(start*1000));
    QStringList args{"-y", "-ss", QString::number(start,'f',3), "-i", m_source, "-map", QString("0:a:%1").arg(m_current), "-t", QString::number(duration,'f',3)};
    const QString chain = p.filterChain();
    if (!chain.isEmpty()) { args << "-af" << chain; }
    args << "-c:a" << "pcm_s16le" << "-avoid_negative_ts" << "make_zero" << "-f" << "wav" << file;
    setWindowTitle(tr("Audio timing and effects — rendering %1").arg(chain.isEmpty() ? tr("original") : chain));
    if (m_previewProcess) { m_previewProcess->disconnect(this); if (m_previewProcess->state()!=QProcess::NotRunning) { m_previewProcess->kill(); m_previewProcess->waitForFinished(500); } m_previewProcess->deleteLater(); }
    m_previewProcess = new QProcess(this);
    connect(m_previewProcess, qOverload<int,QProcess::ExitStatus>(&QProcess::finished), this, [this,file,start](int code, QProcess::ExitStatus) {
        if (code != 0) { QMessageBox::warning(this, tr("Preview"), tr("FFmpeg could not generate the preview.")); return; }
        if (m_player) { m_player->stop(); m_player->deleteLater(); }
        m_player = new QMediaPlayer(this); m_audioOutput = new QAudioOutput(this); m_player->setAudioOutput(m_audioOutput);
        m_player->setLoops(m_loop && m_loop->isChecked() ? QMediaPlayer::Infinite : QMediaPlayer::Once);
        m_player->setSource(QUrl::fromLocalFile(file)); m_player->play();
        if (m_videoPlayer) { m_videoPlayer->setPosition(qRound64(start*1000)); m_videoPlayer->play(); }
    });
    m_previewProcess->start("ffmpeg", args);
}
