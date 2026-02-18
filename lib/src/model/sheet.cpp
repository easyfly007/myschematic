#include "myschematic/sheet.h"
#include "myschematic/schematic_element.h"
#include "myschematic/component.h"
#include "myschematic/id_generator.h"
#include <algorithm>
#include <cmath>

namespace myschematic {

Sheet::Sheet(const QString& name, QObject* parent)
    : QObject(parent)
    , m_id(IdGenerator::generate())
    , m_name(name)
{
}

Sheet::~Sheet() = default;

QString Sheet::id() const {
    return m_id;
}

QString Sheet::name() const {
    return m_name;
}

Component* Sheet::addComponent(const QPointF& pos) {
    auto* comp = new Component(this);
    comp->setPosition(pos);
    m_elements.append(comp);

    connect(comp, &SchematicElement::changed, this, [this, comp]() {
        emit elementChanged(comp);
    });

    emit elementAdded(comp);
    return comp;
}

void Sheet::removeElement(const QString& id) {
    auto it = std::find_if(m_elements.begin(), m_elements.end(),
                           [&id](SchematicElement* e) { return e->id() == id; });
    if (it != m_elements.end()) {
        SchematicElement* elem = *it;
        m_elements.erase(it);
        emit elementRemoved(id);
        elem->deleteLater();
    }
}

SchematicElement* Sheet::elementById(const QString& id) const {
    auto it = std::find_if(m_elements.begin(), m_elements.end(),
                           [&id](SchematicElement* e) { return e->id() == id; });
    return (it != m_elements.end()) ? *it : nullptr;
}

Component* Sheet::componentById(const QString& id) const {
    auto* elem = elementById(id);
    return qobject_cast<Component*>(elem);
}

QList<Component*> Sheet::components() const {
    QList<Component*> result;
    for (auto* elem : m_elements) {
        if (auto* comp = qobject_cast<Component*>(elem)) {
            result.append(comp);
        }
    }
    return result;
}

QList<SchematicElement*> Sheet::elements() const {
    return m_elements;
}

QList<SchematicElement*> Sheet::elementsAt(const QPointF& pos, qreal radius) const {
    QList<SchematicElement*> result;
    for (auto* elem : m_elements) {
        double dx = elem->position().x() - pos.x();
        double dy = elem->position().y() - pos.y();
        if (std::sqrt(dx * dx + dy * dy) <= radius) {
            result.append(elem);
        }
    }
    return result;
}

QList<SchematicElement*> Sheet::elementsInRect(const QRectF& rect) const {
    QList<SchematicElement*> result;
    for (auto* elem : m_elements) {
        if (rect.intersects(elem->boundingRect())) {
            result.append(elem);
        }
    }
    return result;
}

} // namespace myschematic
