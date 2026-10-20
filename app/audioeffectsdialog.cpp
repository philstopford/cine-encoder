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
#include <QGuiApplication>
#include <QClipboard>
#include <algorithm>

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
    m_timeline = new QAudioTimeline(this); m_timeline->setTrackCount(data.fields[Constants::Data::audioFormats].size() + data.fields[Constants::Data::externAudioFormats].size()); m_timeline->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
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
    m_videoReference = new QComboBox(this);
    for (int i = 0; i < data.fields[Constants::Data::audioFormats].size(); ++i)
        m_videoReference->addItem(tr("Video audio %1: %2").arg(i + 1).arg(data.fields[Constants::Data::audioTitles][i]), i);
    m_syncReference = new QComboBox(this);
    for (int i = 0; i < data.fields[Constants::Data::externAudioFormats].size(); ++i)
        m_syncReference->addItem(tr("External %1: %2").arg(i + 1).arg(data.fields[Constants::Data::externAudioTitles][i]), 1000 + i);
    auto *analyzeButton = new QPushButton(tr("Analyze leading/trailing silence"), this);
    m_applySuggestedOffset = new QPushButton(tr("Apply estimated shift"), this);
    m_applySuggestedOffset->setEnabled(false);
    m_copyTiming = new QPushButton(tr("Copy timing adjustments"), this);
    m_pasteTiming = new QPushButton(tr("Paste timing adjustments"), this);
    m_syncResult = new QLabel(tr("Compare an internal video track with an external track containing the same programme audio. Copy its resulting timing adjustments to any other audio track when appropriate."), this);
    m_syncResult->setWordWrap(true);
    layout->addRow(tr("Video reference track"), m_videoReference); layout->addRow(tr("External comparison track"), m_syncReference); layout->addRow(analyzeButton); layout->addRow(m_syncResult); layout->addRow(m_applySuggestedOffset); layout->addRow(m_copyTiming); layout->addRow(m_pasteTiming);
    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this); layout->addRow(buttons);
    connect(m_track, &QComboBox::currentIndexChanged, this, &AudioEffectsDialog::trackChanged);
    connect(play, &QPushButton::clicked, this, [this,play] { if (m_videoPlayer->playbackState()==QMediaPlayer::PlayingState) { m_videoPlayer->pause(); play->setText(tr("Play")); } else { m_videoPlayer->play(); play->setText(tr("Pause")); } });
    connect(m_loop, &QPushButton::toggled, this, [this](bool on) { if(on) m_videoPlayer->setLoops(QMediaPlayer::Infinite); else m_videoPlayer->setLoops(QMediaPlayer::Once); });
    connect(m_videoPosition,&QSlider::sliderMoved,this,[this](int v){m_videoPlayer->setPosition(v);m_timeLabel->setText(QString::number(v/1000.0,'f',3)+" s");});
    connect(m_videoPlayer,&QMediaPlayer::durationChanged,this,[this](qint64 d){m_videoPosition->setRange(0,int(qMin<qint64>(d,INT_MAX)));m_loopEnd->setMaximum(d/1000.0);m_timeline->setDuration(d / 1000.0);});
    connect(m_videoPlayer,&QMediaPlayer::positionChanged,this,[this](qint64 p){if(!m_videoPosition->isSliderDown())m_videoPosition->setValue(int(p));m_timeLabel->setText(QString::number(p/1000.0,'f',3)+" s");if(m_loop->isChecked()&&p>=qRound64(m_loopEnd->value()*1000))m_videoPlayer->setPosition(qRound64(m_loopStart->value()*1000));});
    connect(m_zoom,&QSlider::valueChanged,this,[this](int v){m_timeline->setFitWidth(false);m_timeline->setZoom(v/100.0);}); connect(fit,&QPushButton::clicked,this,[this]{m_timeline->setFitWidth(true);});
    connect(m_timeline, &QAudioTimeline::offsetDragged, this, [this](int track,double value) { if(logicalTrack(track)==m_current){m_delay->setValue(value); save(); if(!m_renderDebounce){m_renderDebounce=new QTimer(this);m_renderDebounce->setSingleShot(true);connect(m_renderDebounce,&QTimer::timeout,this,&AudioEffectsDialog::preview);} m_renderDebounce->start(250);} });
    connect(m_timeline, &QAudioTimeline::trimDragged, this, [this](int track,double start,double end) { if(logicalTrack(track)==m_current){m_start->setValue(start);m_end->setValue(end);save();} });
    connect(buttons, &QDialogButtonBox::accepted, this, &AudioEffectsDialog::acceptChanges); connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(previewButton, &QPushButton::clicked, this, &AudioEffectsDialog::preview);
    connect(analyzeButton, &QPushButton::clicked, this, &AudioEffectsDialog::analyzeSync);
    connect(m_applySuggestedOffset, &QPushButton::clicked, this, [this] {
        save();
        const int reference = m_syncReference->currentData().toInt() - 1000;
        m_data.externAudioProcessing[reference].enabled = true;
        m_data.externAudioProcessing[reference].delayMs = m_suggestedOffsetMs;
        const int logical = 1000 + reference;
        m_timeline->setOffset(timelineTrack(logical), m_suggestedOffsetMs);
        if (m_current == logical) { m_enabled->setChecked(true); m_delay->setValue(m_suggestedOffsetMs); }
    });
    connect(m_copyTiming, &QPushButton::clicked, this, [this] {
        save();
        const auto &p = m_current < 1000 ? m_data.audioProcessing[m_current] : m_data.externAudioProcessing[m_current - 1000];
        QGuiApplication::clipboard()->setText(QString("cine-encoder-audio-timing-v1;%1;%2;%3;%4")
            .arg(p.delayMs, 0, 'f', 1).arg(p.tempo, 0, 'f', 6).arg(p.cropStartSeconds, 0, 'f', 3).arg(p.cropEndSeconds, 0, 'f', 3));
    });
    connect(m_pasteTiming, &QPushButton::clicked, this, [this] {
        const QStringList values = QGuiApplication::clipboard()->text().split(';');
        if (values.size() != 5 || values.first() != "cine-encoder-audio-timing-v1") { QMessageBox::information(this, tr("Paste timing adjustments"), tr("The clipboard does not contain Cine Encoder timing adjustments.")); return; }
        save();
        auto &p = m_current < 1000 ? m_data.audioProcessing[m_current] : m_data.externAudioProcessing[m_current - 1000];
        bool valid = false; const double delay = values[1].toDouble(&valid); if (!valid) return;
        const double tempo = values[2].toDouble(&valid); if (!valid) return;
        const double start = values[3].toDouble(&valid); if (!valid) return;
        const double end = values[4].toDouble(&valid); if (!valid) return;
        p.enabled = true; p.delayMs = delay; p.tempo = tempo; p.cropStartSeconds = start; p.cropEndSeconds = end;
        m_enabled->setChecked(true); m_delay->setValue(delay); m_tempo->setValue(tempo); m_start->setValue(start); m_end->setValue(end);
        m_timeline->setOffset(timelineTrack(m_current), delay); m_timeline->setTrim(timelineTrack(m_current), start, end);
    });
    for (int i = 0; i < m_track->count(); ++i) {
        const int logical = m_track->itemData(i).toInt();
        const auto &processing = logical < 1000 ? m_data.audioProcessing[logical] : m_data.externAudioProcessing[logical - 1000];
        m_timeline->setOffset(timelineTrack(logical), processing.delayMs);
        m_timeline->setTrim(timelineTrack(logical), processing.cropStartSeconds, processing.cropEndSeconds);
    }
    if (m_track->count()) load(0);
}
AudioEffectsDialog::~AudioEffectsDialog() {
    if (m_renderDebounce) m_renderDebounce->stop();
    if (m_waveformProcess) {
        disconnect(m_waveformProcess, nullptr, this, nullptr);
        if (m_waveformProcess->state() != QProcess::NotRunning) {
            m_waveformProcess->kill();
            m_waveformProcess->waitForFinished(500);
        }
    }
    if (m_previewProcess) {
        disconnect(m_previewProcess, nullptr, this, nullptr);
        if (m_previewProcess->state() != QProcess::NotRunning) {
            m_previewProcess->kill();
            m_previewProcess->waitForFinished(500);
        }
    }
}
void AudioEffectsDialog::save() { if (m_current < 0) return; auto &p = m_current < 1000 ? m_data.audioProcessing[m_current] : m_data.externAudioProcessing[m_current - 1000]; p.enabled=m_enabled->isChecked(); p.delayMs=m_delay->value(); p.tempo=m_tempo->value(); p.cropStartSeconds=m_start->value(); p.cropEndSeconds=m_end->value(); p.filters=m_filters->text().split(',', Qt::SkipEmptyParts); for (auto &f:p.filters) f=f.trimmed(); }
void AudioEffectsDialog::load(int i) { if (m_current >= 0) save(); m_current=m_track->itemData(i).toInt(); auto &p=m_current<1000?m_data.audioProcessing[m_current]:m_data.externAudioProcessing[m_current-1000]; m_enabled->setChecked(p.enabled);m_delay->setValue(p.delayMs);m_tempo->setValue(p.tempo);m_start->setValue(p.cropStartSeconds);m_end->setValue(p.cropEndSeconds);m_filters->setText(p.filters.join(',')); if(m_timeline){const int t=timelineTrack(m_current);m_timeline->setOffset(t,p.delayMs);m_timeline->setTrim(t,p.cropStartSeconds,p.cropEndSeconds);loadWaveform(t);} }
void AudioEffectsDialog::loadWaveform(int track) {
    const int logical = logicalTrack(track); const QString file = trackFile(logical); if (file.isEmpty() || track < 0) return;
    if (m_waveformProcess) m_waveformProcess->kill();
    m_waveformProcess = new QProcess(this);
    connect(m_waveformProcess,&QProcess::finished,this,[this,track](int,QProcess::ExitStatus){
        const QByteArray raw=m_waveformProcess->readAllStandardOutput(); const int count=raw.size()/int(sizeof(float)); QVector<float> values; const int bins=1800; values.fill(0.0f,qMin(count,bins));
        if(count>0){for(int i=0;i<count;++i){float v=0;memcpy(&v,raw.constData()+i*sizeof(float),sizeof(float));int b=qMin(values.size()-1,i*values.size()/count);values[b]=qMax(values[b],float(qAbs(v)));} float mx=0;for(float v:values)mx=qMax(mx,v);if(mx>0)for(float &v:values)v=qBound(.03f,v/mx,1.0f);}
        m_timeline->setWaveform(track,values);
    });
    m_waveformProcess->start("ffmpeg",{"-v","error","-i",file,"-map",trackMap(logical),"-ac","1","-ar","8000","-f","f32le","-"});
}
void AudioEffectsDialog::trackChanged(int i) { load(i); }
void AudioEffectsDialog::acceptChanges() { save(); accept(); }
void AudioEffectsDialog::preview() {
    save();
    if (m_current < 0 || trackFile(m_current).isEmpty()) { QMessageBox::information(this, tr("Preview"), tr("No audio track is available for preview.")); return; }
    const auto &p = m_current < 1000 ? m_data.audioProcessing[m_current] : m_data.externAudioProcessing[m_current - 1000];
    const QString file = QDir::tempPath() + QString("/cineencoder-audio-preview-%1.wav").arg(++m_previewSequence);
    if (m_player) {
        m_player->stop();
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        m_player->setSource(QUrl());
#endif
    }
    const double start=m_loopStart ? m_loopStart->value() : 0.0; const double duration=qMax(0.1,(m_loopEnd ? m_loopEnd->value() : start+8)-start);
    m_videoPlayer->setPosition(qRound64(start*1000));
    QStringList args{"-y", "-ss", QString::number(start,'f',3), "-i", trackFile(m_current), "-map", trackMap(m_current), "-t", QString::number(duration,'f',3)};
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

int AudioEffectsDialog::timelineTrack(int track) const {
    return track < 1000 ? track : m_data.fields[Constants::Data::audioFormats].size() + track - 1000;
}

int AudioEffectsDialog::logicalTrack(int track) const {
    const int internal = m_data.fields[Constants::Data::audioFormats].size();
    return track < internal ? track : 1000 + track - internal;
}

QString AudioEffectsDialog::trackFile(int track) const {
    if (track < 1000) return m_source;
    const int external = track - 1000;
    return external >= 0 && external < m_data.fields[Constants::Data::externAudioPath].size()
        ? m_data.fields[Constants::Data::externAudioPath][external] : QString();
}

QString AudioEffectsDialog::trackMap(int track) const {
    return track < 1000 ? QString("0:a:%1").arg(track) : QStringLiteral("0:a:0");
}

static QVector<float> syncEnvelope(const QByteArray &raw) {
    const int samples = raw.size() / int(sizeof(float)), block = 100; // 100 ms at 1 kHz
    QVector<float> out;
    out.reserve(samples / block);
    for (int start = 0; start < samples; start += block) {
        double sum = 0; const int end = qMin(start + block, samples);
        for (int i = start; i < end; ++i) { float value = 0; memcpy(&value, raw.constData() + i * int(sizeof(float)), sizeof(float)); sum += value * value; }
        out.append(float(std::sqrt(sum / qMax(1, end - start))));
    }
    return out;
}

static QPair<int, int> audibleBounds(const QVector<float> &envelope) {
    if (envelope.isEmpty()) return {-1, -1};
    QVector<float> ordered = envelope; std::sort(ordered.begin(), ordered.end());
    const float peak = ordered[qMin(ordered.size() - 1, int(ordered.size() * .95))];
    const float threshold = qMax(0.0001f, peak * .06f);
    constexpr int sustainedBlocks = 5; // ignore clicks: require half a second of signal
    int first = -1, last = -1, run = 0;
    for (int i = 0; i < envelope.size(); ++i) { run = envelope[i] >= threshold ? run + 1 : 0; if (run >= sustainedBlocks) { first = i - sustainedBlocks + 1; break; } }
    run = 0;
    for (int i = envelope.size() - 1; i >= 0; --i) { run = envelope[i] >= threshold ? run + 1 : 0; if (run >= sustainedBlocks) { last = i + sustainedBlocks - 1; break; } }
    return {first, last};
}

void AudioEffectsDialog::analyzeSync() {
    if (m_data.fields[Constants::Data::audioFormats].isEmpty() || m_syncReference->count() == 0) {
        QMessageBox::information(this, tr("Sync analysis"), tr("Add an external timing source and select an internal video reference track.")); return;
    }
    m_syncResult->setText(tr("Decoding low-resolution audio envelopes…")); m_applySuggestedOffset->setEnabled(false);
    m_analysisTarget.clear(); m_analysisReference.clear();
    const int targetTrack = m_videoReference->currentData().toInt();
    decodeAnalysisWaveform(m_source, trackMap(targetTrack), true);
}

void AudioEffectsDialog::decodeAnalysisWaveform(const QString &file, const QString &map, bool target) {
    auto *process = new QProcess(this);
    connect(process, qOverload<int, QProcess::ExitStatus>(&QProcess::finished), this, [this, process, target](int code, QProcess::ExitStatus) {
        const QVector<float> envelope = code == 0 ? syncEnvelope(process->readAllStandardOutput()) : QVector<float>();
        process->deleteLater();
        if (envelope.isEmpty()) { m_syncResult->setText(tr("Could not decode the audio needed for sync analysis.")); return; }
        if (target) { m_analysisTarget = envelope; const int track = m_syncReference->currentData().toInt(); decodeAnalysisWaveform(trackFile(track), trackMap(track), false); }
        else { m_analysisReference = envelope; finishSyncAnalysis(); }
    });
    process->start("ffmpeg", {"-v", "error", "-i", file, "-map", map, "-ac", "1", "-ar", "1000", "-f", "f32le", "-"});
}

void AudioEffectsDialog::finishSyncAnalysis() {
    const auto target = audibleBounds(m_analysisTarget), reference = audibleBounds(m_analysisReference);
    if (target.first < 0 || reference.first < 0) { m_syncResult->setText(tr("No sustained non-silent section was found. Check the selected reference track.")); return; }
    m_suggestedOffsetMs = (target.first - reference.first) * 100.0;
    const double targetEnd = target.second * .1, referenceEnd = reference.second * .1;
    m_syncResult->setText(tr("First sustained audio: video %1 s, reference %2 s. Suggested offset: %3 ms. Last sustained audio: video %4 s, reference %5 s. Apply this when the reference is the original programme audio; use Preview to verify a scene before encoding.")
        .arg(target.first * .1, 0, 'f', 1).arg(reference.first * .1, 0, 'f', 1).arg(qRound(m_suggestedOffsetMs)).arg(targetEnd, 0, 'f', 1).arg(referenceEnd, 0, 'f', 1));
    m_applySuggestedOffset->setEnabled(true);
}
