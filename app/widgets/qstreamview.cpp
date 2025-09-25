#include "qstreamview.h"
#include "helper.h"
#include <QMouseEvent>
#include <QMenu>
#include <QPropertyAnimation>
#include <QStandardItemModel>
#include <QHeaderView>
#include <QEvent>
#include <QStyle>
#include <QLayout>
#include <QAction>
#include <QLineEdit>
#include <QCheckBox>
#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <QIcon>
#include <QFile>
#include <iostream>

#define ROW_HEIGHT 22

namespace QStreamViewPrivate {
    QLabel *createLabel(QWidget *parent, const char *name, const QString &text)
    {
        auto *label = new QLabel(parent);
        label->setObjectName(QString::fromUtf8(name));
        label->setText(text);
        label->setAutoFillBackground(false);
        label->setFrameShadow(QFrame::Plain);
        return label;
    }

    QLineEdit *createLine(QWidget *parent, const char *name, QString &text)
    {
        auto *line = new QLineEdit(parent);
        line->setObjectName(QString::fromUtf8(name));
        line->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        line->setEnabled(true);
        line->setText(text);
        line->setCursorPosition(0);
        line->setFixedHeight(ROW_HEIGHT * Helper::scaling());
        QObject::connect(line, &QLineEdit::editingFinished, [line, &text]() {
            if (line->isModified()) {
                line->setModified(false);
                text = line->text();
            }
        });
        return line;
    }

    QRadioButton *createRadio(QWidget *parent, const char *name, const QString &text, bool checked)
    {
        auto *btn = new QRadioButton(parent);
        btn->setObjectName(QString::fromUtf8(name));
        btn->setAutoExclusive(false);
        if (!text.isEmpty())
            btn->setText(text);
        btn->setChecked(checked);
        return btn;
    }

    void onRowHovered(QObject *obj, bool flag)
    {
        auto *wgt = dynamic_cast<QWidget*>(obj);
        if (wgt) {
            wgt->setProperty("hover", flag);
            wgt->style()->polish(wgt);
        }
    }

    void onRowResize(QWidget *wgt, int start, int end)
    {
        auto *animation = new QPropertyAnimation(wgt, "minimumHeight");
        animation->setDuration(200);
        animation->setStartValue(start);
        animation->setEndValue(end);
        animation->start(QPropertyAnimation::DeleteWhenStopped);
    }
}

QStreamView::QStreamView(QWidget *parent) :
    QWidget(parent),
    m_pData(nullptr),
    m_targetAudioCodec(QString()),
    m_usePresetSubtitleSettings(false)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    m_pLayout = new QVBoxLayout(this);
    m_pLayout->setContentsMargins(0,0,0,0);
    m_pLayout->setSpacing(1 * Helper::scaling());
    m_pLayout->setSizeConstraint(QLayout::SetMinimumSize);
    setLayout(m_pLayout);
}

QStreamView::~QStreamView()
= default;

void QStreamView::setContentType(Content type)
{
    m_type = type;
}

void QStreamView::clearList()
{
    m_pData = nullptr;
    QLayoutItem *item;
    while ((item = m_pLayout->takeAt(0)) != nullptr) {
        if (item->widget()) {
            item->widget()->deleteLater();
        }
        delete item;
    }
}

