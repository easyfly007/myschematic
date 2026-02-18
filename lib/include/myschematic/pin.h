#pragma once

#include <QPointF>
#include <QString>

namespace myschematic {

class Component;
struct SymbolPin;

class Pin {
public:
    Pin(Component* component, const SymbolPin* symbolPin);

    QString name() const;
    const SymbolPin* symbolPin() const;
    Component* component() const;

    /// Computes the world position of this pin.
    /// worldPosition = component.position + transform(symbolPin.offset)
    /// Transform applies mirror-then-rotate:
    ///   Mirror: if mirrored, flip X (offset.x = -offset.x)
    ///   Rotate: 0=identity, 90=(-y,x), 180=(-x,-y), 270=(y,-x)
    QPointF worldPosition() const;

private:
    Component* m_component;
    const SymbolPin* m_symbolPin;
};

} // namespace myschematic
