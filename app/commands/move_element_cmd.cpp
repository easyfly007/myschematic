#include "commands/move_element_cmd.h"
#include "view/schematic_scene.h"
#include "view/items/component_item.h"
#include <myschematic/sheet.h>
#include <myschematic/schematic_element.h>

MoveElementCmd::MoveElementCmd(SchematicScene* scene, const QString& elementId,
                               const QPointF& oldPos, const QPointF& newPos,
                               QUndoCommand* parent)
    : QUndoCommand(QObject::tr("Move Element"), parent)
    , m_scene(scene)
    , m_elementId(elementId)
    , m_oldPos(oldPos)
    , m_newPos(newPos)
{
}

void MoveElementCmd::undo() {
    if (auto* elem = m_scene->sheet()->elementById(m_elementId)) {
        elem->setPosition(m_oldPos);
    }
    if (auto* item = m_scene->componentItemById(m_elementId)) {
        item->setPos(m_oldPos);
    }
}

void MoveElementCmd::redo() {
    if (auto* elem = m_scene->sheet()->elementById(m_elementId)) {
        elem->setPosition(m_newPos);
    }
    if (auto* item = m_scene->componentItemById(m_elementId)) {
        item->setPos(m_newPos);
    }
}

bool MoveElementCmd::mergeWith(const QUndoCommand* other) {
    if (other->id() != id()) return false;

    auto* moveCmd = static_cast<const MoveElementCmd*>(other);
    if (moveCmd->m_elementId != m_elementId) return false;

    m_newPos = moveCmd->m_newPos;
    return true;
}
