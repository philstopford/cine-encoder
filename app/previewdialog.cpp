#include "previewdialog.h"
#include "widgets/qeffectstack.h"
#include "widgets/qabvideowidget.h"

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QAudioOutput>
#endif
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QFileInfo>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QMediaPlayer>
#include <QPushButton>
#include <QSlider>
#include <QSpinBox>
#include <QTemporaryDir>
#include <QUrl>
#include <QVBoxLayout>
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QVideoFrame>
#include <QVideoSink>
#else
#include <QMediaContent>
#include <QVideoProbe>
#endif
#include <algorithm>

namespace {
QString previewPixelFormat(const QString &inputFile)
{
    QProcess probe;
    probe.start("ffprobe", {"-v", "error", "-select_streams", "v:0",
                             "-show_entries", "stream=pix_fmt",
                             "-of", "default=nw=1:nk=1", inputFile});
    if (!probe.waitForFinished(3000)) return QStringLiteral("yuv420p");
    const QString format = QString::fromLocal8Bit(probe.readAllStandardOutput()).trimmed().toLower();
    // H.264 High 10 preview preserves the precision of the common 10-bit
    // YUV formats. Other formats remain on the broadly supported 8-bit path.
    return format.contains("10") ? QStringLiteral("yuv420p10le") : QStringLiteral("yuv420p");
}

bool isDenoiseFilter(const QString &filter)
{
    const QString n = filter.trimmed().section('=', 0, 0).toLower();
    return n == "hqdn3d" || n == "atadenoise" || n == "nlmeans" || n == "bm3d" ||
           n == "vaguedenoiser" || n == "dctdnoiz" || n == "fftdnoiz" || n == "owdenoise";
}
}

PreviewDialog::PreviewDialog(const PreviewRequest &request, QWidget *parent)
    : QDialog(parent), m_request(request), m_tempDir(new QTemporaryDir),
      m_renderer(new QProcess(this)), m_originalPlayer(new QMediaPlayer(this)),
      m_processedPlayer(new QMediaPlayer(this)),
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
      m_audioOutput(new QAudioOutput(this)),
#endif
      m_video(new QABVideoWidget(this)), m_wipe(new QSlider(Qt::Horizontal, this)),
      m_previousFrame(new QPushButton(tr("◀ Frame"), this)), m_nextFrame(new QPushButton(tr("Frame ▶"), this)),
      m_status(new QLabel(this)), m_play(new QPushButton(tr("Play"), this)), m_position(new QSlider(Qt::Horizontal, this)),
      m_start(new QDoubleSpinBox(this)), m_duration(new QSpinBox(this)), m_render(new QPushButton(tr("Render preview"), this)),
      m_applyDenoise(new QCheckBox(tr("Apply denoise filters"), this)), m_effectStack(new QEffectStack(this)),
      m_workingFilters(request.videoFilters)
{
    setWindowTitle(request.title.isEmpty() ? tr("Settings preview") : request.title);
    resize(1200, 900); setMinimumSize(800, 600);
    auto *root = new QVBoxLayout(this); root->addWidget(m_video, 1);
    auto *zoomRow = new QHBoxLayout;
    auto *zoomLabel = new QLabel(tr("Zoom: 100%"), this);
    auto *resetZoom = new QPushButton(tr("Reset zoom"), this);
    zoomRow->addWidget(zoomLabel); zoomRow->addWidget(resetZoom); zoomRow->addStretch(); root->addLayout(zoomRow);
    root->addWidget(m_status);
    auto *range = new QHBoxLayout; m_start->setDecimals(2); m_start->setSuffix(tr(" s"));
    m_start->setRange(0, request.sourceDurationSeconds > 0 ? request.sourceDurationSeconds : 359999); m_start->setValue(request.startSeconds);
    m_duration->setRange(1, 120); m_duration->setSuffix(tr(" s")); m_duration->setValue(request.durationSeconds);
    range->addWidget(new QLabel(tr("Start"))); range->addWidget(m_start); range->addWidget(new QLabel(tr("Duration"))); range->addWidget(m_duration);
    range->addWidget(m_applyDenoise); range->addWidget(m_render); range->addStretch(); root->addLayout(range);
    m_wipe->setRange(0, 100); m_wipe->setValue(50);
    auto *wipeBox = new QGroupBox(tr("A/B comparison — drag the divider")); auto *wipe = new QHBoxLayout(wipeBox);
    wipe->addWidget(new QLabel(tr("Original ◀"))); wipe->addWidget(m_wipe, 1); wipe->addWidget(new QLabel(tr("▶ Effects"))); root->addWidget(wipeBox);
    m_effectStack->setFilters(&m_workingFilters); m_effectStack->setPreviewButtonVisible(false); root->addWidget(m_effectStack, 1);
    auto *controls = new QHBoxLayout; controls->addWidget(m_previousFrame); controls->addWidget(m_play); controls->addWidget(m_position, 1); controls->addWidget(m_nextFrame); root->addLayout(controls);
    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel); root->addWidget(buttons);
    m_play->setEnabled(false); m_position->setEnabled(false); m_previousFrame->setEnabled(false); m_nextFrame->setEnabled(false);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    m_processedPlayer->setAudioOutput(m_audioOutput);
    m_originalPtsOffset = -1; m_processedPtsOffset = -1;
    auto *os = new QVideoSink(this); auto *ps = new QVideoSink(this); m_originalPlayer->setVideoSink(os); m_processedPlayer->setVideoSink(ps);
    connect(os, &QVideoSink::videoFrameChanged, this, [this](const QVideoFrame &f) {
        if (f.isValid()) { const qint64 pts = f.startTime() / 1000; if (m_originalPtsOffset < 0) m_originalPtsOffset = m_originalPlayer->position() - pts; setOriginalFrame(f.toImage(), pts + m_originalPtsOffset - qRound64(m_start->value() * 1000)); }
    });
    connect(ps, &QVideoSink::videoFrameChanged, this, [this](const QVideoFrame &f) {
        if (f.isValid()) { const qint64 pts = f.startTime() / 1000; if (m_processedPtsOffset < 0) m_processedPtsOffset = m_processedPlayer->position() - pts; setProcessedFrame(f.toImage(), pts + m_processedPtsOffset); }
    });
