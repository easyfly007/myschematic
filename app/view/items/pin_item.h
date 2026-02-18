#pragma once

#include <QGraphicsItem>

namespace myschematic {
class Pin;
}

class PinItem : public QGraphicsItem {
public:
    PinItem(myschematic::Pin* pin, QGraphicsItem* parent = nullptr);

    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
               QWidget* widget) override;

    myschematic::Pin* pin() const { return m_pin; }
    void updatePosition();

private:
    static constexpr double kPinRadius = 3.0;

    myschematic::Pin* m_pin;
};
