#pragma once

#include <QGraphicsScene>
#include <QMap>
#include <QString>

class QUndoStack;
class ToolManager;
class ComponentItem;

namespace myschematic {
class Sheet;
class SchematicElement;
class Component;
}

class SchematicScene : public QGraphicsScene {
    Q_OBJECT

public:
    explicit SchematicScene(QObject* parent = nullptr);
    ~SchematicScene() override;

    void setSheet(myschematic::Sheet* sheet);
    myschematic::Sheet* sheet() const { return m_sheet; }

    QUndoStack* undoStack() const { return m_undoStack; }
    ToolManager* toolManager() const { return m_toolManager; }

    ComponentItem* componentItemById(const QString& elementId) const;

    // Called by commands
    void addComponentFromModel(myschematic::Component* comp);
    void removeComponentFromModel(const QString& elementId);
    void deleteSelectedElements();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private slots:
    void onElementAdded(myschematic::SchematicElement* element);
    void onElementRemoved(const QString& elementId);

private:
    myschematic::Sheet* m_sheet = nullptr;
    QUndoStack* m_undoStack;
    ToolManager* m_toolManager;
    QMap<QString, ComponentItem*> m_componentItems;
};