#else
    auto *op = new QVideoProbe(this); auto *pp = new QVideoProbe(this); op->setSource(m_originalPlayer); pp->setSource(m_processedPlayer); m_originalPtsOffset = -1; m_processedPtsOffset = -1;
    connect(op, &QVideoProbe::videoFrameProbed, this, [this](const QVideoFrame &f) { QVideoFrame copy(f); if (copy.map(QAbstractVideoBuffer::ReadOnly)) { const qint64 pts = copy.startTime() / 1000; if (m_originalPtsOffset < 0) m_originalPtsOffset = m_originalPlayer->position() - pts; setOriginalFrame(copy.image(), pts + m_originalPtsOffset - qRound64(m_start->value() * 1000)); copy.unmap(); } });
    connect(pp, &QVideoProbe::videoFrameProbed, this, [this](const QVideoFrame &f) { QVideoFrame copy(f); if (copy.map(QAbstractVideoBuffer::ReadOnly)) { const qint64 pts = copy.startTime() / 1000; if (m_processedPtsOffset < 0) m_processedPtsOffset = m_processedPlayer->position() - pts; setProcessedFrame(copy.image(), pts + m_processedPtsOffset); copy.unmap(); } });
#endif
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept); connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(m_play, &QPushButton::clicked, this, &PreviewDialog::togglePlayback); connect(m_render, &QPushButton::clicked, this, &PreviewDialog::render);
    connect(m_wipe, &QSlider::valueChanged, this, [this](int v) { m_video->setDivider(v); }); connect(m_position, &QSlider::sliderMoved, this, &PreviewDialog::seek);
    connect(m_video, &QABVideoWidget::dividerMoved, m_wipe, &QSlider::setValue);
    connect(m_video, &QABVideoWidget::zoomChanged, this, [zoomLabel](int value) { zoomLabel->setText(QObject::tr("Zoom: %1%").arg(value)); });
    connect(resetZoom, &QPushButton::clicked, m_video, &QABVideoWidget::resetZoom);
    connect(m_previousFrame, &QPushButton::clicked, this, [this] { stepFrame(-1); }); connect(m_nextFrame, &QPushButton::clicked, this, [this] { stepFrame(1); });
    connect(m_renderer, QOverload<int,QProcess::ExitStatus>::of(&QProcess::finished), this, &PreviewDialog::renderFinished);
    connect(m_processedPlayer, &QMediaPlayer::positionChanged, this, &PreviewDialog::updatePosition);
    connect(m_processedPlayer, &QMediaPlayer::durationChanged, this, [this](qint64 duration) {
        if (duration > 0)
            m_position->setRange(0, static_cast<int>(qMin<qint64>(duration, INT_MAX)));
    });
    connect(m_originalPlayer, &QMediaPlayer::mediaStatusChanged, this, &PreviewDialog::originalStatusChanged);
    connect(m_processedPlayer, &QMediaPlayer::mediaStatusChanged, this, &PreviewDialog::processedStatusChanged);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    connect(m_originalPlayer, &QMediaPlayer::errorOccurred, this, [this](QMediaPlayer::Error, const QString &error) {
        m_status->setText(tr("Original preview playback error: %1").arg(error));
    });
    connect(m_processedPlayer, &QMediaPlayer::errorOccurred, this, [this](QMediaPlayer::Error, const QString &error) {
        m_status->setText(tr("Processed preview playback error: %1").arg(error));
    });
