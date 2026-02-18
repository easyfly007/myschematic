#include "tools/place_tool.h"
#include "view/schematic_scene.h"
#include "view/items/component_item.h"
#include "commands/add_component_cmd.h"
#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>
#include <QUndoStack>

PlaceTool::PlaceTool(SchematicScene* scene, QObject* parent)
    : Tool(scene, parent)
{
}

void PlaceTool::activate() {
    if (!m_preview) {
        m_preview = new ComponentItem(QString(), QStringLiteral("New"));
        m_preview->setOpacity(0.5);
        m_scene->addItem(m_preview);
    }
}

void PlaceTool::deactivate() {
    if (m_preview) {
        m_scene->removeItem(m_preview);
        delete m_preview;
        m_preview = nullptr;
    }
}

void PlaceTool::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    if (event->button() != Qt::LeftButton) return;

    QPointF pos = event->scenePos();
    // Snap to grid (10px)
    double gridSize = 10.0;
    pos.setX(qRound(pos.x() / gridSize) * gridSize);
    pos.setY(qRound(pos.y() / gridSize) * gridSize);

    auto* cmd = new AddComponentCmd(m_scene, pos);
    m_scene->undoStack()->push(cmd);
}

void PlaceTool::mouseMoveEvent(QGraphicsSceneMouseEvent* event) {
    if (m_preview) {
        QPointF pos = event->scenePos();
        double gridSize = 10.0;
        pos.setX(qRound(pos.x() / gridSize) * gridSize);
        pos.setY(qRound(pos.y() / gridSize) * gridSize);
        m_preview->setPos(pos);
    }
}

void PlaceTool::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Escape) {
        emit placementFinished();
    }
}
