#pragma once

#include <QGraphicsItem>
#include <QString>

class ComponentItem : public QGraphicsItem {
public:
    ComponentItem(const QString& elementId, const QString& instanceName,
                  QGraphicsItem* parent = nullptr);

    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
               QWidget* widget) override;

    QString elementId() const { return m_elementId; }
    void setElementId(const QString& id) { m_elementId = id; }

    QString instanceName() const { return m_instanceName; }
    void setInstanceName(const QString& name);

    static constexpr double kWidth = 60.0;
    static constexpr double kHeight = 40.0;

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;

private:
    QString m_elementId;
    QString m_instanceName;
};
