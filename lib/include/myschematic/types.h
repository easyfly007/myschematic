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

} // namespace myschematic
