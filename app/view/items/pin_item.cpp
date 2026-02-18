#include "view/items/pin_item.h"
#include <myschematic/pin.h>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QPen>
#include <QFont>

PinItem::PinItem(myschematic::Pin* pin, QGraphicsItem* parent)
    : QGraphicsItem(parent)
    , m_pin(pin)
{
    setFlag(QGraphicsItem::ItemIsSelectable, false);
    updatePosition();
}

QRectF PinItem::boundingRect() const {
    // Small circle plus room for pin name text
    double margin = 20.0;
    return QRectF(-kPinRadius - margin, -kPinRadius - margin,
                  2.0 * (kPinRadius + margin), 2.0 * (kPinRadius + margin));
}

void PinItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* /*option*/,
                    QWidget* /*widget*/) {
    // Determine connection state color
    bool connected = m_pin->isConnected();
    QColor fillColor = connected ? QColor(60, 120, 255) : QColor(60, 200, 60);
    QColor borderColor = connected ? Qt::darkBlue : Qt::darkGreen;

    // Draw pin circle
    painter->setPen(QPen(borderColor, 1.0));
    painter->setBrush(fillColor);
    painter->drawEllipse(QPointF(0, 0), kPinRadius, kPinRadius);

    // Draw pin name
    painter->setPen(Qt::darkGray);
    QFont font;
    font.setPointSize(6);
    painter->setFont(font);
    painter->drawText(QPointF(kPinRadius + 2.0, 3.0), m_pin->name());
}

void PinItem::updatePosition() {
    QPointF worldPos = m_pin->worldPosition();
    // If we have a parent item, set position relative to parent
    if (parentItem()) {
        setPos(worldPos - parentItem()->pos());
    } else {
        setPos(worldPos);
    }
}
