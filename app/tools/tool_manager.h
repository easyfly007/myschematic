#pragma once

#include <QObject>
#include <QMap>
#include <QString>

class Tool;
class SchematicScene;
class QGraphicsSceneMouseEvent;
class QKeyEvent;

class ToolManager : public QObject {
    Q_OBJECT

public:
    explicit ToolManager(SchematicScene* scene, QObject* parent = nullptr);
    ~ToolManager() override;

    void registerTool(const QString& name, Tool* tool);
    void setActiveTool(const QString& name);
    Tool* activeTool() const;
    QString activeToolName() const;

    void mousePressEvent(QGraphicsSceneMouseEvent* event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
    void keyPressEvent(QKeyEvent* event);

signals:
    void activeToolChanged(const QString& toolName);

private:
    SchematicScene* m_scene;
    QMap<QString, Tool*> m_tools;
    Tool* m_activeTool = nullptr;
    QString m_activeToolName;
};
