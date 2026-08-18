#include "qeffectstack.h"

#include <QComboBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QSplitter>
#include <QTableWidget>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QProcess>
#include <QRegularExpression>

namespace {
struct FilterPreset { const char *name; const char *expression; const char *description; };
const FilterPreset presets[] = {
    {"Color adjustment", "eq=brightness=0:contrast=1:saturation=1:gamma=1", "Brightness -1…1; contrast, saturation and gamma commonly use 0…3."},
    {"Hue", "hue=h=0:s=1", "Rotate hue in degrees and adjust saturation."},
    {"Blur", "boxblur=luma_radius=2:luma_power=1", "Box blur radius and number of passes."},
    {"Sharpen", "unsharp=5:5:1.0", "Sharpen with matrix width, height and strength."},
    {"Denoise — HQDN3D (fast)", "hqdn3d=luma_spatial=4:chroma_spatial=3:luma_tmp=6:chroma_tmp=4.5", "Fast high-quality 3D denoising. Spatial values clean each frame; luma_tmp and chroma_tmp remove noise across adjacent frames."},
    {"Denoise — Adaptive temporal", "atadenoise=0a=0.02:0b=0.04:1a=0.02:1b=0.04:2a=0.02:2b=0.04:s=9", "Averages similar pixels over time while preserving changing areas. Increase s for a wider temporal window; thresholds 0a…2b control each plane."},
    {"Denoise — Non-local means", "nlmeans=s=3:p=7:r=15", "High-quality spatial denoising that preserves detail, but is substantially slower. s is strength; p and r control patch and search sizes."},
    {"Denoise — BM3D (very slow)", "bm3d=sigma=3:block=8:bstep=1:group=1:range=9:mstep=1:estim=basic", "Strong block-matching 3D denoising. Sigma controls strength. This basic single-input mode is computationally expensive."},
    {"Artifact cleanup — SPP", "spp=quality=4:use_bframe_qp=1", "Simple postprocessing for blocking and ringing artifacts. Quality 4 is a conservative speed/quality default."},
    {"Artifact cleanup — USPP", "uspp=quality=4", "Higher-quality, slower postprocessing for compressed sources. Reduce quality to 2 or 3 for faster previews."},
    {"Artifact cleanup — FSPP", "fspp=quality=4:strength=0", "Fast postprocessing/deblocking. Strength 0 uses the filter default; raise it carefully to avoid softening detail."},
    {"Artifact cleanup — PP7", "pp7=qp=1", "Postprocessing 7 for strong MPEG-style blocking. qp=1 is a gentle starting point."},
    {"Artifact cleanup — Libplacebo deband", "libplacebo=deband=1:deband_iterations=2:deband_threshold=4:deband_radius=8:deband_grain=4", "High-quality banding/debanding cleanup. Requires an FFmpeg build with libplacebo; GPU support may be required."},
    {"Vignette", "vignette=angle=PI/5", "Darken the edges of the image."},
    {"Film grain / noise", "noise=alls=8:allf=t+u", "Add temporal uniform noise; alls controls strength."},
    {"Fade in", "fade=t=in:st=0:d=1", "Fade video in; st and d are seconds."},
    {"Fade out", "fade=t=out:st=5:d=1", "Fade video out; st and d are seconds."},
    {"Crop", "crop=w=iw:h=ih:x=0:y=0", "Crop using output width/height and origin."},
    {"Scale", "scale=w=-2:h=1080", "Resize while preserving aspect ratio with -1 or -2."},
    {"Rotate 90°", "transpose=clock", "Rotate clockwise. Use cclock for counter-clockwise."},
    {"Horizontal flip", "hflip", "Mirror the image horizontally."},
    {"Vertical flip", "vflip", "Mirror the image vertically."},
    {"Grayscale", "hue=s=0", "Remove color saturation."},
    {"Negative", "negate", "Invert RGB components."},
    {"Deinterlace", "yadif", "YADIF deinterlacing with FFmpeg defaults."},
    {"Custom FFmpeg filter", "", "Enter any video-filter expression supported by this FFmpeg installation."}
};

QString displayName(const QString &expression)
{
    QString active = expression;
    if (active.startsWith("#disabled:")) active.remove(0, 10);
    for (const auto &preset : presets)
        if (active == QString::fromLatin1(preset.expression) && *preset.name)
            return QObject::tr(preset.name);
    const QString id = active.section('=', 0, 0);
    for (const auto &preset : presets)
        if (QString::fromLatin1(preset.expression).section('=', 0, 0) == id && !id.isEmpty())
            return QObject::tr(preset.name);
    return id.isEmpty() ? QObject::tr("Custom filter") : id;
}
}