void QStreamView::setList(QString extension, Data &data, const QString& targetAudioCodec, bool usePresetSubtitleSettings)
{
    // Early return if data is being initialized - check if basic data structures are ready
    if (data.videoMetadata.isEmpty()) {
        // Data is still being initialized, clear the list and return early
        clearList();
        m_pData = &data;
        return;
    }

    // Save current audio selection state before clearing
    QVector<bool> savedAudioChecks;
    QVector<bool> savedExternAudioChecks;
    QVector<bool> savedAudioDef;
    QVector<bool> savedExternAudioDef;
    // Save current subtitle selection state before clearing
    QVector<bool> savedSubtChecks;
    QVector<bool> savedExternSubtChecks;
    QVector<bool> savedSubtDef;
    QVector<bool> savedExternSubtDef;
    QVector<bool> savedSubtBurn;
    QVector<bool> savedExternSubtBurn;
    
    if (m_pData != nullptr) {
        // Save states only if the previous data was fully initialized
        if (!m_pData->videoMetadata.isEmpty()) {
            // Save audio states - use try-catch to handle potential atomic operation failures
            try {
                if (Data::audioChecks < Data::CHECKS_COUNT && !m_pData->checks[Data::audioChecks].isEmpty()) {
                    savedAudioChecks = m_pData->checks[Data::audioChecks];
                }
                if (Data::externAudioChecks < Data::CHECKS_COUNT && !m_pData->checks[Data::externAudioChecks].isEmpty()) {
                    savedExternAudioChecks = m_pData->checks[Data::externAudioChecks];
                }
                if (Data::audioDef < Data::CHECKS_COUNT && !m_pData->checks[Data::audioDef].isEmpty()) {
                    savedAudioDef = m_pData->checks[Data::audioDef];
                }
                if (Data::externAudioDef < Data::CHECKS_COUNT && !m_pData->checks[Data::externAudioDef].isEmpty()) {
                    savedExternAudioDef = m_pData->checks[Data::externAudioDef];
                }
                // Save subtitle states
                if (Data::subtChecks < Data::CHECKS_COUNT && !m_pData->checks[Data::subtChecks].isEmpty()) {
                    savedSubtChecks = m_pData->checks[Data::subtChecks];
                }
                if (Data::externSubtChecks < Data::CHECKS_COUNT && !m_pData->checks[Data::externSubtChecks].isEmpty()) {
                    savedExternSubtChecks = m_pData->checks[Data::externSubtChecks];
                }
                if (Data::subtDef < Data::CHECKS_COUNT && !m_pData->checks[Data::subtDef].isEmpty()) {
                    savedSubtDef = m_pData->checks[Data::subtDef];
                }
                if (Data::externSubtDef < Data::CHECKS_COUNT && !m_pData->checks[Data::externSubtDef].isEmpty()) {
                    savedExternSubtDef = m_pData->checks[Data::externSubtDef];
                }
                if (Data::subtBurn < Data::CHECKS_COUNT && !m_pData->checks[Data::subtBurn].isEmpty()) {
                    savedSubtBurn = m_pData->checks[Data::subtBurn];
                }
                if (Data::externSubtBurn < Data::CHECKS_COUNT && !m_pData->checks[Data::externSubtBurn].isEmpty()) {
                    savedExternSubtBurn = m_pData->checks[Data::externSubtBurn];
                }
            } catch (...) {
                // If any exception occurs during vector copying, continue with empty saved vectors
                // This handles race conditions during data initialization
            }
        }
    }
    
    clearList();
    
    // Only restore saved selection state if we're refreshing the same file's data
    // This preserves user choices within the same file while preventing cross-file pollution
    bool isSameFile = (m_pData == &data);
    m_pData = &data;
    
    if (isSameFile) {
        // Same file being refreshed - restore user selections
        if (!savedAudioChecks.isEmpty() && savedAudioChecks.size() == data.checks[Data::audioChecks].size()) {
            data.checks[Data::audioChecks] = savedAudioChecks;
        }
        if (!savedExternAudioChecks.isEmpty() && savedExternAudioChecks.size() == data.checks[Data::externAudioChecks].size()) {
            data.checks[Data::externAudioChecks] = savedExternAudioChecks;
        }
        if (!savedAudioDef.isEmpty() && savedAudioDef.size() == data.checks[Data::audioDef].size()) {
            data.checks[Data::audioDef] = savedAudioDef;
        }
        if (!savedExternAudioDef.isEmpty() && savedExternAudioDef.size() == data.checks[Data::externAudioDef].size()) {
            data.checks[Data::externAudioDef] = savedExternAudioDef;
        }
        // Also restore subtitle selections for the same file
        if (!savedSubtChecks.isEmpty() && savedSubtChecks.size() == data.checks[Data::subtChecks].size()) {
            data.checks[Data::subtChecks] = savedSubtChecks;
        }
        if (!savedExternSubtChecks.isEmpty() && savedExternSubtChecks.size() == data.checks[Data::externSubtChecks].size()) {
            data.checks[Data::externSubtChecks] = savedExternSubtChecks;
        }
        if (!savedSubtDef.isEmpty() && savedSubtDef.size() == data.checks[Data::subtDef].size()) {
            data.checks[Data::subtDef] = savedSubtDef;
        }
        if (!savedExternSubtDef.isEmpty() && savedExternSubtDef.size() == data.checks[Data::externSubtDef].size()) {
            data.checks[Data::externSubtDef] = savedExternSubtDef;
        }
        if (!savedSubtBurn.isEmpty() && savedSubtBurn.size() == data.checks[Data::subtBurn].size()) {
            data.checks[Data::subtBurn] = savedSubtBurn;
        }
        if (!savedExternSubtBurn.isEmpty() && savedExternSubtBurn.size() == data.checks[Data::externSubtBurn].size()) {
            data.checks[Data::externSubtBurn] = savedExternSubtBurn;
        }
    }
    // Note: For different files, we preserve each file's own defaults as set by Helper::isAudioSupported() and Helper::isSubtitleSupported()
    m_targetAudioCodec = targetAudioCodec;
    m_usePresetSubtitleSettings = usePresetSubtitleSettings;
    const QString columns[] = {
        tr("Format"), tr("Title"), tr("Language")
    };
    auto *model = new QStandardItemModel(this);
    for (int i = 0; i < 3; i++) {
        auto *__item = new QStandardItem(columns[i]);
        model->setHorizontalHeaderItem(i, __item);
    }
    auto *hw = new QHeaderView(Qt::Horizontal, this);
    QFont fnt = hw->font();
    fnt.setItalic(true);
    fnt.setBold(true);
    hw->setFont(fnt);
    hw->setDefaultSectionSize(132 * Helper::scaling());
    hw->setFixedHeight(28 * Helper::scaling());
    hw->setModel(model);
    hw->setSectionResizeMode(0, QHeaderView::Fixed);
    hw->setSectionResizeMode(1, QHeaderView::Stretch);
    hw->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_pLayout->addWidget(hw);

    if (m_type == Content::Audio) {
        bool stub = false;
        for (int i = 0; i < data.fields[Data::audioFormats].size(); i++) {
            QWidget *cell = createCell(data.checks[Data::audioChecks][i],
                                       extension,
                                       data.fields[Data::audioFormats][i],
                                    data.fields[Data::audioDuration][i],
                                    data.fields[Data::audioLangs][i],
                                    data.fields[Data::audioTitles][i],
                                    data.fields[Data::audioChannels][i],
                                    data.fields[Data::audioChLayouts][i],
                                       "",
            data.checks[Data::audioDef][i],
                                       stub);
            m_pLayout->addWidget(cell);
        }
        for (int i = 0; i < data.fields[Data::externAudioFormats].size(); i++) {
            QWidget *cell = createCell(data.checks[Data::externAudioChecks][i],
                                       extension,
                                       data.fields[Data::externAudioFormats][i],
                                       data.fields[Data::externAudioDuration][i],
                                       data.fields[Data::externAudioLangs][i],
                                       data.fields[Data::externAudioTitles][i],
                                       data.fields[Data::externAudioChannels][i],
                                       data.fields[Data::externAudioChLayouts][i],
                                       data.fields[Data::externAudioPath][i],
                                       data.checks[Data::externAudioDef][i],
                                       stub,
                                       true);
            m_pLayout->addWidget(cell);
        }
    } else
    if (m_type == Content::Subtitle) {
        for (int i = 0; i < data.fields[Data::subtFormats].size(); i++) {
            QWidget *cell = createCell(data.checks[Data::subtChecks][i],
                                       extension,
                                       data.fields[Data::subtFormats][i],
                                       data.fields[Data::subtDuration][i],
                                       data.fields[Data::subtLangs][i],
                                       data.fields[Data::subtTitles][i],
                                       "",
                                       "",
                                       "",
                                       data.checks[Data::subtDef][i],
                                       data.checks[Data::subtBurn][i]);
            m_pLayout->addWidget(cell);
        }
        for (int i = 0; i < data.fields[Data::externSubtFormats].size(); i++) {
            QWidget *cell = createCell(data.checks[Data::externSubtChecks][i],
                                       extension,
                                       data.fields[Data::externSubtFormats][i],
                                       data.fields[Data::externSubtDuration][i],
                                       data.fields[Data::externSubtLangs][i],
                                       data.fields[Data::externSubtTitles][i],
                                       "",
                                       "",
                                       data.fields[Data::externSubtPath][i],
                                       data.checks[Data::externSubtDef][i],
                                       data.checks[Data::externSubtBurn][i],
                                       true);
            m_pLayout->addWidget(cell);
        }
    }
}

