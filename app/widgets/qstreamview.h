#ifndef QSTREAMVIEW_H
#define QSTREAMVIEW_H

#include <QWidget>
#include <QVBoxLayout>
#include "constants.h"

class QCheckBox;

using namespace Constants;

class QStreamView : public QWidget
{
    Q_OBJECT
public:
    enum class Content {
        Audio, Subtitle
    };
    enum class TrackType {
        Internal, External
    };
    QStreamView(QWidget *parent);
    ~QStreamView() override;
    void setContentType(Content type);
    void clearList();
    void setList(QString container, Data &data, const QString& targetAudioCodec = QString(), bool usePresetSubtitleSettings = false);
    void deselectTitles();
    void clearTitles();
    void undoTitles();

signals:
    void onExtractTrack(QStreamView::Content type, int track);
    void streamSelectionChanged();

private:
    // UI flag types for resetFlags function
    enum class FlagType {
        Check,      // Selection checkboxes
        Default,    // Default stream radio buttons
        Burn        // Burn-into-video radio buttons
    };
    
    bool eventFilter(QObject*, QEvent*) final;
    void resetFlags(FlagType type, int excludeIndex);
    void updateIncompatibleStreamStyling(QWidget* cell, QCheckBox* chkBox, bool isIncompatible, bool isSelected);
    
    // Event handlers for UI interactions
    void onDefaultStreamClicked(QWidget* cell, bool checked, bool& deflt, bool& state, bool& burn);
    void onBurnIntoClicked(QWidget* cell, bool checked, bool& burn, bool& deflt, bool& state, bool burnOnly);
    void onStreamCheckboxClicked(QWidget* cell, QCheckBox* chkBox, bool& state, bool& deflt, bool& burn, bool burnOnly, bool isIncompatible);
    
    QWidget *createCell(bool &state,
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
                        bool externFlag = false);
    QVBoxLayout *m_pLayout;
    Content m_type;
    Data *m_pData;
    QString m_targetAudioCodec;
    bool m_usePresetSubtitleSettings;
};

#endif // QSTREAMVIEW_H
