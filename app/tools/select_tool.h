#pragma once

#include "tools/tool.h"
#include <QPointF>
#include <QList>

class QGraphicsItem;
class ComponentItem;

class SelectTool : public Tool {
    Q_OBJECT

public:
    explicit SelectTool(SchematicScene* scene, QObject* parent = nullptr);

    QString name() const override { return QStringLiteral("select"); }

    void activate() override;
    void deactivate() override;

    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private:
    enum class State { Idle, Dragging, RubberBand };
    State m_state = State::Idle;
    QPointF m_dragStartScene;
    QMap<QString, QPointF> m_dragStartPositions; // element id -> start pos
};