void QStreamView::deselectTitles()
{
    auto lines = findChildren<QCheckBox*>("checkStream");
    foreach (auto line, lines) {
        line->setChecked(false);
    }
    auto lines2 = findChildren<QRadioButton*>("burnInto");
    foreach (auto line, lines2) {
        line->setChecked(false);
    }
    auto lines3 = findChildren<QRadioButton*>("defaultStream");
    foreach (auto line, lines3) {
        line->setChecked(false);
    }
    if (m_type == Content::Audio) {
        if (m_pData != nullptr) {
            m_pData->checks[Data::audioChecks].fill(false);
            m_pData->checks[Data::externAudioChecks].fill(false);
            m_pData->checks[Data::audioDef].fill(false);
        }
    } else
    if (m_type == Content::Subtitle) {
        if (m_pData != nullptr) {
            m_pData->checks[Data::subtChecks].fill(false);
            m_pData->checks[Data::externSubtChecks].fill(false);
            m_pData->checks[Data::subtDef].fill(false);
            m_pData->checks[Data::externSubtDef].fill(false);
            m_pData->checks[Data::externSubtBurn].fill(false);
        }
    }
    setFocus();
}

void QStreamView::clearTitles()
{
    auto lines = findChildren<QLineEdit*>("lineTitle");
    foreach (auto line, lines) {
        line->clear();
        line->insert("");
        line->setFocus();
        line->setModified(true);
    }
    setFocus();
}

