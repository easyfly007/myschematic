#pragma once

#include <QPointF>
#include <QString>
#include "types.h"

namespace myschematic {

struct SymbolPin {
    QString name;
    int pinNumber = 0;
    QPointF offset;  // relative to symbol origin
    PinDirection direction = PinDirection::Left;
    ElectricalType electricalType = ElectricalType::Passive;
};

} // namespace myschematic
