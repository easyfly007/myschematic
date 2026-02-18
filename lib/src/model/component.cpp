#include "myschematic/component.h"

#include "myschematic/pin.h"
#include "myschematic/symbol_def.h"

#include <algorithm>

namespace myschematic {

Component::Component(QObject* parent)
    : SchematicElement(parent)
{
}

Component::Component(const SymbolDef* symbolDef, QObject* parent)
    : SchematicElement(parent)
    , m_symbolDef(symbolDef)
{
    if (m_symbolDef) {
        rebuildPins();
        m_properties.initFromTemplates(m_symbolDef->propertyTemplates());
    }
}

Component::~Component()
{
    qDeleteAll(m_pins);
}

QRectF Component::boundingRect() const
{
    double w = kWidth;
    double h = kHeight;

    if (m_symbolDef) {
        QRectF symRect = m_symbolDef->boundingRect();
        if (!symRect.isNull()) {
            w = symRect.width();
            h = symRect.height();

            // For 90/270 rotation, swap width and height
            if (m_rotation == 90 || m_rotation == 270) {
                std::swap(w, h);
            }

            // Compute the center of the symbol rect (in symbol coordinates)
            double cx = symRect.center().x();
            double cy = symRect.center().y();

            // Apply mirror
            if (m_mirrored) {
                cx = -cx;
            }

            // Apply rotation to center offset
            double rx = cx;
            double ry = cy;
            switch (m_rotation) {
            case 90:
                rx = -cy;
                ry = cx;
                break;
            case 180:
                rx = -cx;
                ry = -cy;
                break;
            case 270:
                rx = cy;
                ry = -cx;
                break;
            default:
                break;
            }

            return QRectF(m_position.x() + rx - w / 2.0,
                          m_position.y() + ry - h / 2.0,
                          w, h);
        }
    }

    // Fallback: fixed-size bounding rect
    if (m_rotation == 90 || m_rotation == 270) {
        std::swap(w, h);
    }
    return QRectF(m_position.x() - w / 2.0,
                  m_position.y() - h / 2.0,
                  w, h);
}

ElementType Component::type() const
{
    return ElementType::Component;
}

QString Component::instanceName() const
{
    return m_instanceName;
}

void Component::setInstanceName(const QString& name)
{
    if (m_instanceName != name) {
        m_instanceName = name;
        emit changed();
    }
}

const SymbolDef* Component::symbolDef() const
{
    return m_symbolDef;
}

void Component::setSymbolDef(const SymbolDef* def)
{
    m_symbolDef = def;
    rebuildPins();
    if (m_symbolDef) {
        m_properties.initFromTemplates(m_symbolDef->propertyTemplates());
    }
    emit changed();
}

const QList<Pin*>& Component::pins() const
{
    return m_pins;
}

Pin* Component::pinByName(const QString& name) const
{
    for (auto* pin : m_pins) {
        if (pin->name() == name) {
            return pin;
        }
    }
    return nullptr;
}

QPointF Component::pinWorldPosition(const QString& pinName) const
{
    Pin* pin = pinByName(pinName);
    if (pin) {
        return pin->worldPosition();
    }
    return m_position;
}

PropertyBag& Component::properties()
{
    return m_properties;
}

const PropertyBag& Component::properties() const
{
    return m_properties;
}

void Component::setProperty(const QString& name, const QString& value)
{
    m_properties.set(name, value);
}

QString Component::property(const QString& name) const
{
    return m_properties.get(name);
}

void Component::rebuildPins()
{
    qDeleteAll(m_pins);
    m_pins.clear();

    if (!m_symbolDef) {
        return;
    }

    const auto& symbolPins = m_symbolDef->pins();
    for (int i = 0; i < symbolPins.size(); ++i) {
        m_pins.append(new Pin(this, &symbolPins[i]));
    }
}

} // namespace myschematic
