#pragma once

#include <QUndoCommand>
#include <QString>

class SchematicScene;

namespace myschematic {
class Component;
}

class ChangePropertyCmd : public QUndoCommand {
public:
    ChangePropertyCmd(myschematic::Component* component, const QString& propertyName,
                      const QString& newValue, SchematicScene* scene,
                      QUndoCommand* parent = nullptr);

    void undo() override;
    void redo() override;

private:
    myschematic::Component* m_component;
    SchematicScene* m_scene;
    QString m_propertyName;
    QString m_oldValue;
    QString m_newValue;
};
