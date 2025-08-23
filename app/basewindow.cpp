/***********************************************************************

                          C I N E   E N C O D E R
                                JULY, 2020
                            COPYRIGHT (C) 2020

 FILE: basewindow.cpp
 COMMENT: Modernized unified base window implementation using public Qt6 APIs
 LICENSE: GNU General Public License v3.0

***********************************************************************/

#include "basewindow.h"
#include "helper.h"
#include <QApplication>
#include <QMouseEvent>
#include <QEventLoop>
#include <QTimer>
#include <QGraphicsDropShadowEffect>
#include <QWindow>
#include <QCursor>

#define BORDER int(15 * Helper::scaling())

BaseWindow::BaseWindow(QWidget *parent, bool isResizable) :
    QMainWindow(parent, Qt::Window | Qt::FramelessWindowHint),
    m_titlebar(nullptr),
    m_maskwidget(nullptr),
    m_windowActivated(false),
    m_isResizable(isResizable),
    m_resizeEdge(ResizeEdge::None),
    m_borderWidth(BORDER)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_ShowModal);
    auto *ui_widget = new QWidget(this);
    setCentralWidget(ui_widget);
    setMaskWidget(ui_widget);
    installEventFilter(this);
    setMouseTracking(true);
    setWindowIcon(QIcon(QPixmap(":/resources/icons/svg/cine-encoder.svg")));
}

BaseWindow::~BaseWindow()
= default;

int BaseWindow::exec()
{
    show();
    setWindowModality(Qt::ApplicationModal);
    QEventLoop loop;
    connect(this, &BaseWindow::exitLoop, &loop, &QEventLoop::exit);
    const int code = loop.exec();
    hide();
    this->deleteLater();
    return code;
}

void BaseWindow::acceptDialog()
{
    emit BaseWindow::exitLoop(Dialog::Accept);
}

void BaseWindow::closeDialog()
{
    emit BaseWindow::exitLoop(Dialog::Decline);
}

void BaseWindow::setTitleBar(QWidget* titlebar)
{
    m_titlebar = titlebar;
    m_titlebar->installEventFilter(this);
}

void BaseWindow::setMaskWidget(QWidget* maskwidget)
{
    m_maskwidget = maskwidget;
    m_maskwidget->setAttribute(Qt::WA_Hover, true);
    m_maskwidget->setAttribute(Qt::WA_NoMousePropagation, true);
    m_maskwidget->installEventFilter(this);
    auto shadow = QGraphicsDropShadowEffect(m_maskwidget);
    shadow.setBlurRadius(25.0);
    shadow.setColor(QColor(0, 0, 0, 80));
    shadow.setOffset(0.0);
    m_maskwidget->setGraphicsEffect(&shadow);
}

void BaseWindow::onExpandWindow()
{
    isMaximized() ? showNormal() : showMaximized();
}

void BaseWindow::showEvent(QShowEvent *event)
{
    QMainWindow::showEvent(event);
    if (!m_windowActivated) {
        m_windowActivated = true;
        setContentsMargins(m_borderWidth, m_borderWidth, m_borderWidth, m_borderWidth);
    }
}

BaseWindow::ResizeEdge BaseWindow::getResizeEdge(const QPoint &pos) const
{
    if (!m_isResizable) {
        return ResizeEdge::None;
    }
    
    const QRect rect = this->rect();
    const QMargins margins = contentsMargins();
    const QRect contentRect = rect.marginsRemoved(margins);
    
    int edge = static_cast<int>(ResizeEdge::None);
    
    // Check for top edge
    if (pos.y() >= contentRect.top() - m_borderWidth && pos.y() < contentRect.top()) {
        edge |= static_cast<int>(ResizeEdge::Top);
    }
    // Check for bottom edge
    if (pos.y() <= contentRect.bottom() + m_borderWidth && pos.y() > contentRect.bottom()) {
        edge |= static_cast<int>(ResizeEdge::Bottom);
    }
    // Check for left edge
    if (pos.x() >= contentRect.left() - m_borderWidth && pos.x() < contentRect.left()) {
        edge |= static_cast<int>(ResizeEdge::Left);
    }
    // Check for right edge
    if (pos.x() <= contentRect.right() + m_borderWidth && pos.x() > contentRect.right()) {
        edge |= static_cast<int>(ResizeEdge::Right);
    }
    
    return static_cast<ResizeEdge>(edge);
}

