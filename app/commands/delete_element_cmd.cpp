#include "commands/delete_element_cmd.h"
#include "view/schematic_scene.h"
#include <myschematic/sheet.h>
#include <myschematic/component.h>

DeleteElementCmd::DeleteElementCmd(SchematicScene* scene, const QString& elementId,
                                   QUndoCommand* parent)
    : QUndoCommand(QObject::tr("Delete Element"), parent)
    , m_scene(scene)
    , m_elementId(elementId)
{
    // Save element state for undo
    if (auto* comp = scene->sheet()->componentById(elementId)) {
        m_position = comp->position();
        m_instanceName = comp->instanceName();
        m_rotation = comp->rotation();
        m_mirrored = comp->isMirrored();
    }
}

void DeleteElementCmd::redo() {
    m_scene->sheet()->removeElement(m_elementId);
}

void DeleteElementCmd::undo() {
    auto* comp = m_scene->sheet()->addComponent(m_position);
    // The new component gets a new ID; we need to update our tracking
    m_elementId = comp->id();
    comp->setInstanceName(m_instanceName);
    comp->setRotation(m_rotation);
    comp->setMirrored(m_mirrored);
}
