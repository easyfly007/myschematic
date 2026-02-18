#include "commands/rotate_element_cmd.h"
#include "view/schematic_scene.h"
#include "view/items/component_item.h"
#include <myschematic/schematic_element.h>

RotateElementCmd::RotateElementCmd(myschematic::SchematicElement* element, int degrees,
                                   SchematicScene* scene, QUndoCommand* parent)
    : QUndoCommand(parent)
    , m_element(element)
    , m_scene(scene)
    , m_oldRotation(element->rotation())
    , m_newRotation((element->rotation() + degrees) % 360)
{
    // Normalize to [0, 360)
    if (m_newRotation < 0)
        m_newRotation += 360;

    setText(QStringLiteral("Rotate element"));
}

void RotateElementCmd::undo() {
    m_element->setRotation(m_oldRotation);

    // Update the visual item
    if (auto* item = m_scene->componentItemById(m_element->id())) {
        item->setRotation(m_oldRotation);
        item->update();
    }
}

void RotateElementCmd::redo() {
    m_element->setRotation(m_newRotation);

    // Update the visual item
    if (auto* item = m_scene->componentItemById(m_element->id())) {
        item->setRotation(m_newRotation);
        item->update();
    }
}
