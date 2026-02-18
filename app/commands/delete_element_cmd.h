#pragma once

#include <QUndoCommand>
#include <QPointF>
#include <QString>

class SchematicScene;

class DeleteElementCmd : public QUndoCommand {
public:
    DeleteElementCmd(SchematicScene* scene, const QString& elementId,
                     QUndoCommand* parent = nullptr);

    void undo() override;
    void redo() override;

private:
    SchematicScene* m_scene;
    QString m_elementId;
    QPointF m_position;
    QString m_instanceName;
    int m_rotation = 0;
    bool m_mirrored = false;
};
