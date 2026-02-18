#pragma once

#include <QUndoCommand>

class SchematicScene;

namespace myschematic {
class SchematicElement;
}

class MirrorElementCmd : public QUndoCommand {
public:
    MirrorElementCmd(myschematic::SchematicElement* element, SchematicScene* scene,
                     QUndoCommand* parent = nullptr);

    void undo() override;
    void redo() override;

private:
    myschematic::SchematicElement* m_element;
    SchematicScene* m_scene;
    bool m_oldMirrored;
};