#else
    connect(m_originalPlayer, QOverload<QMediaPlayer::Error>::of(&QMediaPlayer::error), this, [this](QMediaPlayer::Error) {
        m_status->setText(tr("Original preview playback error."));
    });
    connect(m_processedPlayer, QOverload<QMediaPlayer::Error>::of(&QMediaPlayer::error), this, [this](QMediaPlayer::Error) {
        m_status->setText(tr("Processed preview playback error."));
    });
#endif
    connect(m_applyDenoise, &QCheckBox::toggled, this, [this] { render(); });
    connect(m_effectStack, &QEffectStack::filtersChanged, this, [this] { m_status->setText(tr("Effects changed. Select Render preview to review them.")); });
    render();
}

PreviewDialog::~PreviewDialog() { if (m_renderer->state() != QProcess::NotRunning) m_renderer->kill(); delete m_tempDir; }
QStringList PreviewDialog::filters() const { return m_workingFilters; }

void PreviewDialog::render()
{
    if (!QFileInfo::exists(m_request.inputFile) || !m_tempDir->isValid()) return;
    if (m_renderer->state() != QProcess::NotRunning) m_renderer->kill();
    m_originalPlayer->stop(); m_processedPlayer->stop(); m_sourcesReady = false;
    m_originalPtsOffset = -1; m_processedPtsOffset = -1;
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
    m_originalPlayer->setSource(QUrl()); m_processedPlayer->setSource(QUrl());
#else
    m_originalPlayer->setMedia(QMediaContent()); m_processedPlayer->setMedia(QMediaContent());
#endif
    m_play->setEnabled(false); m_position->setEnabled(false); m_previousFrame->setEnabled(false); m_nextFrame->setEnabled(false); m_render->setEnabled(false);
    m_outputFile = m_tempDir->filePath(QString("preview-%1.mp4").arg(++m_renderSequence));
    QStringList args{"-hide_banner","-loglevel","error","-y"}; if (m_start->value() > 0) args << "-ss" << QString::number(m_start->value(),'f',3);
    args << m_request.ffmpegInputArguments << "-i" << m_request.inputFile << "-t" << QString::number(m_duration->value());
    QStringList filters; for (const QString &f : QEffectStack::enabledFilters(m_workingFilters)) if (m_applyDenoise->isChecked() || !isDenoiseFilter(f)) filters << f;
    if (!filters.isEmpty()) args << "-vf" << filters.join(',');
    const QString pixelFormat = previewPixelFormat(m_request.inputFile);
    args << "-map" << "0:v:0" << "-map" << "0:a:0?" << "-c:v" << "libx264" << "-preset" << "ultrafast" << "-crf" << "25";
    if (pixelFormat == QLatin1String("yuv420p10le")) args << "-profile:v" << "high10";
    args << "-pix_fmt" << pixelFormat << "-c:a" << "aac" << "-movflags" << "+faststart" << m_request.ffmpegOutputArguments << m_outputFile;
    m_status->setText(tr("Rendering %1-bit preview…").arg(pixelFormat == QLatin1String("yuv420p10le") ? 10 : 8));
    m_renderer->start("ffmpeg", args);
}

