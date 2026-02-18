#include "commands/change_property_cmd.h"
#include "view/schematic_scene.h"
#include "view/items/component_item.h"
#include <myschematic/component.h>
#include <myschematic/property.h>

ChangePropertyCmd::ChangePropertyCmd(myschematic::Component* component,
                                     const QString& propertyName,
                                     const QString& newValue,
                                     SchematicScene* scene,
                                     QUndoCommand* parent)
    : QUndoCommand(parent)
    , m_component(component)
    , m_scene(scene)
    , m_propertyName(propertyName)
    , m_oldValue(component->property(propertyName))
    , m_newValue(newValue)
{
    setText(QStringLiteral("Change property '%1'").arg(propertyName));
}

void ChangePropertyCmd::undo() {
    m_component->setProperty(m_propertyName, m_oldValue);

    // Update the visual item in case instance name or visible property changed
    if (auto* item = m_scene->componentItemById(m_component->id())) {
        item->update();
    }
}

void ChangePropertyCmd::redo() {
    m_component->setProperty(m_propertyName, m_newValue);

    // Update the visual item in case instance name or visible property changed
    if (auto* item = m_scene->componentItemById(m_component->id())) {
        item->update();
    }
}
