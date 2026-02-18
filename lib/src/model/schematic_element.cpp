#include "myschematic/schematic_element.h"
#include "myschematic/id_generator.h"

namespace myschematic {

SchematicElement::SchematicElement(QObject* parent)
    : QObject(parent)
    , m_id(IdGenerator::generate())
{
}

SchematicElement::~SchematicElement() = default;

QString SchematicElement::id() const {
    return m_id;
}

QPointF SchematicElement::position() const {
    return m_position;
}

void SchematicElement::setPosition(const QPointF& pos) {
    if (m_position != pos) {
        m_position = pos;
        emit changed();
    }
}

int SchematicElement::rotation() const {
    return m_rotation;
}

void SchematicElement::setRotation(int degrees) {
    degrees = ((degrees % 360) + 360) % 360;
    if (m_rotation != degrees) {
        m_rotation = degrees;
        emit changed();
    }
}

bool SchematicElement::isMirrored() const {
    return m_mirrored;
}

void SchematicElement::setMirrored(bool mirrored) {
    if (m_mirrored != mirrored) {
        m_mirrored = mirrored;
        emit changed();
    }
}

} // namespace myschematic
