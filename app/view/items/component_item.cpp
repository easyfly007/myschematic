#include "view/items/component_item.h"
#include <myschematic/component.h>
#include <myschematic/symbol_def.h>
#include <myschematic/symbol_graphic.h>
#include <myschematic/symbol_pin.h>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QPen>
#include <QFont>
#include <QPolygonF>

ComponentItem::ComponentItem(myschematic::Component* component, QGraphicsItem* parent)
    : QGraphicsItem(parent)
    , m_component(component)
    , m_elementId(component->id())
    , m_instanceName(component->instanceName())
{
    setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
}

QRectF ComponentItem::boundingRect() const {
    const auto* sym = m_component->symbolDef();
    if (sym) {
        QRectF sr = sym->boundingRect();
        // Add margin for pin circles and text
        double margin = 8.0;
        QRectF expanded = sr.adjusted(-margin, -margin, margin, margin);
        // Add extra space below for instance name
        expanded.setBottom(expanded.bottom() + 17.0);
        return expanded;
    }
    // Fallback: fixed rect with text area below
    double margin = 2.0;
    return QRectF(-kWidth / 2.0 - margin, -kHeight / 2.0 - margin,
                  kWidth + 2 * margin, kHeight + 2 * margin + 17.0);
}

void ComponentItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
                          QWidget* /*widget*/) {
    const auto* sym = m_component->symbolDef();

    // LOD check
    qreal lod = option->levelOfDetailFromTransform(painter->worldTransform());
    if (lod < 0.3) {
        painter->fillRect(boundingRect(), QColor(100, 100, 200, 50));
        return;
    }

    painter->save();

    // Apply mirror
    if (m_component->isMirrored()) {
        painter->scale(-1, 1);
    }

    if (sym) {
        // Draw symbol graphics
        QPen pen(Qt::darkBlue, 1.5);
        painter->setPen(pen);
        for (const auto& graphic : sym->graphics()) {
            drawGraphic(painter, graphic.get());
        }
        // Draw pins
        if (lod > 0.5) {
            drawPins(painter, sym);
        }
    } else {
        // Fallback blue rectangle
        drawFallbackRect(painter);
    }

    painter->restore();

    // Draw instance name (always in world orientation, not mirrored/rotated)
    if (lod > 0.4 && !m_component->instanceName().isEmpty()) {
        painter->setPen(Qt::black);
        QFont font;
        font.setPointSize(8);
        painter->setFont(font);
        QRectF textRect = boundingRect();
        textRect.moveTop(textRect.bottom() - 15.0);
        textRect.setHeight(15);
        painter->drawText(textRect, Qt::AlignHCenter | Qt::AlignTop,
                          m_component->instanceName());
    }

    // Selection handles
    if (isSelected()) {
        drawSelectionHandles(painter);
    }
}

void ComponentItem::setInstanceName(const QString& name) {
    m_instanceName = name;
    update();
}

QVariant ComponentItem::itemChange(GraphicsItemChange change, const QVariant& value) {
    return QGraphicsItem::itemChange(change, value);
}

void ComponentItem::drawGraphic(QPainter* painter,
                                const myschematic::SymbolGraphic* graphic) {
    using Type = myschematic::SymbolGraphic::Type;

    switch (graphic->graphicType()) {
    case Type::Line: {
        auto* line = static_cast<const myschematic::GraphicLine*>(graphic);
        painter->drawLine(QPointF(line->x1, line->y1), QPointF(line->x2, line->y2));
        break;
    }
    case Type::Rect: {
        auto* rect = static_cast<const myschematic::GraphicRect*>(graphic);
        QRectF r(rect->x, rect->y, rect->width, rect->height);
        if (rect->filled) {
            painter->save();
            painter->setBrush(QColor(200, 220, 255, 80));
            painter->drawRect(r);
            painter->restore();
        } else {
            painter->drawRect(r);
        }
        break;
    }
    case Type::Circle: {
        auto* circle = static_cast<const myschematic::GraphicCircle*>(graphic);
        QPointF center(circle->cx, circle->cy);
        double r = circle->radius;
        if (circle->filled) {
            painter->save();
            painter->setBrush(QColor(200, 220, 255, 80));
            painter->drawEllipse(center, r, r);
            painter->restore();
        } else {
            painter->drawEllipse(center, r, r);
        }
        break;
    }
    case Type::Arc: {
        auto* arc = static_cast<const myschematic::GraphicArc*>(graphic);
        QRectF arcRect(arc->cx - arc->radius, arc->cy - arc->radius,
                       arc->radius * 2.0, arc->radius * 2.0);
        // QPainter::drawArc uses 1/16th degree units
        int startAngle16 = static_cast<int>(arc->startAngle * 16.0);
        int spanAngle16 = static_cast<int>(arc->spanAngle * 16.0);
        painter->drawArc(arcRect, startAngle16, spanAngle16);
        break;
    }
    case Type::Polyline: {
        auto* poly = static_cast<const myschematic::GraphicPolyline*>(graphic);
        if (poly->points.isEmpty())
            break;
        QPolygonF polygon(poly->points);
        if (poly->filled) {
            painter->save();
            painter->setBrush(QColor(200, 220, 255, 80));
            painter->drawPolygon(polygon);
            painter->restore();
        } else {
            painter->drawPolyline(polygon);
        }
        break;
    }
    case Type::Text: {
        auto* text = static_cast<const myschematic::GraphicText*>(graphic);
        painter->save();
        QFont font;
        font.setPointSizeF(text->fontSize);
        painter->setFont(font);
        painter->drawText(QPointF(text->x, text->y), text->text);
        painter->restore();
        break;
    }
    }
}

