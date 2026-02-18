#include "commands/add_component_cmd.h"
#include "view/schematic_scene.h"
#include <myschematic/sheet.h>
#include <myschematic/component.h>

AddComponentCmd::AddComponentCmd(SchematicScene* scene, const QPointF& position,
                                 QUndoCommand* parent)
    : QUndoCommand(QObject::tr("Add Component"), parent)
    , m_scene(scene)
    , m_position(position)
{
}

void AddComponentCmd::redo() {
    if (m_firstRedo) {
        // First time: create component in model
        auto* comp = m_scene->sheet()->addComponent(m_position);
        m_elementId = comp->id();
        m_firstRedo = false;
    } else {
        // Re-do: recreate component with same position
        auto* comp = m_scene->sheet()->addComponent(m_position);
        m_elementId = comp->id();
    }
}

void AddComponentCmd::undo() {
    m_scene->sheet()->removeElement(m_elementId);
}
