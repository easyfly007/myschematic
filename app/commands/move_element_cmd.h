#pragma once

#include <QUndoCommand>
#include <QPointF>
#include <QString>

class SchematicScene;

class MoveElementCmd : public QUndoCommand {
public:
    enum { Id = 1001 };

    MoveElementCmd(SchematicScene* scene, const QString& elementId,
                   const QPointF& oldPos, const QPointF& newPos,
                   QUndoCommand* parent = nullptr);

    void undo() override;
    void redo() override;
    int id() const override { return Id; }
    bool mergeWith(const QUndoCommand* other) override;

private:
    SchematicScene* m_scene;
    QString m_elementId;
    QPointF m_oldPos;
    QPointF m_newPos;
};
