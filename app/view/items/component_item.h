#pragma once

#include <QGraphicsItem>
#include <QString>

namespace myschematic {
class Component;
class SymbolDef;
class SymbolGraphic;
}

class ComponentItem : public QGraphicsItem {
public:
    ComponentItem(myschematic::Component* component, QGraphicsItem* parent = nullptr);

    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
               QWidget* widget) override;

    QString elementId() const { return m_elementId; }
    void setElementId(const QString& id) { m_elementId = id; }

    QString instanceName() const { return m_instanceName; }
    void setInstanceName(const QString& name);

    myschematic::Component* component() const { return m_component; }

    static constexpr double kWidth = 60.0;
    static constexpr double kHeight = 40.0;

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;

private:
    void drawGraphic(QPainter* painter, const myschematic::SymbolGraphic* graphic);
    void drawPins(QPainter* painter, const myschematic::SymbolDef* sym);
    void drawFallbackRect(QPainter* painter);
    void drawSelectionHandles(QPainter* painter);

    myschematic::Component* m_component;
    QString m_elementId;
    QString m_instanceName;
};
