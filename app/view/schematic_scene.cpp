#include "view/schematic_scene.h"
#include "view/items/component_item.h"
#include "tools/tool_manager.h"
#include "tools/select_tool.h"
#include "tools/place_tool.h"
#include "commands/delete_element_cmd.h"
#include <myschematic/sheet.h>
#include <myschematic/component.h>
#include <QUndoStack>
#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>

SchematicScene::SchematicScene(QObject* parent)
    : QGraphicsScene(parent)
    , m_undoStack(new QUndoStack(this))
    , m_toolManager(new ToolManager(this, this))
{
    setSceneRect(-5000, -5000, 10000, 10000);

    // Register tools
    auto* selectTool = new SelectTool(this, this);
    auto* placeTool = new PlaceTool(this, this);

    m_toolManager->registerTool(QStringLiteral("select"), selectTool);
    m_toolManager->registerTool(QStringLiteral("place"), placeTool);

    connect(placeTool, &PlaceTool::placementFinished, this, [this]() {
        m_toolManager->setActiveTool(QStringLiteral("select"));
    });

    m_toolManager->setActiveTool(QStringLiteral("select"));
}

SchematicScene::~SchematicScene() = default;

void SchematicScene::setSheet(myschematic::Sheet* sheet) {
    // Disconnect old sheet
    if (m_sheet) {
        disconnect(m_sheet, nullptr, this, nullptr);
    }

    // Clear existing items
    for (auto* item : m_componentItems) {
        removeItem(item);
        delete item;
    }
    m_componentItems.clear();

    m_sheet = sheet;

    if (m_sheet) {
        connect(m_sheet, &myschematic::Sheet::elementAdded,
                this, &SchematicScene::onElementAdded);
        connect(m_sheet, &myschematic::Sheet::elementRemoved,
                this, &SchematicScene::onElementRemoved);

        // Add existing elements
        for (auto* comp : m_sheet->components()) {
            onElementAdded(comp);
        }
    }
}

ComponentItem* SchematicScene::componentItemById(const QString& elementId) const {
    return m_componentItems.value(elementId, nullptr);
}

void SchematicScene::addComponentFromModel(myschematic::Component* comp) {
    auto* item = new ComponentItem(comp->id(), comp->instanceName());
    item->setPos(comp->position());
    addItem(item);
    m_componentItems.insert(comp->id(), item);
}

void SchematicScene::removeComponentFromModel(const QString& elementId) {
    if (auto* item = m_componentItems.value(elementId)) {
        removeItem(item);
        m_componentItems.remove(elementId);
        delete item;
    }
}

void SchematicScene::deleteSelectedElements() {
    auto selected = selectedItems();
    for (auto* item : selected) {
        if (auto* ci = dynamic_cast<ComponentItem*>(item)) {
            auto* cmd = new DeleteElementCmd(this, ci->elementId());
            m_undoStack->push(cmd);
        }
    }
}

void SchematicScene::onElementAdded(myschematic::SchematicElement* element) {
    if (auto* comp = qobject_cast<myschematic::Component*>(element)) {
        if (!m_componentItems.contains(comp->id())) {
            addComponentFromModel(comp);
        }
    }
}

void SchematicScene::onElementRemoved(const QString& elementId) {
    removeComponentFromModel(elementId);
}

void SchematicScene::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    if (event->button() == Qt::MiddleButton) {
        // Middle button handled by view for panning
        event->ignore();
        return;
    }
    m_toolManager->mousePressEvent(event);
}

void SchematicScene::mouseMoveEvent(QGraphicsSceneMouseEvent* event) {
    m_toolManager->mouseMoveEvent(event);
}

void SchematicScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
    m_toolManager->mouseReleaseEvent(event);
}

void SchematicScene::keyPressEvent(QKeyEvent* event) {
    m_toolManager->keyPressEvent(event);
}