void QStreamView::undoTitles()
{
    auto lines = findChildren<QLineEdit*>("lineTitle");
    foreach (auto line, lines) {
        line->undo();
        if (line->text() != "") {
            line->setFocus();
            line->setCursorPosition(0);
            line->setModified(true);
        }
    }
    setFocus();
}

bool QStreamView::eventFilter(QObject *obj, QEvent *event)
{
    switch (event->type()) {
    case QEvent::HoverEnter:
    case QEvent::HoverLeave: {
        // For hover events, find the parent cell if the event came from a child widget
        QWidget *cell = qobject_cast<QWidget*>(obj);
        if (!cell || cell->objectName() != "Cell") {
            QWidget *parent = qobject_cast<QWidget*>(obj);
            while (parent && parent->objectName() != "Cell") {
                parent = parent->parentWidget();
            }
            if (parent) {
                // Forward hover event to parent cell
                QStreamViewPrivate::onRowHovered(parent, event->type() == QEvent::HoverEnter);
                break;
            }
        }
        QStreamViewPrivate::onRowHovered(obj, event->type() == QEvent::HoverEnter);
        break;
    }
    case QEvent::MouseButtonDblClick: {
        auto* mouse_event = dynamic_cast<QMouseEvent*>(event);
        if (mouse_event->buttons() & Qt::LeftButton) {
            QWidget *cell = qobject_cast<QWidget*>(obj);
            
            // If the event came from a child widget, find the parent cell
            if (!cell || cell->objectName() != "Cell") {
                QWidget *parent = qobject_cast<QWidget*>(obj);
                while (parent && parent->objectName() != "Cell") {
                    parent = parent->parentWidget();
                }
                cell = parent;
            }
            
            if (!cell) return QWidget::eventFilter(obj, event);
            
            auto *btn = cell->findChild<QPushButton*>("expandBtn");
            if (btn)
                btn->click();
        }
        break;
    }
    case QEvent::MouseButtonPress: {
        auto* mouse_event = dynamic_cast<QMouseEvent*>(event);
        if (mouse_event->buttons() & Qt::RightButton) {
            // Prevent context menu during encoding when widget is disabled
            if (!this->isEnabled()) {
                return QWidget::eventFilter(obj, event);
            }
            
            QWidget *cell = qobject_cast<QWidget*>(obj);
            
            // If the event came from a child widget (like burn radio button), find the parent cell
            if (!cell || cell->objectName() != "Cell") {
                QWidget *parent = qobject_cast<QWidget*>(obj);
                while (parent && parent->objectName() != "Cell") {
                    parent = parent->parentWidget();
                }
                cell = parent;
            }
            
            if (!cell) return QWidget::eventFilter(obj, event);
            
            auto *btn = cell->findChild<QPushButton*>("expandBtn");
            bool expanded = false;
            if (btn)
                expanded = btn->property("expanded").toBool();
            auto *chkBox = cell->findChild<QCheckBox*>("checkStream");
            bool checked = false;
            if (chkBox)
                checked = (chkBox->checkState() == 2);
            auto *rbtn = cell->findChild<QRadioButton*>("defaultStream");
            bool deflt = false;
            if (rbtn)
                deflt = rbtn->isChecked();

            auto *streamMenu = new QMenu(cell);
            auto *pActExpand = new QAction(expanded ? tr("Collapse") : tr("Expand"), streamMenu);
            auto *pActCheck = new QAction(checked ? tr("Uncheck") : tr("Check"), streamMenu);
            auto *pActSetDef = new QAction(tr("Set as default track"), streamMenu);
            auto *pActDeselectAll = new QAction(tr("Deselect all"), streamMenu);
            connect(pActExpand, &QAction::triggered, this, [btn]() {
                if (btn)
                    btn->click();
            });
            connect(pActCheck, &QAction::triggered, this, [chkBox]() {
                if (chkBox)
                    chkBox->click();
            });
            connect(pActSetDef, &QAction::triggered, this, [rbtn, deflt]() {
                if (rbtn && !deflt)
                    rbtn->click();
            });
            connect(pActDeselectAll, &QAction::triggered, this, [this]() {
                deselectTitles();
            });
            QAction *pActExtract = nullptr;
            const bool external = cell->property("External").toBool();
            const int num = cell->property("Number").toInt();
            if (!external) {
                pActExtract = new QAction(tr("Extract track"), streamMenu);
                connect(pActExtract, &QAction::triggered, this, [this, num]() {
                    emit onExtractTrack(m_type, num);
                });
            }
            streamMenu->addAction(pActExpand);
            streamMenu->addSeparator();
            streamMenu->addAction(pActCheck);
            streamMenu->addAction(pActDeselectAll);
            streamMenu->addAction(pActSetDef);
            if (!external) {
                streamMenu->addSeparator();
                streamMenu->addAction(pActExtract);
            }
            const QPoint globPos = cell->mapToGlobal(mouse_event->pos() + QPoint(0, 10));
            streamMenu->exec(globPos);
            streamMenu->deleteLater();
            QStreamViewPrivate::onRowHovered(obj, false);
        }
        break;
    }
    default:
        break;
    }
    return QWidget::eventFilter(obj, event);
}

