#include "view/schematic_view.h"
#include "view/schematic_scene.h"
#include <QWheelEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QScrollBar>
#include <cmath>

SchematicView::SchematicView(SchematicScene* scene, QWidget* parent)
    : QGraphicsView(scene, parent)
{
    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    setDragMode(QGraphicsView::NoDrag);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setResizeAnchor(QGraphicsView::AnchorViewCenter);
    setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Initial zoom
    scale(1.0, 1.0);
}

void SchematicView::fitToWindow() {
    if (scene() && !scene()->items().isEmpty()) {
        fitInView(scene()->itemsBoundingRect().adjusted(-50, -50, 50, 50),
                  Qt::KeepAspectRatio);
    }
}

void SchematicView::wheelEvent(QWheelEvent* event) {
    double factor = 1.15;
    if (event->angleDelta().y() < 0) {
        factor = 1.0 / factor;
    }

    double currentScale = transform().m11();
    double newScale = currentScale * factor;
    if (newScale < kMinScale || newScale > kMaxScale) {
        return;
    }

    scale(factor, factor);
}

void SchematicView::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::MiddleButton) {
        m_isPanning = true;
        m_panStartPos = event->pos();
        setCursor(Qt::ClosedHandCursor);
        event->accept();
        return;
    }
    QGraphicsView::mousePressEvent(event);
}

void SchematicView::mouseMoveEvent(QMouseEvent* event) {
    if (m_isPanning) {
        QPoint delta = event->pos() - m_panStartPos;
        m_panStartPos = event->pos();
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() - delta.x());
        verticalScrollBar()->setValue(verticalScrollBar()->value() - delta.y());
        event->accept();
        return;
    }
    QGraphicsView::mouseMoveEvent(event);
}

void SchematicView::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::MiddleButton && m_isPanning) {
        m_isPanning = false;
        setCursor(Qt::ArrowCursor);
        event->accept();
        return;
    }
    QGraphicsView::mouseReleaseEvent(event);
}

void SchematicView::drawBackground(QPainter* painter, const QRectF& rect) {
    QGraphicsView::drawBackground(painter, rect);

    double scale = transform().m11();

    // Don't draw grid if zoomed out too much
    if (scale < 0.2) return;

    painter->setPen(QPen(QColor(200, 200, 200), 0));

    double gridSize = kGridSize;
    // If zoomed out, increase grid spacing for performance
    if (scale < 0.5) {
        gridSize *= 5.0;
    }

    double left = std::floor(rect.left() / gridSize) * gridSize;
    double top = std::floor(rect.top() / gridSize) * gridSize;

    QVector<QPointF> points;
    for (double x = left; x <= rect.right(); x += gridSize) {
        for (double y = top; y <= rect.bottom(); y += gridSize) {
            points.append(QPointF(x, y));
        }
    }
    painter->drawPoints(points.data(), static_cast<int>(points.size()));
}