void ComponentItem::drawPins(QPainter* painter, const myschematic::SymbolDef* sym) {
    const double pinRadius = 3.0;

    painter->save();

    for (const auto& pin : sym->pins()) {
        QPointF pinPos = pin.offset;

        // Draw pin circle
        painter->setPen(QPen(Qt::darkGreen, 1.0));
        painter->setBrush(Qt::green);
        painter->drawEllipse(pinPos, pinRadius, pinRadius);

        // Draw pin name
        painter->setPen(Qt::darkGray);
        QFont font;
        font.setPointSize(6);
        painter->setFont(font);

        // Position text based on pin direction
        QPointF textPos = pinPos;
        Qt::Alignment align = Qt::AlignLeft | Qt::AlignVCenter;
        QRectF textRect;

        switch (pin.direction) {
        case myschematic::PinDirection::Left:
            textRect = QRectF(pinPos.x() - 40.0, pinPos.y() - 6.0, 35.0, 12.0);
            align = Qt::AlignRight | Qt::AlignVCenter;
            break;
        case myschematic::PinDirection::Right:
            textRect = QRectF(pinPos.x() + 5.0, pinPos.y() - 6.0, 35.0, 12.0);
            align = Qt::AlignLeft | Qt::AlignVCenter;
            break;
        case myschematic::PinDirection::Up:
            textRect = QRectF(pinPos.x() - 17.5, pinPos.y() - 18.0, 35.0, 12.0);
            align = Qt::AlignHCenter | Qt::AlignBottom;
            break;
        case myschematic::PinDirection::Down:
            textRect = QRectF(pinPos.x() - 17.5, pinPos.y() + 5.0, 35.0, 12.0);
            align = Qt::AlignHCenter | Qt::AlignTop;
            break;
        }

        painter->drawText(textRect, align, pin.name);
    }

    painter->restore();
}

void ComponentItem::drawFallbackRect(QPainter* painter) {
    QRectF body(-kWidth / 2.0, -kHeight / 2.0, kWidth, kHeight);

    QPen pen(Qt::blue, 1.5);
    painter->setPen(pen);
    painter->setBrush(QColor(200, 220, 255, 80));
    painter->drawRect(body);

    // Draw instance name centered in body
    painter->setPen(Qt::black);
    QFont font;
    font.setPixelSize(10);
    painter->setFont(font);
    painter->drawText(body, Qt::AlignCenter, m_instanceName);
}

void ComponentItem::drawSelectionHandles(QPainter* painter) {
    const auto* sym = m_component->symbolDef();
    QRectF body;
    if (sym) {
        body = sym->boundingRect();
    } else {
        body = QRectF(-kWidth / 2.0, -kHeight / 2.0, kWidth, kHeight);
    }

    // Dashed selection border
    QPen dashPen(Qt::red, 1.0, Qt::DashLine);
    painter->setPen(dashPen);
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(body);

    // 8 handles: corners + midpoints
    double hs = 3.0;
    painter->setPen(Qt::NoPen);
    painter->setBrush(Qt::red);
    QPointF handles[] = {
        body.topLeft(), body.topRight(), body.bottomLeft(), body.bottomRight(),
        {body.center().x(), body.top()}, {body.center().x(), body.bottom()},
        {body.left(), body.center().y()}, {body.right(), body.center().y()}
    };
    for (const auto& h : handles) {
        painter->drawRect(QRectF(h.x() - hs / 2, h.y() - hs / 2, hs, hs));
    }
}
