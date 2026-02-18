#pragma once

#include <QObject>

class QGraphicsSceneMouseEvent;
class QKeyEvent;

namespace myschematic {
class SchematicElement;
}

class SchematicScene;

class Tool : public QObject {
    Q_OBJECT

public:
    explicit Tool(SchematicScene* scene, QObject* parent = nullptr);
    ~Tool() override;

    virtual QString name() const = 0;

    virtual void activate() {}
    virtual void deactivate() {}

    virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
    virtual void keyPressEvent(QKeyEvent* event);

protected:
    SchematicScene* m_scene;
};