void QStreamView::resetCheckFlags(const int ind)
{
    for (int i = 1; i < m_pLayout->count(); i++) {
        if (i != ind) {
            QLayoutItem *item = m_pLayout->itemAt(i);
            if (item && item->widget()) {
                auto *chkBox = item->widget()->findChild<QCheckBox*>("checkStream");
                if (chkBox)
                    chkBox->setChecked(false);
            }
        }
    }

    if (m_type == Content::Audio) {
        m_pData->checks[Data::audioChecks].fill(false);
        m_pData->checks[Data::externAudioChecks].fill(false);
    } else
    if (m_type == Content::Subtitle) {
        m_pData->checks[Data::subtChecks].fill(false);
        m_pData->checks[Data::externSubtChecks].fill(false);
    }
}

void QStreamView::resetDefFlags(const int ind)
{
    for (int i = 1; i < m_pLayout->count(); i++) {
        if (i != ind) {
            QLayoutItem *item = m_pLayout->itemAt(i);
            if (item && item->widget()) {
                auto *rbtn = item->widget()->findChild<QRadioButton*>("defaultStream", Qt::FindDirectChildrenOnly);
                if (rbtn)
                    rbtn->setChecked(false);
            }
        }
    }

    if (m_type == Content::Audio) {
        m_pData->checks[Data::audioDef].fill(false);
        m_pData->checks[Data::externAudioDef].fill(false);
    } else
    if (m_type == Content::Subtitle) {
        m_pData->checks[Data::subtDef].fill(false);
        m_pData->checks[Data::externSubtDef].fill(false);
    }
}

void QStreamView::resetBurnFlags(const int ind)
{
    for (int i = 1; i < m_pLayout->count(); i++) {
        if (i != ind) {
            QLayoutItem *item = m_pLayout->itemAt(i);
            if (item && item->widget()) {
                auto *rbtn = item->widget()->findChild<QRadioButton*>("burnInto");
                if (rbtn)
                    rbtn->setChecked(false);
            }
        }
    }

    m_pData->checks[Data::subtBurn].fill(false);
    m_pData->checks[Data::externSubtBurn].fill(false);
}

