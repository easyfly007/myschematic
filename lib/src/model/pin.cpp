#include "myschematic/pin.h"

#include "myschematic/component.h"
#include "myschematic/symbol_pin.h"

namespace myschematic {

Pin::Pin(Component* component, const SymbolPin* symbolPin)
    : m_component(component)
    , m_symbolPin(symbolPin)
{
}

QString Pin::name() const
{
    return m_symbolPin->name;
}

const SymbolPin* Pin::symbolPin() const
{
    return m_symbolPin;
}

Component* Pin::component() const
{
    return m_component;
}

QPointF Pin::worldPosition() const
{
    QPointF offset = m_symbolPin->offset;

    // Mirror first (flip X)
    if (m_component->isMirrored()) {
        offset.setX(-offset.x());
    }

    // Then rotate
    switch (m_component->rotation()) {
    case 90:
        offset = QPointF(-offset.y(), offset.x());
        break;
    case 180:
        offset = QPointF(-offset.x(), -offset.y());
        break;
    case 270:
        offset = QPointF(offset.y(), -offset.x());
        break;
    default:
        break; // 0 degrees, no change
    }

    return m_component->position() + offset;
}

} // namespace myschematic