QEffectStack::QEffectStack(QWidget *parent) : QWidget(parent)
{
    auto *root = new QVBoxLayout(this);
    auto *addRow = new QHBoxLayout;
    m_catalog = new QComboBox(this);
    QProcess filterProbe;
    filterProbe.start("ffmpeg", {"-hide_banner", "-filters"});
    filterProbe.waitForFinished(3000);
    const QString available = QString::fromLocal8Bit(filterProbe.readAllStandardOutput());
    for (const auto &preset : presets) {
        const QString filterName = QString::fromLatin1(preset.expression).section('=', 0, 0);
        if (!filterName.isEmpty() && !available.contains(QRegularExpression("\\b" + QRegularExpression::escape(filterName) + "\\b")))
            continue;
        m_catalog->addItem(tr(preset.name), QString::fromLatin1(preset.expression));
        m_catalog->setItemData(m_catalog->count() - 1, tr(preset.description), Qt::ToolTipRole);
    }
    auto *add = new QPushButton(tr("Add"), this);
    addRow->addWidget(m_catalog, 1); addRow->addWidget(add);
    root->addLayout(addRow);

    auto *splitter = new QSplitter(this);
    m_list = new QListWidget(splitter);
    m_list->setAlternatingRowColors(true);
    auto *editor = new QWidget(splitter);
    auto *form = new QFormLayout(editor);
    m_description = new QLabel(editor);
    m_description->setWordWrap(true);
    m_expression = new QLineEdit(editor);
    m_expression->setPlaceholderText(tr("filter=option=value:option=value"));
    m_parameters = new QTableWidget(editor);
    m_parameters->setColumnCount(2);
    m_parameters->setHorizontalHeaderLabels({tr("Parameter"), tr("Value")});
    m_parameters->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_parameters->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_parameters->verticalHeader()->hide();
    m_parameters->setAlternatingRowColors(true);
    form->addRow(m_description);
    form->addRow(tr("Parameters"), m_parameters);
    form->addRow(tr("FFmpeg expression"), m_expression);
    splitter->addWidget(m_list); splitter->addWidget(editor);
    splitter->setStretchFactor(1, 1);
    root->addWidget(splitter, 1);

    auto *buttons = new QHBoxLayout;
    m_remove = new QPushButton(tr("Remove"), this);
    m_up = new QPushButton(tr("Move up"), this);
    m_down = new QPushButton(tr("Move down"), this);
    m_preview = new QPushButton(tr("Preview stack…"), this);
    buttons->addWidget(m_remove); buttons->addWidget(m_up); buttons->addWidget(m_down);
    buttons->addStretch(); buttons->addWidget(m_preview);
    root->addLayout(buttons);

    connect(add, &QPushButton::clicked, this, &QEffectStack::addFilter);
    connect(m_remove, &QPushButton::clicked, this, &QEffectStack::removeFilter);
    connect(m_up, &QPushButton::clicked, this, &QEffectStack::moveUp);
    connect(m_down, &QPushButton::clicked, this, &QEffectStack::moveDown);
    connect(m_list, &QListWidget::currentRowChanged, this, &QEffectStack::selectionChanged);
    connect(m_list, &QListWidget::itemChanged, this, &QEffectStack::itemChanged);
    connect(m_expression, &QLineEdit::editingFinished, this, &QEffectStack::expressionEdited);
    connect(m_parameters, &QTableWidget::cellChanged, this, &QEffectStack::parameterEdited);
    connect(m_preview, &QPushButton::clicked, this, &QEffectStack::previewRequested);
    rebuild();
}

QStringList QEffectStack::enabledFilters(const QStringList &filters)
{
    QStringList result;
    for (const QString &filter : filters)
        if (!filter.startsWith("#disabled:") && !filter.trimmed().isEmpty()) result << filter;
    return result;
}

void QEffectStack::setFilters(QStringList *filters) { m_filters = filters; rebuild(); }
void QEffectStack::setPreviewButtonVisible(bool visible) { m_preview->setVisible(visible); }

void QEffectStack::rebuild()
{
    m_updating = true; m_list->clear();
    if (m_filters) for (const QString &filter : *m_filters) {
        auto *item = new QListWidgetItem(displayName(filter), m_list);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(filter.startsWith("#disabled:") ? Qt::Unchecked : Qt::Checked);
        item->setToolTip(filter.startsWith("#disabled:") ? filter.mid(10) : filter);
    }
    m_updating = false;
    if (m_list->count()) m_list->setCurrentRow(0);
    selectionChanged();
}