QWidget *QStreamView::createCell(bool &state,
                                 QString &extension,
                                 const QString &format,
                                 const QString &dur,
                                 QString &lang,
                                 QString &title,
                                 const QString &channels,
                                 QString chLayouts,
                                 const QString &path,
                                 bool &deflt,
                                 bool &burn,
                                 bool externFlag
                                 )
{
    auto connectAction = [this](QLineEdit* line, bool isVisible)->void {
        auto actionList = line->findChildren<QAction*>();
        if (!actionList.isEmpty()) {
            connect(actionList.first(), &QAction::triggered, this, [this, line]() {
                line->clear();
                line->insert("");
                line->setModified(true);
                setFocus();
            });
        }
        if (!isVisible) {
            line->setEnabled(true);
            line->setVisible(false);
        }
    };

    auto *cell = new QWidget(this);
    cell->setAttribute(Qt::WA_Hover);
    cell->installEventFilter(this);
    cell->setObjectName("Cell");
    cell->setProperty("Number", m_pLayout->count() - 1);
    cell->setProperty("External", externFlag);
    cell->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    cell->setMinimumHeight(46 * Helper::scaling());
    auto *lut = new QGridLayout(cell);
    lut->setContentsMargins(6,2,6,4);
    lut->setHorizontalSpacing(6 * Helper::scaling());
    lut->setVerticalSpacing(4 * Helper::scaling());
    cell->setLayout(lut);

    // Radio button 'Default stream'
    QRadioButton *rbtn = QStreamViewPrivate::createRadio(cell, "defaultStream", "", deflt);
    rbtn->setFixedSize(QSize(12,12) * Helper::scaling());
    rbtn->setToolTip(tr("Default"));
    // Install event filter on default radio button to forward context menu events to parent cell
    rbtn->installEventFilter(this);
    connect(rbtn, &QRadioButton::clicked, this, [this, cell, &burn, &deflt, &state](bool checked) {
        resetBurnFlags(m_pLayout->indexOf(cell));
        resetDefFlags(m_pLayout->indexOf(cell));
        deflt = checked;
        QLayoutItem *item = m_pLayout->itemAt(m_pLayout->indexOf(cell));
        if (item && item->widget()) {
            if (deflt) {
                auto *chkBox = item->widget()->findChild<QCheckBox*>("checkStream");
                if (chkBox && !chkBox->isChecked()) {
                    chkBox->setChecked(true);
                    state = true;
                }
            } else {
                auto *brn_rbtn = item->widget()->findChild<QRadioButton*>("burnInto");
                if (brn_rbtn && brn_rbtn->isChecked()) {
                    brn_rbtn->setChecked(false);
                    burn = false;
                }
            }
        }
    });
    lut->addWidget(rbtn, 0, 0, Qt::AlignLeft);

    // External Audio Label
    QFont fnt;
    fnt.setPointSize(8);
    QLabel *tit = QStreamViewPrivate::createLabel(cell, "extAudioLabel", "");
    tit->setFont(fnt);
    tit->setEnabled(false);
    tit->setMinimumSize(QSize(0, 12) * Helper::scaling());
    tit->setMaximumSize(QSize(150, 12) * Helper::scaling());
    lut->addWidget(tit, 0, 1, 1, 2, Qt::AlignLeft);
    if (externFlag)
        tit->setText(tr("external") + " ");

    auto *info = new QWidget(cell);
    info->setObjectName("infoWidget");
    info->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    info->hide();
    lut->addWidget(info, 2, 0, 3, 0);
    auto *infoLut = new QGridLayout(info);
    infoLut->setContentsMargins(6,6,6,6);
    infoLut->setHorizontalSpacing(6 * Helper::scaling());
    infoLut->setVerticalSpacing(2 * Helper::scaling());
    info->setLayout(infoLut);

    double duration_double = 0.001 * dur.toDouble();
    QString durationTime = Helper::timeConverter(static_cast<float>(duration_double));
    QLabel *labDuration = QStreamViewPrivate::createLabel(info, "labelDuration",
                                                    QString("%1: %2").arg(tr("Duration"),
                                                                          durationTime));
    labDuration->setEnabled(true);
    labDuration->setFixedHeight(ROW_HEIGHT * Helper::scaling());
    infoLut->addWidget(labDuration, 0, 0);

    bool burn_only = false;
    bool isIncompatible = false; // Track incompatibility for visual styling
    
    // Label channels
    if (m_type == Content::Audio) {
        if (Helper::isAudioIncompatible(extension, format, m_targetAudioCodec)) {
            tit->setText(tit->text() + tr("unsupported"));
            isIncompatible = true;
        }
        if (chLayouts.isEmpty())
            chLayouts = tr("No layouts");

        QLabel *labChLayouts = QStreamViewPrivate::createLabel(info, "labelChLayouts",
                                                        QString("%1: %2").arg(tr("Layouts"), chLayouts));
        labChLayouts->setEnabled(true);
        labChLayouts->setFixedHeight(ROW_HEIGHT);
        infoLut->addWidget(labChLayouts, 1, 1);

        QLabel *labCh = QStreamViewPrivate::createLabel(info, "labelChannels",
                                                        QString("%1: %2")
                                                        .arg(tr("Channels"),
                                                             Helper::recalcChannels(channels)));
        labCh->setEnabled(true);
        labCh->setFixedHeight(ROW_HEIGHT * Helper::scaling());
        infoLut->addWidget(labCh, 0, 1);
    } else
    if (m_type == Content::Subtitle) {
        if (Helper::isSubtitleIncompatible(extension, format, m_usePresetSubtitleSettings)) {
            tit->setText(tit->text() + tr("Hard-burn only"));
            burn_only = true;
            state = false;
            isIncompatible = true;
        }
        QRadioButton *brn_rbtn = QStreamViewPrivate::createRadio(info, "burnInto", tr("Burn into video"), burn);
        brn_rbtn->setFixedHeight(12 * Helper::scaling());
        brn_rbtn->setToolTip(tr("Burn into video"));
        // Stream can only be burnt for target.
        if (burn_only) {
            brn_rbtn->setChecked(true);
        }
        // Install event filter on burn radio button to forward context menu events to parent cell
        brn_rbtn->installEventFilter(this);
        connect(brn_rbtn, &QRadioButton::clicked, this, [this, cell, &burn, &deflt, &state, &burn_only](bool checked) {
            resetBurnFlags(m_pLayout->indexOf(cell));
            resetDefFlags(m_pLayout->indexOf(cell));
            resetCheckFlags(m_pLayout->indexOf(cell));
            burn = checked || burn_only;
            if (burn) {
                QLayoutItem *item = m_pLayout->itemAt(m_pLayout->indexOf(cell));
                if (item && item->widget()) {
                    // Cannot copy stream if burn is set.
                    auto *chkBox = item->widget()->findChild<QCheckBox*>("checkStream");
                    if (chkBox && chkBox->isChecked()) {
                        chkBox->setChecked(false);
                        state = false;
                    }
                    auto *rbtn = item->widget()->findChild<QRadioButton *>("defaultStream",
                                                                                   Qt::FindDirectChildrenOnly);
                    // Don't force this here - the default button is the way that the hard-burn is enabled.
                    if (!burn_only) {
                        if (rbtn && !rbtn->isChecked()) {
                            rbtn->setChecked(true);
                            deflt = true;
                        }
                    }
                }
            }
        });
        infoLut->addWidget(brn_rbtn, 0, 1, Qt::AlignLeft);
    }

    // Label external path
    if (externFlag) {
        const QString elidedPath = Helper::elideText(this, path, Qt::ElideMiddle);
        QLabel *labPath = QStreamViewPrivate::createLabel(info, "labelPath",
                                                          QString("%1: %2").arg(tr("Path"), elidedPath));
        labPath->setEnabled(true);
        labPath->setFixedHeight(ROW_HEIGHT * Helper::scaling());
        infoLut->addWidget(labPath, 2, 0, 1, 2);
    }

    auto *sp_bottom = new QSpacerItem(5, 5, QSizePolicy::Fixed, QSizePolicy::Expanding);
    infoLut->addItem(sp_bottom, 5, 0);

    // Expand button
    auto *btn = new QPushButton(cell);
    btn->setObjectName(QString::fromUtf8("expandBtn"));
    btn->setFixedSize(QSize(12, 12) * Helper::scaling());
    connect(btn, &QPushButton::clicked, this, [cell, btn, info]() {
        if (cell->minimumHeight() == 46 * Helper::scaling()) {
            QStreamViewPrivate::onRowResize(cell, 46 * Helper::scaling(), 120 * Helper::scaling());
            btn->setProperty("expanded", true);
            btn->style()->polish(btn);
            info->show();
        } else {
            QStreamViewPrivate::onRowResize(cell, 120 * Helper::scaling(), 46 * Helper::scaling());
            btn->setProperty("expanded", false);
            btn->style()->polish(btn);
            info->hide();
        }
    });
    lut->addWidget(btn, 0, 3, Qt::AlignRight);

    // Number
    QLabel *num = QStreamViewPrivate::createLabel(cell, "numAudioLabel",
                                                  QString::number(m_pLayout->count()) + ".");
    num->setFont(fnt);
    num->setEnabled(false);
    num->setMinimumWidth(14 * Helper::scaling());
    lut->addWidget(num, 1, 0, Qt::AlignLeft);

    // Check
    auto *chkBox = new QCheckBox(cell);
    chkBox->setObjectName(QString::fromUtf8("checkStream"));
    chkBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    chkBox->setFixedWidth(100 * Helper::scaling());
    chkBox->setText(format);
    // Install event filter on checkbox to forward context menu events to parent cell
    chkBox->installEventFilter(this);
    if (burn_only && (m_type == Content::Subtitle)) {
        chkBox->setEnabled(false);
        chkBox->setChecked(false);
    }
    else
    {
        chkBox->setEnabled(true);
        chkBox->setChecked(state);
    }
    // Burn is whether the user selected to burn; burn_only is when only burning is an option.
    // Default marks the default stream, which triggers burn. A stream cannot be burnt if it is not default.
    connect(chkBox, &QCheckBox::clicked, this, [this, cell, chkBox, &burn, &burn_only, &state, &deflt, isIncompatible](){
        state = (chkBox->checkState() == 2);
        // Burn-only prohibits the copy of subtitle streams (target format cannot support the stream).
        if (burn_only)
        {
            state = false;
        }
        if (!state) {
            QLayoutItem *item = m_pLayout->itemAt(m_pLayout->indexOf(cell));
            if (item && item->widget()) {
                auto *rbtn = item->widget()->findChild<QRadioButton*>("defaultStream", Qt::FindDirectChildrenOnly);
                if (rbtn && rbtn->isChecked()) {
                    rbtn->setChecked(false);
                    deflt = false;
                }
                auto *brn_rbtn = item->widget()->findChild<QRadioButton*>("burnInto");
                if (brn_rbtn /* && brn_rbtn->isChecked()*/ ) {
                    burn = false;
                    if (burn_only)
                    {
                        burn = true;
                    }
                    brn_rbtn->setChecked(burn);
                }
            }
        }
        
        // Update incompatible stream styling based on new selection state
        updateIncompatibleStreamStyling(cell, chkBox, isIncompatible, state);
        
        // Emit signal to notify about stream selection change
        emit streamSelectionChanged();
    });
    lut->addWidget(chkBox, 1, 1);

    // Title
    QLineEdit *line_1 = QStreamViewPrivate::createLine(cell, "lineTitle", title);
    line_1->setClearButtonEnabled(true);
    connectAction(line_1, true);
    lut->addWidget(line_1, 1, 2);

    // Lang
    QLineEdit *line = QStreamViewPrivate::createLine(cell, "lineLang", lang);
    line->setMaximumWidth(30 * Helper::scaling());
    connectAction(line, true);
    lut->addWidget(line, 1, 3);

    // Initialize incompatible stream styling
    updateIncompatibleStreamStyling(cell, chkBox, isIncompatible, state);

    return cell;
}

