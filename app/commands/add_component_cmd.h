#pragma once

#include <QUndoCommand>
#include <QPointF>
#include <QString>

class SchematicScene;

class AddComponentCmd : public QUndoCommand {
public:
    AddComponentCmd(SchematicScene* scene, const QPointF& position,
                    QUndoCommand* parent = nullptr);

    void undo() override;
    void redo() override;

    QString elementId() const { return m_elementId; }

private:
    SchematicScene* m_scene;
    QPointF m_position;
    QString m_elementId;
    bool m_firstRedo = true;
};
