#include "view/items/component_item.h"
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QPen>
#include <QFont>

ComponentItem::ComponentItem(const QString& elementId, const QString& instanceName,
                             QGraphicsItem* parent)
    : QGraphicsItem(parent)
    , m_elementId(elementId)
    , m_instanceName(instanceName)
{
    setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
}

QRectF ComponentItem::boundingRect() const {
    double margin = 2.0;
    return QRectF(-kWidth / 2.0 - margin, -kHeight / 2.0 - margin,
                  kWidth + 2 * margin, kHeight + 2 * margin);
}

void ComponentItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
                          QWidget* /*widget*/) {
    QRectF body(-kWidth / 2.0, -kHeight / 2.0, kWidth, kHeight);

    // Draw body
    QPen pen(Qt::blue, 1.5);
    if (option->state & QStyle::State_Selected) {
        pen.setStyle(Qt::DashLine);
        pen.setColor(Qt::red);
    }
    painter->setPen(pen);
    painter->setBrush(QColor(200, 220, 255, 80));
    painter->drawRect(body);

    // Draw instance name
    painter->setPen(Qt::black);
    QFont font;
    font.setPixelSize(10);
    painter->setFont(font);
    painter->drawText(body, Qt::AlignCenter, m_instanceName);

    // Draw selection handles
    if (option->state & QStyle::State_Selected) {
        double hs = 3.0;
        painter->setPen(Qt::NoPen);
        painter->setBrush(Qt::red);
        QRectF r = body;
        // 8 handles: corners + midpoints
        QPointF handles[] = {
            r.topLeft(), r.topRight(), r.bottomLeft(), r.bottomRight(),
            {r.center().x(), r.top()}, {r.center().x(), r.bottom()},
            {r.left(), r.center().y()}, {r.right(), r.center().y()}
        };
        for (const auto& h : handles) {
            painter->drawRect(QRectF(h.x() - hs/2, h.y() - hs/2, hs, hs));
        }
    }
}

void ComponentItem::setInstanceName(const QString& name) {
    m_instanceName = name;
    update();
}

QVariant ComponentItem::itemChange(GraphicsItemChange change, const QVariant& value) {
    return QGraphicsItem::itemChange(change, value);
}
