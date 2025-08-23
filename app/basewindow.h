/***********************************************************************

                          C I N E   E N C O D E R
                                JULY, 2020
                            COPYRIGHT (C) 2020

 FILE: basewindow.h
 COMMENT: Modernized unified base window implementation using public Qt6 APIs
 LICENSE: GNU General Public License v3.0

***********************************************************************/

#ifndef BASEWINDOW_H
#define BASEWINDOW_H

#include <QMainWindow>
#include <QMouseEvent>
#include <QTimer>

class BaseWindow: public QMainWindow
{
    Q_OBJECT
public:
    explicit BaseWindow(QWidget *parent = nullptr, bool isResizable = true);
    ~BaseWindow() override;
    int exec();
    void acceptDialog();
    void closeDialog();
    enum Dialog {
        Decline, Accept
    };

signals:
    void exitLoop(int);

protected:
    void setTitleBar(QWidget*);
    void setMaskWidget(QWidget*);

    virtual void onExpandWindow();
    void showEvent(QShowEvent*) override;
    void mouseMoveEvent(QMouseEvent *) override;
    void mousePressEvent(QMouseEvent *) override;
    void mouseReleaseEvent(QMouseEvent *) override;
    void resizeEvent(QResizeEvent *) override;
    void changeEvent(QEvent*) override;
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    enum class ResizeEdge {
        None = 0,
        Top = 1,
        Right = 2,
        Bottom = 4,
        Left = 8,
        TopLeft = Top | Left,
        TopRight = Top | Right,
        BottomLeft = Bottom | Left,
        BottomRight = Bottom | Right
    };
    
    ResizeEdge getResizeEdge(const QPoint &pos) const;
    void updateCursor(const QPoint &pos);
    void startResize(ResizeEdge edge);
    void startMove();
    
    QWidget *m_titlebar;
    QWidget *m_maskwidget;
    bool m_windowActivated;
    bool m_isResizable;
    ResizeEdge m_resizeEdge;
    QPoint m_dragStartPosition;
    int m_borderWidth;
};

#endif // BASEWINDOW_H