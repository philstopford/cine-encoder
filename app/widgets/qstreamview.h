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
    void setList(QString container, Data &data, int audioCodecIndex = 0, bool usePresetSubtitleSettings = false);
    void deselectTitles();
    void clearTitles();
    void undoTitles();

signals:
    void onExtractTrack(QStreamView::Content type, int track);
    void streamSelectionChanged();

private:
    bool eventFilter(QObject*, QEvent*) final;
    void resetCheckFlags(int ind);
    void resetDefFlags(int ind);
    void resetBurnFlags(int ind);
    void updateIncompatibleStreamStyling(QWidget* cell, QCheckBox* chkBox, bool isIncompatible, bool isSelected);
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
    int m_audioCodecIndex;
    bool m_usePresetSubtitleSettings;
};

#endif // QSTREAMVIEW_H
