#include "tools/tool.h"

Tool::Tool(SchematicScene* scene, QObject* parent)
    : QObject(parent)
    , m_scene(scene)
{
}

Tool::~Tool() = default;

void Tool::mousePressEvent(QGraphicsSceneMouseEvent* /*event*/) {}
void Tool::mouseMoveEvent(QGraphicsSceneMouseEvent* /*event*/) {}
void Tool::mouseReleaseEvent(QGraphicsSceneMouseEvent* /*event*/) {}
void Tool::keyPressEvent(QKeyEvent* /*event*/) {}