void BaseWindow::updateCursor(const QPoint &pos)
{
    const ResizeEdge edge = getResizeEdge(pos);
    
    switch (edge) {
        case ResizeEdge::Top:
        case ResizeEdge::Bottom:
            setCursor(Qt::SizeVerCursor);
            break;
        case ResizeEdge::Left:
        case ResizeEdge::Right:
            setCursor(Qt::SizeHorCursor);
            break;
        case ResizeEdge::TopLeft:
        case ResizeEdge::BottomRight:
            setCursor(Qt::SizeFDiagCursor);
            break;
        case ResizeEdge::TopRight:
        case ResizeEdge::BottomLeft:
            setCursor(Qt::SizeBDiagCursor);
            break;
        default:
            setCursor(Qt::ArrowCursor);
            break;
    }
}

void BaseWindow::startResize(ResizeEdge edge)
{
    if (!m_isResizable || edge == ResizeEdge::None) {
        return;
    }
    
    // Convert our ResizeEdge enum to Qt's edges
    Qt::Edges qtEdges = Qt::Edges();
    
    if (static_cast<int>(edge) & static_cast<int>(ResizeEdge::Top)) {
        qtEdges |= Qt::TopEdge;
    }
    if (static_cast<int>(edge) & static_cast<int>(ResizeEdge::Bottom)) {
        qtEdges |= Qt::BottomEdge;
    }
    if (static_cast<int>(edge) & static_cast<int>(ResizeEdge::Left)) {
        qtEdges |= Qt::LeftEdge;
    }
    if (static_cast<int>(edge) & static_cast<int>(ResizeEdge::Right)) {
        qtEdges |= Qt::RightEdge;
    }
    
    // Use Qt6's public API for window resizing
    if (windowHandle()) {
        windowHandle()->startSystemResize(qtEdges);
    }
}

void BaseWindow::startMove()
{
    // Use Qt6's public API for window movement
    if (windowHandle()) {
        windowHandle()->startSystemMove();
    }
}

void BaseWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (m_resizeEdge == ResizeEdge::None && m_isResizable) {
        updateCursor(event->pos());
    }
    return QMainWindow::mouseMoveEvent(event);
}

void BaseWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && m_isResizable) {
        const ResizeEdge edge = getResizeEdge(event->pos());
        if (edge != ResizeEdge::None) {
            m_resizeEdge = edge;
            startResize(edge);
            return;
        }
    }
    return QMainWindow::mousePressEvent(event);
}

void BaseWindow::mouseReleaseEvent(QMouseEvent *event)
{
    m_resizeEdge = ResizeEdge::None;
    setCursor(Qt::ArrowCursor);
    return QMainWindow::mouseReleaseEvent(event);
}

void BaseWindow::resizeEvent(QResizeEvent *event)
{
    // No special window extents handling needed with public Qt6 APIs
    return QMainWindow::resizeEvent(event);
}

void BaseWindow::changeEvent(QEvent *event)
{
    QMainWindow::changeEvent(event);
    if (event->type() == QEvent::WindowStateChange) {
        const int margin = isMaximized() ? 0 : m_borderWidth;
        setContentsMargins(margin, margin, margin, margin);
        QTimer::singleShot(50, this, [this](){
            update();
        });
    }
}

bool BaseWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_maskwidget) {
        if (event->type() == QEvent::HoverEnter) {
            QApplication::setOverrideCursor(QCursor(Qt::ArrowCursor));
            QApplication::restoreOverrideCursor();
            return true;
        }
    } else if (watched == m_titlebar) {
        if (event->type() == QEvent::MouseButtonPress) {
            auto* mouse_event = dynamic_cast<QMouseEvent*>(event);
            if (mouse_event->buttons() & Qt::LeftButton) {
                startMove();
                return true;
            }
        } else if (event->type() == QEvent::MouseButtonDblClick) {
            auto* mouse_event = dynamic_cast<QMouseEvent*>(event);
            if (mouse_event->buttons() & Qt::LeftButton) {
                if (m_isResizable)
                    onExpandWindow();
                return true;
            }
        }
    }
    return QMainWindow::eventFilter(watched, event);
}