void QStreamView::updateIncompatibleStreamStyling(QWidget* cell, QCheckBox* chkBox, bool isIncompatible, bool isSelected)
{
    if (!isIncompatible) {
        // Reset styling for compatible streams
        cell->setStyleSheet("");
        cell->setProperty("incompatible", "false");
        chkBox->setStyleSheet("");
        chkBox->setIcon(QIcon());
        return;
    }

    // Common incompatibility setup
    QString incompatibilityReason = (m_type == Content::Audio) ? 
        tr("Audio codec not supported in target container") :
        tr("Subtitle codec not supported in target container - burn-in required");
    
    if (isSelected) {
        // Yellow background and warning icon for selected incompatible streams
        cell->setStyleSheet("QWidget#Cell[incompatible=\"true\"] { background-color: #fff3cd; border: 1px solid #ffeeba; border-radius: 3px; }"
                           "QWidget#Cell[incompatible=\"true\"][hover=\"true\"] { background-color: #ffe69c; border: 1px solid #ffcc02; }");
        cell->setProperty("incompatible", "true");
        chkBox->setStyleSheet("QCheckBox { color: #856404; }");
        chkBox->setToolTip(tr("WARNING: %1 - This will cause encoding issues!").arg(incompatibilityReason));
        
        // Add warning icon for selected incompatible streams
        QIcon warningIcon;
        if (QFile::exists(":/resources/icons/svg/warning.svg")) {
            warningIcon = QIcon(":/resources/icons/svg/warning.svg");
        } else {
            warningIcon = style()->standardIcon(QStyle::SP_MessageBoxWarning);
        }
        
        if (!warningIcon.isNull()) {
            chkBox->setIcon(warningIcon);
            chkBox->setIconSize(QSize(12, 12) * Helper::scaling());
        }
    } else {
        // Reset styling for unselected incompatible streams (no warning needed)
        cell->setStyleSheet("");
        cell->setProperty("incompatible", "false");
        chkBox->setStyleSheet("");
        chkBox->setIcon(QIcon());
        chkBox->setToolTip("");
    }
}
