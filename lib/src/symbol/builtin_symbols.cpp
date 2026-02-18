#include "myschematic/builtin_symbols.h"

#include "myschematic/symbol_def.h"
#include "myschematic/symbol_graphic.h"
#include "myschematic/symbol_library.h"
#include "myschematic/symbol_pin.h"
#include "myschematic/property.h"

namespace myschematic {

// ---------------------------------------------------------------------------
// Helper to create a Property template
// ---------------------------------------------------------------------------
static Property makePropTemplate(const QString& name, const QString& defaultValue,
                                 bool required, bool visible)
{
    Property p;
    p.name = name;
    p.defaultValue = defaultValue;
    p.value = defaultValue;
    p.required = required;
    p.visible = visible;
    return p;
}

// ---------------------------------------------------------------------------
// Helper to create a SymbolPin
// ---------------------------------------------------------------------------
static SymbolPin makePin(const QString& name, int pinNumber, double x, double y,
                         PinDirection dir, ElectricalType elec)
{
    SymbolPin pin;
    pin.name = name;
    pin.pinNumber = pinNumber;
    pin.offset = QPointF(x, y);
    pin.direction = dir;
    pin.electricalType = elec;
    return pin;
}

// ---------------------------------------------------------------------------
// Helper to create a controlled source (diamond shape, 4 pins)
// ---------------------------------------------------------------------------
static std::shared_ptr<SymbolDef> createControlledSource(
    const QString& name, const QString& propName, const QString& propDefault,
    const QString& spiceLetter)
{
    auto sym = std::make_shared<SymbolDef>(name);
    sym->setCategory(QStringLiteral("Sources"));

    // Pins
    sym->addPin(makePin(QStringLiteral("NP"), 1, 15, -10, PinDirection::Right, ElectricalType::Output));
    sym->addPin(makePin(QStringLiteral("NN"), 2, 15, 10, PinDirection::Right, ElectricalType::Output));
    sym->addPin(makePin(QStringLiteral("CP"), 3, -15, -10, PinDirection::Left, ElectricalType::Input));
    sym->addPin(makePin(QStringLiteral("CN"), 4, -15, 10, PinDirection::Left, ElectricalType::Input));

    // Diamond shape
    sym->addGraphic(std::make_shared<GraphicPolyline>(
        QList<QPointF>{{0, -15}, {15, 0}, {0, 15}, {-15, 0}, {0, -15}}, false));

    // Property
    sym->addPropertyTemplate(makePropTemplate(propName, propDefault, true, true));

    // SPICE template
    sym->setSpiceTemplate(
        spiceLetter + QStringLiteral("{instanceName} {NP} {NN} {CP} {CN} {")
        + propName + QStringLiteral("}"));

    return sym;
}

// ===========================================================================
// NMOS4
// ===========================================================================
std::shared_ptr<SymbolDef> BuiltinSymbols::createNmos4()
{
    auto sym = std::make_shared<SymbolDef>(QStringLiteral("nmos4"));
    sym->setDescription(QStringLiteral("N-Channel MOSFET (4-terminal)"));
    sym->setCategory(QStringLiteral("Transistors"));

    // Pins
    sym->addPin(makePin(QStringLiteral("G"), 1, -30, 0, PinDirection::Left, ElectricalType::Input));
    sym->addPin(makePin(QStringLiteral("D"), 2, 0, -20, PinDirection::Up, ElectricalType::Passive));
    sym->addPin(makePin(QStringLiteral("S"), 3, 0, 20, PinDirection::Down, ElectricalType::Passive));
    sym->addPin(makePin(QStringLiteral("B"), 4, 10, 0, PinDirection::Right, ElectricalType::Passive));

    // Graphics
    sym->addGraphic(std::make_shared<GraphicLine>(-30, 0, -10, 0));   // gate horizontal
    sym->addGraphic(std::make_shared<GraphicLine>(-10, -15, -10, 15)); // gate vertical bar
    sym->addGraphic(std::make_shared<GraphicLine>(-5, -15, -5, -5));   // channel segment top
    sym->addGraphic(std::make_shared<GraphicLine>(-5, -10, 0, -10));   // drain horizontal
    sym->addGraphic(std::make_shared<GraphicLine>(0, -20, 0, -10));    // drain vertical to pin
    sym->addGraphic(std::make_shared<GraphicLine>(-5, 5, -5, 15));     // channel segment bottom
    sym->addGraphic(std::make_shared<GraphicLine>(-5, 10, 0, 10));     // source horizontal
    sym->addGraphic(std::make_shared<GraphicLine>(0, 10, 0, 20));      // source vertical to pin
    sym->addGraphic(std::make_shared<GraphicLine>(-5, 0, 0, 0));       // channel center
    sym->addGraphic(std::make_shared<GraphicLine>(0, 0, 10, 0));       // bulk horizontal to pin

    // Arrow pointing right (N-channel)
    sym->addGraphic(std::make_shared<GraphicPolyline>(
        QList<QPointF>{{-5, -2}, {-2, 0}, {-5, 2}}, true));

    // Properties
    sym->addPropertyTemplate(makePropTemplate(QStringLiteral("model"), QStringLiteral("nch"), true, true));
    sym->addPropertyTemplate(makePropTemplate(QStringLiteral("W"), QStringLiteral("1u"), true, true));
    sym->addPropertyTemplate(makePropTemplate(QStringLiteral("L"), QStringLiteral("100n"), true, true));
    sym->addPropertyTemplate(makePropTemplate(QStringLiteral("nf"), QStringLiteral("1"), false, false));
    sym->addPropertyTemplate(makePropTemplate(QStringLiteral("m"), QStringLiteral("1"), false, false));

    // SPICE
    sym->setSpiceTemplate(QStringLiteral(
        "M{instanceName} {D} {G} {S} {B} {model} W={W} L={L} nf={nf} m={m}"));

    return sym;
}

// ===========================================================================
// PMOS4
// ===========================================================================
std::shared_ptr<SymbolDef> BuiltinSymbols::createPmos4()
{
    auto sym = std::make_shared<SymbolDef>(QStringLiteral("pmos4"));
    sym->setDescription(QStringLiteral("P-Channel MOSFET (4-terminal)"));
    sym->setCategory(QStringLiteral("Transistors"));

    // Pins
    sym->addPin(makePin(QStringLiteral("G"), 1, -30, 0, PinDirection::Left, ElectricalType::Input));
    sym->addPin(makePin(QStringLiteral("D"), 2, 0, -20, PinDirection::Up, ElectricalType::Passive));
    sym->addPin(makePin(QStringLiteral("S"), 3, 0, 20, PinDirection::Down, ElectricalType::Passive));
    sym->addPin(makePin(QStringLiteral("B"), 4, 10, 0, PinDirection::Right, ElectricalType::Passive));

    // Graphics
    sym->addGraphic(std::make_shared<GraphicLine>(-30, 0, -10, 0));   // gate horizontal
    sym->addGraphic(std::make_shared<GraphicLine>(-10, -15, -10, 15)); // gate vertical bar
    sym->addGraphic(std::make_shared<GraphicLine>(-5, -15, -5, -5));   // channel segment top
    sym->addGraphic(std::make_shared<GraphicLine>(-5, -10, 0, -10));   // drain horizontal
    sym->addGraphic(std::make_shared<GraphicLine>(0, -20, 0, -10));    // drain vertical to pin
    sym->addGraphic(std::make_shared<GraphicLine>(-5, 5, -5, 15));     // channel segment bottom
    sym->addGraphic(std::make_shared<GraphicLine>(-5, 10, 0, 10));     // source horizontal
    sym->addGraphic(std::make_shared<GraphicLine>(0, 10, 0, 20));      // source vertical to pin
    sym->addGraphic(std::make_shared<GraphicLine>(-5, 0, 0, 0));       // channel center
    sym->addGraphic(std::make_shared<GraphicLine>(0, 0, 10, 0));       // bulk horizontal to pin

    // Arrow pointing left (P-channel, reversed from N-channel)
    sym->addGraphic(std::make_shared<GraphicPolyline>(
        QList<QPointF>{{-2, -2}, {-5, 0}, {-2, 2}}, true));

    // Circle (bubble) on gate
    sym->addGraphic(std::make_shared<GraphicCircle>(-8, 0, 2));

    // Properties
    sym->addPropertyTemplate(makePropTemplate(QStringLiteral("model"), QStringLiteral("pch"), true, true));
    sym->addPropertyTemplate(makePropTemplate(QStringLiteral("W"), QStringLiteral("1u"), true, true));
    sym->addPropertyTemplate(makePropTemplate(QStringLiteral("L"), QStringLiteral("100n"), true, true));
    sym->addPropertyTemplate(makePropTemplate(QStringLiteral("nf"), QStringLiteral("1"), false, false));
    sym->addPropertyTemplate(makePropTemplate(QStringLiteral("m"), QStringLiteral("1"), false, false));

    // SPICE
    sym->setSpiceTemplate(QStringLiteral(
        "M{instanceName} {D} {G} {S} {B} {model} W={W} L={L} nf={nf} m={m}"));

    return sym;
}

// ===========================================================================
// Resistor
// ===========================================================================
std::shared_ptr<SymbolDef> BuiltinSymbols::createResistor()
{
    auto sym = std::make_shared<SymbolDef>(QStringLiteral("resistor"));
    sym->setDescription(QStringLiteral("Resistor"));
    sym->setCategory(QStringLiteral("Passives"));

    // Pins
    sym->addPin(makePin(QStringLiteral("P"), 1, 0, -20, PinDirection::Up, ElectricalType::Passive));
    sym->addPin(makePin(QStringLiteral("N"), 2, 0, 20, PinDirection::Down, ElectricalType::Passive));

    // Graphics (zigzag)
    sym->addGraphic(std::make_shared<GraphicLine>(0, -20, 0, -15));
    sym->addGraphic(std::make_shared<GraphicLine>(0, -15, 5, -12));
    sym->addGraphic(std::make_shared<GraphicLine>(5, -12, -5, -6));
    sym->addGraphic(std::make_shared<GraphicLine>(-5, -6, 5, 0));
    sym->addGraphic(std::make_shared<GraphicLine>(5, 0, -5, 6));
    sym->addGraphic(std::make_shared<GraphicLine>(-5, 6, 5, 12));
    sym->addGraphic(std::make_shared<GraphicLine>(5, 12, 0, 15));
    sym->addGraphic(std::make_shared<GraphicLine>(0, 15, 0, 20));

    // Properties
    sym->addPropertyTemplate(makePropTemplate(QStringLiteral("value"), QStringLiteral("1k"), true, true));

    // SPICE
    sym->setSpiceTemplate(QStringLiteral("R{instanceName} {P} {N} {value}"));

    return sym;
}

// ===========================================================================
// Capacitor
// ===========================================================================
std::shared_ptr<SymbolDef> BuiltinSymbols::createCapacitor()
{
    auto sym = std::make_shared<SymbolDef>(QStringLiteral("capacitor"));
    sym->setDescription(QStringLiteral("Capacitor"));
    sym->setCategory(QStringLiteral("Passives"));

    // Pins
    sym->addPin(makePin(QStringLiteral("P"), 1, 0, -15, PinDirection::Up, ElectricalType::Passive));
    sym->addPin(makePin(QStringLiteral("N"), 2, 0, 15, PinDirection::Down, ElectricalType::Passive));

    // Graphics
    sym->addGraphic(std::make_shared<GraphicLine>(0, -15, 0, -3));    // top lead
    sym->addGraphic(std::make_shared<GraphicLine>(-8, -3, 8, -3));    // top plate
    sym->addGraphic(std::make_shared<GraphicLine>(-8, 3, 8, 3));      // bottom plate
    sym->addGraphic(std::make_shared<GraphicLine>(0, 3, 0, 15));      // bottom lead

    // Properties
    sym->addPropertyTemplate(makePropTemplate(QStringLiteral("value"), QStringLiteral("1p"), true, true));

    // SPICE
    sym->setSpiceTemplate(QStringLiteral("C{instanceName} {P} {N} {value}"));

    return sym;
}

// ===========================================================================
// Inductor
// ===========================================================================
std::shared_ptr<SymbolDef> BuiltinSymbols::createInductor()
{
    auto sym = std::make_shared<SymbolDef>(QStringLiteral("inductor"));
    sym->setDescription(QStringLiteral("Inductor"));
    sym->setCategory(QStringLiteral("Passives"));

    // Pins
    sym->addPin(makePin(QStringLiteral("P"), 1, 0, -20, PinDirection::Up, ElectricalType::Passive));
    sym->addPin(makePin(QStringLiteral("N"), 2, 0, 20, PinDirection::Down, ElectricalType::Passive));

    // Graphics (coil arcs)
    sym->addGraphic(std::make_shared<GraphicLine>(0, -20, 0, -12));
    sym->addGraphic(std::make_shared<GraphicArc>(0, -8, 4, 90, 180));   // first bump
    sym->addGraphic(std::make_shared<GraphicArc>(0, 0, 4, 90, 180));    // second bump
    sym->addGraphic(std::make_shared<GraphicArc>(0, 8, 4, 90, 180));    // third bump
    sym->addGraphic(std::make_shared<GraphicLine>(0, 12, 0, 20));

    // Properties
    sym->addPropertyTemplate(makePropTemplate(QStringLiteral("value"), QStringLiteral("10n"), true, true));

    // SPICE
    sym->setSpiceTemplate(QStringLiteral("L{instanceName} {P} {N} {value}"));

    return sym;
}

// ===========================================================================
// Diode
// ===========================================================================
std::shared_ptr<SymbolDef> BuiltinSymbols::createDiode()
{
    auto sym = std::make_shared<SymbolDef>(QStringLiteral("diode"));
    sym->setDescription(QStringLiteral("Diode"));
    sym->setCategory(QStringLiteral("Semiconductors"));

    // Pins
    sym->addPin(makePin(QStringLiteral("A"), 1, 0, -15, PinDirection::Up, ElectricalType::Passive));
    sym->addPin(makePin(QStringLiteral("K"), 2, 0, 15, PinDirection::Down, ElectricalType::Passive));

    // Graphics
    sym->addGraphic(std::make_shared<GraphicLine>(0, -15, 0, -5));     // anode lead
    sym->addGraphic(std::make_shared<GraphicPolyline>(
        QList<QPointF>{{-7, -5}, {7, -5}, {0, 5}}, true));            // triangle pointing down
    sym->addGraphic(std::make_shared<GraphicLine>(-7, 5, 7, 5));      // cathode bar
    sym->addGraphic(std::make_shared<GraphicLine>(0, 5, 0, 15));      // cathode lead

    // Properties
    sym->addPropertyTemplate(makePropTemplate(QStringLiteral("model"), QStringLiteral("d"), true, true));

    // SPICE
    sym->setSpiceTemplate(QStringLiteral("D{instanceName} {A} {K} {model}"));

    return sym;
}

// ===========================================================================
// NPN BJT
// ===========================================================================
std::shared_ptr<SymbolDef> BuiltinSymbols::createNpn()
{
    auto sym = std::make_shared<SymbolDef>(QStringLiteral("npn"));
    sym->setDescription(QStringLiteral("NPN Bipolar Junction Transistor"));
    sym->setCategory(QStringLiteral("Transistors"));

    // Pins
    sym->addPin(makePin(QStringLiteral("B"), 1, -20, 0, PinDirection::Left, ElectricalType::Input));
    sym->addPin(makePin(QStringLiteral("C"), 2, 10, -20, PinDirection::Up, ElectricalType::Passive));
    sym->addPin(makePin(QStringLiteral("E"), 3, 10, 20, PinDirection::Down, ElectricalType::Passive));

    // Graphics
    sym->addGraphic(std::make_shared<GraphicLine>(-20, 0, -5, 0));    // base lead
    sym->addGraphic(std::make_shared<GraphicLine>(-5, -12, -5, 12));  // base vertical bar
    sym->addGraphic(std::make_shared<GraphicLine>(-5, -7, 10, -17));  // collector diagonal
    sym->addGraphic(std::make_shared<GraphicLine>(10, -20, 10, -17)); // collector lead
    sym->addGraphic(std::make_shared<GraphicLine>(-5, 7, 10, 17));    // emitter diagonal
    sym->addGraphic(std::make_shared<GraphicLine>(10, 17, 10, 20));   // emitter lead

    // Arrow on emitter pointing out
    sym->addGraphic(std::make_shared<GraphicPolyline>(
        QList<QPointF>{{6, 14}, {10, 17}, {7, 11}}, true));

    // Properties
    sym->addPropertyTemplate(makePropTemplate(QStringLiteral("model"), QStringLiteral("npn"), true, true));

    // SPICE
    sym->setSpiceTemplate(QStringLiteral("Q{instanceName} {C} {B} {E} {model}"));

    return sym;
}

// ===========================================================================
// PNP BJT
// ===========================================================================
std::shared_ptr<SymbolDef> BuiltinSymbols::createPnp()
{
    auto sym = std::make_shared<SymbolDef>(QStringLiteral("pnp"));
    sym->setDescription(QStringLiteral("PNP Bipolar Junction Transistor"));
    sym->setCategory(QStringLiteral("Transistors"));

    // Pins
    sym->addPin(makePin(QStringLiteral("B"), 1, -20, 0, PinDirection::Left, ElectricalType::Input));
    sym->addPin(makePin(QStringLiteral("C"), 2, 10, -20, PinDirection::Up, ElectricalType::Passive));
    sym->addPin(makePin(QStringLiteral("E"), 3, 10, 20, PinDirection::Down, ElectricalType::Passive));

    // Graphics
    sym->addGraphic(std::make_shared<GraphicLine>(-20, 0, -5, 0));    // base lead
    sym->addGraphic(std::make_shared<GraphicLine>(-5, -12, -5, 12));  // base vertical bar
    sym->addGraphic(std::make_shared<GraphicLine>(-5, -7, 10, -17));  // collector diagonal
    sym->addGraphic(std::make_shared<GraphicLine>(10, -20, 10, -17)); // collector lead
    sym->addGraphic(std::make_shared<GraphicLine>(-5, 7, 10, 17));    // emitter diagonal
    sym->addGraphic(std::make_shared<GraphicLine>(10, 17, 10, 20));   // emitter lead

    // Arrow on emitter pointing toward base
    sym->addGraphic(std::make_shared<GraphicPolyline>(
        QList<QPointF>{{-2, 4}, {-5, 7}, {-1, 10}}, true));

    // Properties
    sym->addPropertyTemplate(makePropTemplate(QStringLiteral("model"), QStringLiteral("pnp"), true, true));

    // SPICE
    sym->setSpiceTemplate(QStringLiteral("Q{instanceName} {C} {B} {E} {model}"));

    return sym;
}

// ===========================================================================
// Voltage Source
// ===========================================================================
std::shared_ptr<SymbolDef> BuiltinSymbols::createVSource()
{
    auto sym = std::make_shared<SymbolDef>(QStringLiteral("vsource"));
    sym->setDescription(QStringLiteral("Voltage Source"));
    sym->setCategory(QStringLiteral("Sources"));

    // Pins
    sym->addPin(makePin(QStringLiteral("P"), 1, 0, -25, PinDirection::Up, ElectricalType::Passive));
    sym->addPin(makePin(QStringLiteral("N"), 2, 0, 25, PinDirection::Down, ElectricalType::Passive));

    // Graphics
    sym->addGraphic(std::make_shared<GraphicCircle>(0, 0, 15, false)); // circle body
    sym->addGraphic(std::make_shared<GraphicLine>(0, -25, 0, -15));    // top lead
    sym->addGraphic(std::make_shared<GraphicLine>(0, 15, 0, 25));      // bottom lead
    sym->addGraphic(std::make_shared<GraphicLine>(0, -9, 0, -3));      // plus vertical
    sym->addGraphic(std::make_shared<GraphicLine>(-3, -6, 3, -6));     // plus horizontal
    sym->addGraphic(std::make_shared<GraphicLine>(-3, 6, 3, 6));       // minus horizontal

    // Properties
    sym->addPropertyTemplate(makePropTemplate(QStringLiteral("value"), QStringLiteral("1.8"), true, true));

    // SPICE
    sym->setSpiceTemplate(QStringLiteral("V{instanceName} {P} {N} {value}"));

    return sym;
}

// ===========================================================================
// Current Source
// ===========================================================================
std::shared_ptr<SymbolDef> BuiltinSymbols::createISource()
{
    auto sym = std::make_shared<SymbolDef>(QStringLiteral("isource"));
    sym->setDescription(QStringLiteral("Current Source"));
    sym->setCategory(QStringLiteral("Sources"));

    // Pins
    sym->addPin(makePin(QStringLiteral("P"), 1, 0, -25, PinDirection::Up, ElectricalType::Passive));
    sym->addPin(makePin(QStringLiteral("N"), 2, 0, 25, PinDirection::Down, ElectricalType::Passive));

    // Graphics
    sym->addGraphic(std::make_shared<GraphicCircle>(0, 0, 15, false)); // circle body
    sym->addGraphic(std::make_shared<GraphicLine>(0, -25, 0, -15));    // top lead
    sym->addGraphic(std::make_shared<GraphicLine>(0, 15, 0, 25));      // bottom lead
    sym->addGraphic(std::make_shared<GraphicLine>(0, 8, 0, -8));       // arrow line
    sym->addGraphic(std::make_shared<GraphicPolyline>(
        QList<QPointF>{{-3, -4}, {0, -8}, {3, -4}}, true));           // arrowhead pointing up

    // Properties
    sym->addPropertyTemplate(makePropTemplate(QStringLiteral("value"), QStringLiteral("100u"), true, true));

    // SPICE
    sym->setSpiceTemplate(QStringLiteral("I{instanceName} {P} {N} {value}"));

    return sym;
}

// ===========================================================================
// VCVS (Voltage-Controlled Voltage Source)
// ===========================================================================
std::shared_ptr<SymbolDef> BuiltinSymbols::createVcvs()
{
    auto sym = createControlledSource(QStringLiteral("vcvs"),
                                      QStringLiteral("gain"), QStringLiteral("1"),
                                      QStringLiteral("E"));
    sym->setDescription(QStringLiteral("Voltage-Controlled Voltage Source"));
    return sym;
}

// ===========================================================================
// VCCS (Voltage-Controlled Current Source)
// ===========================================================================
std::shared_ptr<SymbolDef> BuiltinSymbols::createVccs()
{
    auto sym = createControlledSource(QStringLiteral("vccs"),
                                      QStringLiteral("gm"), QStringLiteral("1m"),
                                      QStringLiteral("G"));
    sym->setDescription(QStringLiteral("Voltage-Controlled Current Source"));
    return sym;
}

// ===========================================================================
// CCVS (Current-Controlled Voltage Source)
// ===========================================================================
std::shared_ptr<SymbolDef> BuiltinSymbols::createCcvs()
{
    auto sym = createControlledSource(QStringLiteral("ccvs"),
                                      QStringLiteral("rm"), QStringLiteral("1k"),
                                      QStringLiteral("H"));
    sym->setDescription(QStringLiteral("Current-Controlled Voltage Source"));
    return sym;
}

// ===========================================================================
// CCCS (Current-Controlled Current Source)
// ===========================================================================
std::shared_ptr<SymbolDef> BuiltinSymbols::createCccs()
{
    auto sym = createControlledSource(QStringLiteral("cccs"),
                                      QStringLiteral("gain"), QStringLiteral("1"),
                                      QStringLiteral("F"));
    sym->setDescription(QStringLiteral("Current-Controlled Current Source"));
    return sym;
}

// ===========================================================================
// Op-Amp
// ===========================================================================
std::shared_ptr<SymbolDef> BuiltinSymbols::createOpamp()
{
    auto sym = std::make_shared<SymbolDef>(QStringLiteral("opamp"));
    sym->setDescription(QStringLiteral("Ideal Operational Amplifier"));
    sym->setCategory(QStringLiteral("Amplifiers"));

    // Pins
    sym->addPin(makePin(QStringLiteral("INP"), 1, -20, -10, PinDirection::Left, ElectricalType::Input));
    sym->addPin(makePin(QStringLiteral("INM"), 2, -20, 10, PinDirection::Left, ElectricalType::Input));
    sym->addPin(makePin(QStringLiteral("OUT"), 3, 20, 0, PinDirection::Right, ElectricalType::Output));

    // Graphics
    sym->addGraphic(std::make_shared<GraphicPolyline>(
        QList<QPointF>{{-15, -20}, {-15, 20}, {20, 0}, {-15, -20}}, false)); // triangle
    sym->addGraphic(std::make_shared<GraphicText>(-10, -8, QStringLiteral("+"), 8));
    sym->addGraphic(std::make_shared<GraphicText>(-10, 10, QString::fromUtf8("\xe2\x88\x92"), 8)); // minus sign
    sym->addGraphic(std::make_shared<GraphicLine>(-20, -10, -15, -10));  // INP lead
    sym->addGraphic(std::make_shared<GraphicLine>(-20, 10, -15, 10));    // INM lead
    sym->addGraphic(std::make_shared<GraphicLine>(20, 0, 20, 0));        // OUT (at triangle tip)

    // No properties for ideal opamp
    // No default SPICE template

    return sym;
}

// ===========================================================================
// Ground
// ===========================================================================
std::shared_ptr<SymbolDef> BuiltinSymbols::createGnd()
{
    auto sym = std::make_shared<SymbolDef>(QStringLiteral("gnd"));
    sym->setDescription(QStringLiteral("Ground"));
    sym->setCategory(QStringLiteral("Power"));

    // Pin
    sym->addPin(makePin(QStringLiteral("PIN"), 1, 0, -10, PinDirection::Up, ElectricalType::Power));

    // Graphics
    sym->addGraphic(std::make_shared<GraphicLine>(0, -10, 0, 0));   // vertical lead
    sym->addGraphic(std::make_shared<GraphicLine>(-10, 0, 10, 0));  // top bar
    sym->addGraphic(std::make_shared<GraphicLine>(-6, 4, 6, 4));    // middle bar
    sym->addGraphic(std::make_shared<GraphicLine>(-2, 8, 2, 8));    // bottom bar

    return sym;
}

// ===========================================================================
// VDD
// ===========================================================================
std::shared_ptr<SymbolDef> BuiltinSymbols::createVdd()
{
    auto sym = std::make_shared<SymbolDef>(QStringLiteral("vdd"));
    sym->setDescription(QStringLiteral("VDD Power Supply"));
    sym->setCategory(QStringLiteral("Power"));

    // Pin
    sym->addPin(makePin(QStringLiteral("PIN"), 1, 0, 10, PinDirection::Down, ElectricalType::Power));

    // Graphics
    sym->addGraphic(std::make_shared<GraphicLine>(0, 10, 0, 0));     // vertical lead
    sym->addGraphic(std::make_shared<GraphicLine>(-10, 0, 10, 0));   // horizontal bar
    sym->addGraphic(std::make_shared<GraphicText>(0, -8, QStringLiteral("VDD"), 8));

    return sym;
}

// ===========================================================================
// Create the full builtin library
// ===========================================================================
std::shared_ptr<SymbolLibrary> BuiltinSymbols::createBuiltinLibrary()
{
    auto lib = std::make_shared<SymbolLibrary>(QStringLiteral("basic_analog"));

    lib->addSymbol(createNmos4());
    lib->addSymbol(createPmos4());
    lib->addSymbol(createResistor());
    lib->addSymbol(createCapacitor());
    lib->addSymbol(createInductor());
    lib->addSymbol(createDiode());
    lib->addSymbol(createNpn());
    lib->addSymbol(createPnp());
    lib->addSymbol(createVSource());
    lib->addSymbol(createISource());
    lib->addSymbol(createVcvs());
    lib->addSymbol(createVccs());
    lib->addSymbol(createCcvs());
    lib->addSymbol(createCccs());
    lib->addSymbol(createOpamp());
    lib->addSymbol(createGnd());
    lib->addSymbol(createVdd());

    return lib;
}

} // namespace myschematic
