#include "tools/select_tool.h"
#include "view/schematic_scene.h"
#include "view/items/component_item.h"
#include "commands/move_element_cmd.h"
#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>
#include <QUndoStack>
#include <cmath>

SelectTool::SelectTool(SchematicScene* scene, QObject* parent)
    : Tool(scene, parent)
{
}

void SelectTool::activate() {
    m_state = State::Idle;
}

void SelectTool::deactivate() {
    m_state = State::Idle;
}

void SelectTool::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    if (event->button() != Qt::LeftButton) return;

    QPointF scenePos = event->scenePos();
    QGraphicsItem* item = m_scene->itemAt(scenePos, QTransform());

    if (auto* compItem = dynamic_cast<ComponentItem*>(item)) {
        // Clicking on a component
        bool shiftHeld = (event->modifiers() & Qt::ShiftModifier);
        if (!shiftHeld && !compItem->isSelected()) {
            m_scene->clearSelection();
        }
        compItem->setSelected(true);

        // Prepare for dragging
        m_state = State::Dragging;
        m_dragStartScene = scenePos;
        m_dragStartPositions.clear();
        for (auto* selItem : m_scene->selectedItems()) {
            if (auto* ci = dynamic_cast<ComponentItem*>(selItem)) {
                m_dragStartPositions.insert(ci->elementId(), ci->pos());
            }
        }
    } else {
        // Clicking on empty space - start rubber band or deselect
        if (!(event->modifiers() & Qt::ShiftModifier)) {
            m_scene->clearSelection();
        }
        m_state = State::RubberBand;
        m_dragStartScene = scenePos;
    }
}

void SelectTool::mouseMoveEvent(QGraphicsSceneMouseEvent* event) {
    if (m_state == State::Dragging) {
        QPointF delta = event->scenePos() - m_dragStartScene;
        for (auto* item : m_scene->selectedItems()) {
            if (auto* ci = dynamic_cast<ComponentItem*>(item)) {
                QPointF startPos = m_dragStartPositions.value(ci->elementId());
                ci->setPos(startPos + delta);
            }
        }
    }
}

void SelectTool::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
    if (event->button() != Qt::LeftButton) return;

    if (m_state == State::Dragging) {
        QPointF delta = event->scenePos() - m_dragStartScene;
        if (std::abs(delta.x()) > 0.5 || std::abs(delta.y()) > 0.5) {
            // Create move command for each selected item
            for (auto* item : m_scene->selectedItems()) {
                if (auto* ci = dynamic_cast<ComponentItem*>(item)) {
                    QPointF oldPos = m_dragStartPositions.value(ci->elementId());
                    QPointF newPos = ci->pos();
                    if (oldPos != newPos) {
                        auto* cmd = new MoveElementCmd(m_scene, ci->elementId(), oldPos, newPos);
                        m_scene->undoStack()->push(cmd);
                    }
                }
            }
        }
    } else if (m_state == State::RubberBand) {
        QRectF selRect = QRectF(m_dragStartScene, event->scenePos()).normalized();
        if (selRect.width() > 1.0 || selRect.height() > 1.0) {
            QPainterPath path;
            path.addRect(selRect);
            m_scene->setSelectionArea(path);
        }
    }

    m_state = State::Idle;
    m_dragStartPositions.clear();
}

void SelectTool::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Delete) {
        m_scene->deleteSelectedElements();
    }
}