void QEffectStack::syncModel()
{
    if (!m_filters) return;
    m_filters->clear();
    for (int i = 0; i < m_list->count(); ++i) {
        auto *item = m_list->item(i);
        QString expression = item->data(Qt::UserRole).toString();
        if (expression.isEmpty()) expression = item->toolTip();
        if (item->checkState() == Qt::Unchecked) expression.prepend("#disabled:");
        m_filters->append(expression);
    }
    emit filtersChanged();
}

void QEffectStack::addFilter()
{
    if (!m_filters) return;
    QString expression = m_catalog->currentData().toString();
    if (expression.isEmpty()) expression = "null";
    m_filters->append(expression); rebuild(); m_list->setCurrentRow(m_list->count() - 1); emit filtersChanged();
}

void QEffectStack::removeFilter()
{
    const int row = m_list->currentRow(); if (row < 0) return;
    delete m_list->takeItem(row); syncModel(); selectionChanged();
}

void QEffectStack::moveUp()
{
    const int row = m_list->currentRow(); if (row <= 0) return;
    auto *item = m_list->takeItem(row); m_list->insertItem(row - 1, item); m_list->setCurrentRow(row - 1); syncModel();
}

void QEffectStack::moveDown()
{
    const int row = m_list->currentRow(); if (row < 0 || row + 1 >= m_list->count()) return;
    auto *item = m_list->takeItem(row); m_list->insertItem(row + 1, item); m_list->setCurrentRow(row + 1); syncModel();
}

void QEffectStack::selectionChanged()
{
    const int row = m_list->currentRow(); const bool selected = row >= 0 && m_filters && row < m_filters->size();
    m_remove->setEnabled(selected); m_up->setEnabled(selected && row > 0);
    m_down->setEnabled(selected && row + 1 < m_list->count()); m_expression->setEnabled(selected);
    m_updating = true;
    m_parameters->setRowCount(0);
    if (!selected) { m_expression->clear(); m_description->setText(tr("Add a filter to build an ordered video-effects stack.")); m_updating = false; return; }
    QString expression = m_filters->at(row); if (expression.startsWith("#disabled:")) expression.remove(0, 10);
    m_expression->setText(expression);
    m_description->setText(m_list->item(row)->toolTip());
    const int equals = expression.indexOf('=');
    if (equals >= 0) {
        const QStringList arguments = expression.mid(equals + 1).split(':');
        m_parameters->setRowCount(arguments.size());
        for (int i = 0; i < arguments.size(); ++i) {
            const int optionEquals = arguments[i].indexOf('=');
            const bool named = optionEquals > 0;
            const QString name = named ? arguments[i].left(optionEquals) : tr("Argument %1").arg(i + 1);
            const QString value = named ? arguments[i].mid(optionEquals + 1) : arguments[i];
            auto *nameItem = new QTableWidgetItem(name);
            nameItem->setData(Qt::UserRole, named);
            auto *valueItem = new QTableWidgetItem(value);
            m_parameters->setItem(i, 0, nameItem);
            m_parameters->setItem(i, 1, valueItem);
        }
    }
    m_updating = false;
}

void QEffectStack::expressionEdited()
{
    const int row = m_list->currentRow(); if (!m_filters || row < 0) return;
    QString value = m_expression->text().trimmed();
    auto *item = m_list->item(row); item->setData(Qt::UserRole, value); item->setText(displayName(value)); item->setToolTip(value);
    syncModel(); selectionChanged();
}

void QEffectStack::parameterEdited()
{
    if (m_updating || m_list->currentRow() < 0) return;
    const QString current = m_expression->text();
    const QString filterName = current.section('=', 0, 0);
    QStringList arguments;
    for (int row = 0; row < m_parameters->rowCount(); ++row) {
        const auto *nameItem = m_parameters->item(row, 0);
        const auto *valueItem = m_parameters->item(row, 1);
        if (!nameItem || !valueItem) continue;
        const QString value = valueItem->text().trimmed();
        if (nameItem->data(Qt::UserRole).toBool())
            arguments << nameItem->text().trimmed() + '=' + value;
        else
            arguments << value;
    }
    m_expression->setText(filterName + '=' + arguments.join(':'));
    expressionEdited();
}

void QEffectStack::itemChanged() { if (!m_updating) syncModel(); }
