#pragma once

#include <QUndoCommand>

class SchematicScene;

namespace myschematic {
class SchematicElement;
}

class RotateElementCmd : public QUndoCommand {
public:
    RotateElementCmd(myschematic::SchematicElement* element, int degrees,
                     SchematicScene* scene, QUndoCommand* parent = nullptr);

    void undo() override;
    void redo() override;

private:
    myschematic::SchematicElement* m_element;
    SchematicScene* m_scene;
    int m_oldRotation;
    int m_newRotation;
};
