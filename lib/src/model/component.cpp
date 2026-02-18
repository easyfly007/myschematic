#include "myschematic/component.h"

namespace myschematic {

Component::Component(QObject* parent)
    : SchematicElement(parent)
{
}

Component::~Component() = default;

QRectF Component::boundingRect() const {
    double w = kWidth;
    double h = kHeight;
    // Swap width/height for 90 or 270 degree rotation
    if (m_rotation == 90 || m_rotation == 270) {
        std::swap(w, h);
    }
    return QRectF(m_position.x() - w / 2.0,
                  m_position.y() - h / 2.0,
                  w, h);
}

ElementType Component::type() const {
    return ElementType::Component;
}

QString Component::instanceName() const {
    return m_instanceName;
}

void Component::setInstanceName(const QString& name) {
    if (m_instanceName != name) {
        m_instanceName = name;
        emit changed();
    }
}

} // namespace myschematic
