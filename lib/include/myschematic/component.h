#pragma once

#include "schematic_element.h"
#include "property.h"

#include <QList>

namespace myschematic {

class SymbolDef;
class Pin;

class Component : public SchematicElement {
    Q_OBJECT

public:
    explicit Component(QObject* parent = nullptr);
    explicit Component(const SymbolDef* symbolDef, QObject* parent = nullptr);
    ~Component() override;

    QRectF boundingRect() const override;
    ElementType type() const override;

    QString instanceName() const;
    void setInstanceName(const QString& name);

    // Symbol
    const SymbolDef* symbolDef() const;
    void setSymbolDef(const SymbolDef* def);

    // Pins (only available when symbolDef is set)
    const QList<Pin*>& pins() const;
    Pin* pinByName(const QString& name) const;
    QPointF pinWorldPosition(const QString& pinName) const;

    // Properties
    PropertyBag& properties();
    const PropertyBag& properties() const;
    void setProperty(const QString& name, const QString& value);
    QString property(const QString& name) const;

private:
    void rebuildPins();

    QString m_instanceName;
    const SymbolDef* m_symbolDef = nullptr;
    QList<Pin*> m_pins;
    PropertyBag m_properties;

    // Phase 1: fixed bounding rect 60x40 (fallback when no symbolDef)
    static constexpr double kWidth = 60.0;
    static constexpr double kHeight = 40.0;
};

} // namespace myschematic
