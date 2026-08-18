#ifndef QEFFECTSTACK_H
#define QEFFECTSTACK_H

#include <QWidget>
#include <QStringList>

class QComboBox;
class QLabel;
class QLineEdit;
class QListWidget;
class QPushButton;
class QTableWidget;

class QEffectStack : public QWidget
{
    Q_OBJECT
public:
    explicit QEffectStack(QWidget *parent = nullptr);
    void setFilters(QStringList *filters);
    void setPreviewButtonVisible(bool visible);
    static QStringList enabledFilters(const QStringList &filters);

signals:
    void filtersChanged();
    void previewRequested();

private slots:
    void addFilter();
    void removeFilter();
    void moveUp();
    void moveDown();
    void selectionChanged();
    void expressionEdited();
    void parameterEdited();
    void itemChanged();

private:
    void rebuild();
    void syncModel();

    QStringList *m_filters = nullptr;
    QComboBox *m_catalog;
    QListWidget *m_list;
    QLabel *m_description;
    QLineEdit *m_expression;
    QTableWidget *m_parameters;
    QPushButton *m_remove, *m_up, *m_down, *m_preview;
    bool m_updating = false;
};

#endif
