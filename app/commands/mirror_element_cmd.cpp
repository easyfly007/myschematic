#include "commands/mirror_element_cmd.h"
#include "view/schematic_scene.h"
#include "view/items/component_item.h"
#include <myschematic/schematic_element.h>

MirrorElementCmd::MirrorElementCmd(myschematic::SchematicElement* element,
                                   SchematicScene* scene, QUndoCommand* parent)
    : QUndoCommand(parent)
    , m_element(element)
    , m_scene(scene)
    , m_oldMirrored(element->isMirrored())
{
    setText(QStringLiteral("Mirror element"));
}

void MirrorElementCmd::undo() {
    m_element->setMirrored(m_oldMirrored);

    // Update the visual item
    if (auto* item = m_scene->componentItemById(m_element->id())) {
        item->update();
    }
}

void MirrorElementCmd::redo() {
    m_element->setMirrored(!m_oldMirrored);

    // Update the visual item
    if (auto* item = m_scene->componentItemById(m_element->id())) {
        item->update();
    }
}
