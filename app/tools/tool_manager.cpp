#include "tools/tool_manager.h"
#include "tools/tool.h"

ToolManager::ToolManager(SchematicScene* scene, QObject* parent)
    : QObject(parent)
    , m_scene(scene)
{
}

ToolManager::~ToolManager() = default;

void ToolManager::registerTool(const QString& name, Tool* tool) {
    m_tools.insert(name, tool);
}

void ToolManager::setActiveTool(const QString& name) {
    if (m_activeToolName == name) return;

    if (m_activeTool) {
        m_activeTool->deactivate();
    }

    m_activeToolName = name;
    m_activeTool = m_tools.value(name, nullptr);

    if (m_activeTool) {
        m_activeTool->activate();
    }

    emit activeToolChanged(name);
}

Tool* ToolManager::activeTool() const {
    return m_activeTool;
}

QString ToolManager::activeToolName() const {
    return m_activeToolName;
}

void ToolManager::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    if (m_activeTool) m_activeTool->mousePressEvent(event);
}

void ToolManager::mouseMoveEvent(QGraphicsSceneMouseEvent* event) {
    if (m_activeTool) m_activeTool->mouseMoveEvent(event);
}

void ToolManager::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
    if (m_activeTool) m_activeTool->mouseReleaseEvent(event);
}

void ToolManager::keyPressEvent(QKeyEvent* event) {
    if (m_activeTool) m_activeTool->keyPressEvent(event);
}
