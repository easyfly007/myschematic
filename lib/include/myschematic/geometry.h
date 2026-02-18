#pragma once

#include <QPointF>
#include <QRectF>

namespace myschematic {

// Snap a coordinate to the nearest grid point
inline double snapToGrid(double value, double gridSize) {
    return qRound(value / gridSize) * gridSize;
}

// Snap a point to the nearest grid point
inline QPointF snapToGrid(const QPointF& point, double gridSize) {
    return QPointF(snapToGrid(point.x(), gridSize),
                   snapToGrid(point.y(), gridSize));
}

} // namespace myschematic
