#pragma once

#include <QObject>
#include <QList>
#include <QPointF>
#include <QRectF>
#include <QString>

namespace myschematic {

class SchematicElement;
class Component;

class Sheet : public QObject {
    Q_OBJECT

public:
    explicit Sheet(const QString& name, QObject* parent = nullptr);
    ~Sheet() override;

    QString id() const;
    QString name() const;

    // Component operations
    Component* addComponent(const QPointF& pos);
    void removeElement(const QString& id);
    SchematicElement* elementById(const QString& id) const;
    Component* componentById(const QString& id) const;
    QList<Component*> components() const;
    QList<SchematicElement*> elements() const;

    // Spatial queries
    QList<SchematicElement*> elementsAt(const QPointF& pos, qreal radius = 5.0) const;
    QList<SchematicElement*> elementsInRect(const QRectF& rect) const;

signals:
    void elementAdded(SchematicElement* element);
    void elementRemoved(const QString& elementId);
    void elementChanged(SchematicElement* element);

private:
    QString m_id;
    QString m_name;
    QList<SchematicElement*> m_elements;
};

} // namespace myschematic
