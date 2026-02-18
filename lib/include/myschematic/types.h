#pragma once

#include <QString>

namespace myschematic {

enum class ElementType {
    Component,
    Wire,
    Label,
    Port,
    PowerSymbol,
    Junction
};

enum class PortDirection {
    Input,
    Output,
    InOut
};

enum class PinDirection {
    Left,
    Right,
    Up,
    Down
};

enum class ElectricalType {
    Input,
    Output,
    Passive,
    Power,
    Bidirectional,
    NotConnected
};

} // namespace myschematic
