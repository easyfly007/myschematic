#pragma once

#include "schematic_element.h"

namespace myschematic {

class Component : public SchematicElement {
    Q_OBJECT

public:
    explicit Component(QObject* parent = nullptr);
    ~Component() override;

    QRectF boundingRect() const override;
    ElementType type() const override;

    QString instanceName() const;
    void setInstanceName(const QString& name);

private:
    QString m_instanceName;
    // Phase 1: fixed bounding rect 60x40
    static constexpr double kWidth = 60.0;
    static constexpr double kHeight = 40.0;
};

} // namespace myschematic