void PreviewDialog::renderFinished(int code, QProcess::ExitStatus status)
{
    m_render->setEnabled(true); if (status != QProcess::NormalExit || code != 0) { m_status->setText(tr("Preview rendering failed: %1").arg(QString::fromLocal8Bit(m_renderer->readAllStandardError()))); return; }
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
    m_originalPlayer->setSource(QUrl::fromLocalFile(m_request.inputFile)); m_processedPlayer->setSource(QUrl::fromLocalFile(m_outputFile));
#else
    m_originalPlayer->setMedia(QUrl::fromLocalFile(m_request.inputFile)); m_processedPlayer->setMedia(QUrl::fromLocalFile(m_outputFile));
#endif
    m_status->setText(tr("Preview ready."));
}

void PreviewDialog::originalStatusChanged(int status)
{
    Q_UNUSED(status);
    tryEnablePlayback();
}
void PreviewDialog::processedStatusChanged(int status)
{
    Q_UNUSED(status);
    tryEnablePlayback();
}
void PreviewDialog::tryEnablePlayback()
{
    const bool originalReady = m_originalPlayer->mediaStatus() == QMediaPlayer::LoadedMedia ||
                               m_originalPlayer->mediaStatus() == QMediaPlayer::BufferedMedia;
    const bool processedReady = m_processedPlayer->mediaStatus() == QMediaPlayer::LoadedMedia ||
                                m_processedPlayer->mediaStatus() == QMediaPlayer::BufferedMedia;
    if (!originalReady || !processedReady || m_sourcesReady) return;
    m_sourcesReady = true;
    seekPlayers(0);
    m_play->setEnabled(true);
    m_position->setEnabled(true);
    m_previousFrame->setEnabled(true);
    m_nextFrame->setEnabled(true);
}
void PreviewDialog::togglePlayback()
{
    if (!m_sourcesReady) return;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    const bool playing = m_processedPlayer->playbackState() == QMediaPlayer::PlayingState;
#else
    const bool playing = m_processedPlayer->state() == QMediaPlayer::PlayingState;
#endif
    if (playing) { m_originalPlayer->pause(); m_processedPlayer->pause(); m_play->setText(tr("Play")); }
    else { m_originalPlayer->play(); m_processedPlayer->play(); m_play->setText(tr("Pause")); }
}
void PreviewDialog::seek(int value) { seekPlayers(value); }
void PreviewDialog::updateWipe(int value) { m_video->setDivider(value); }
void PreviewDialog::seekPlayers(qint64 position)
{
    if (m_syncing) return; m_syncing = true; m_processedPlayer->setPosition(position); m_originalPlayer->setPosition(m_start->value() * 1000 + position); m_syncing = false;
}
void PreviewDialog::updatePosition(qint64 position)
{
    if (!m_syncing) {
        m_position->setValue(static_cast<int>(position));
        const qint64 target = m_start->value() * 1000 + position;
        // The processed file is the local-time master. Keep the source
        // decoder close enough that its next decoded frame cannot visibly
        // lead the processed frame at the wipe.
        if (qAbs(m_originalPlayer->position() - target) > 20)
            m_originalPlayer->setPosition(target);
    }
    if (m_processedPlayer->mediaStatus() == QMediaPlayer::EndOfMedia) {
        const qint64 finalPosition = qMax<qint64>(0, m_processedPlayer->duration() - 1);
        m_syncing = true;
        m_originalPlayer->setPosition(m_start->value() * 1000 + finalPosition);
        m_processedPlayer->setPosition(finalPosition);
        m_originalPlayer->pause();
        m_processedPlayer->pause();
        m_syncing = false;
        m_position->setValue(static_cast<int>(finalPosition));
        m_play->setText(tr("Play"));
    }
}
void PreviewDialog::stepFrame(int direction) { seekPlayers(qBound<qint64>(0, m_processedPlayer->position() + direction * 40, m_processedPlayer->duration())); }
void PreviewDialog::setOriginalFrame(const QImage &frame, qint64 timestampMs) { m_video->setOriginalFrame(frame, timestampMs); }
void PreviewDialog::setProcessedFrame(const QImage &frame, qint64 timestampMs) { m_video->setProcessedFrame(frame, timestampMs); }
