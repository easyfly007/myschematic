#pragma once

#include <QList>
#include <QPointF>
#include <QRectF>
#include <QString>
#include <algorithm>
#include <cmath>

namespace myschematic {

class SymbolGraphic {
public:
    enum class Type { Line, Rect, Circle, Arc, Polyline, Text };
    virtual ~SymbolGraphic() = default;
    virtual Type graphicType() const = 0;
    virtual QRectF boundingRect() const = 0;
};

class GraphicLine : public SymbolGraphic {
public:
    GraphicLine(double x1, double y1, double x2, double y2)
        : x1(x1), y1(y1), x2(x2), y2(y2) {}

    Type graphicType() const override { return Type::Line; }

    QRectF boundingRect() const override {
        double left = std::min(x1, x2);
        double top = std::min(y1, y2);
        double w = std::abs(x2 - x1);
        double h = std::abs(y2 - y1);
        return QRectF(left, top, w, h);
    }

    double x1, y1, x2, y2;
};

class GraphicRect : public SymbolGraphic {
public:
    GraphicRect(double x, double y, double width, double height, bool filled = false)
        : x(x), y(y), width(width), height(height), filled(filled) {}

    Type graphicType() const override { return Type::Rect; }

    QRectF boundingRect() const override {
        return QRectF(x, y, width, height);
    }

    double x, y, width, height;
    bool filled;
};

class GraphicCircle : public SymbolGraphic {
public:
    GraphicCircle(double cx, double cy, double radius, bool filled = false)
        : cx(cx), cy(cy), radius(radius), filled(filled) {}

    Type graphicType() const override { return Type::Circle; }

    QRectF boundingRect() const override {
        return QRectF(cx - radius, cy - radius, radius * 2.0, radius * 2.0);
    }

    double cx, cy, radius;
    bool filled;
};

class GraphicArc : public SymbolGraphic {
public:
    GraphicArc(double cx, double cy, double radius, double startAngle, double spanAngle)
        : cx(cx), cy(cy), radius(radius), startAngle(startAngle), spanAngle(spanAngle) {}

    Type graphicType() const override { return Type::Arc; }

    QRectF boundingRect() const override {
        // Conservative bounding rect: full circle extent
        return QRectF(cx - radius, cy - radius, radius * 2.0, radius * 2.0);
    }

    double cx, cy, radius, startAngle, spanAngle;
};

class GraphicPolyline : public SymbolGraphic {
public:
    GraphicPolyline(const QList<QPointF>& points, bool filled = false)
        : points(points), filled(filled) {}

    Type graphicType() const override { return Type::Polyline; }

    QRectF boundingRect() const override {
        if (points.isEmpty())
            return QRectF();
        double minX = points.first().x();
        double minY = points.first().y();
        double maxX = minX;
        double maxY = minY;
        for (const auto& pt : points) {
            minX = std::min(minX, pt.x());
            minY = std::min(minY, pt.y());
            maxX = std::max(maxX, pt.x());
            maxY = std::max(maxY, pt.y());
        }
        return QRectF(minX, minY, maxX - minX, maxY - minY);
    }

    QList<QPointF> points;
    bool filled;
};

class GraphicText : public SymbolGraphic {
public:
    GraphicText(double x, double y, const QString& text, double fontSize = 10.0)
        : x(x), y(y), fontSize(fontSize), text(text) {}

    Type graphicType() const override { return Type::Text; }

    QRectF boundingRect() const override {
        // Approximate: each character ~0.6 * fontSize wide, height ~fontSize
        double w = text.length() * fontSize * 0.6;
        double h = fontSize;
        return QRectF(x, y - h, w, h);
    }

    double x, y, fontSize;
    QString text;
};

} // namespace myschematic
