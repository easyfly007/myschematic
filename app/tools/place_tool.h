#pragma once

#include "tools/tool.h"
#include <QPointF>

class ComponentItem;

class PlaceTool : public Tool {
    Q_OBJECT

public:
    explicit PlaceTool(SchematicScene* scene, QObject* parent = nullptr);

    QString name() const override { return QStringLiteral("place"); }

    void activate() override;
    void deactivate() override;

    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

signals:
    void placementFinished();

private:
    ComponentItem* m_preview = nullptr;
};
