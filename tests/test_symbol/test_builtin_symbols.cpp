#include <gtest/gtest.h>
#include <myschematic/builtin_symbols.h>
#include <myschematic/symbol_def.h>
#include <myschematic/symbol_library.h>

using namespace myschematic;

TEST(Builtin, Nmos4Exists) {
    auto nmos = BuiltinSymbols::createNmos4();
    ASSERT_NE(nmos, nullptr);
}

TEST(Builtin, Nmos4Pins) {
    auto nmos = BuiltinSymbols::createNmos4();
    ASSERT_NE(nmos, nullptr);
    EXPECT_EQ(nmos->pins().size(), 4);

    ASSERT_NE(nmos->pinByName("G"), nullptr);
    ASSERT_NE(nmos->pinByName("D"), nullptr);
    ASSERT_NE(nmos->pinByName("S"), nullptr);
    ASSERT_NE(nmos->pinByName("B"), nullptr);
}

TEST(Builtin, Nmos4Graphics) {
    auto nmos = BuiltinSymbols::createNmos4();
    ASSERT_NE(nmos, nullptr);
    EXPECT_GT(nmos->graphics().size(), 0);
}

TEST(Builtin, Nmos4Properties) {
    auto nmos = BuiltinSymbols::createNmos4();
    ASSERT_NE(nmos, nullptr);

    const auto& templates = nmos->propertyTemplates();
    bool hasModel = false;
    bool hasW = false;
    bool hasL = false;

    for (const auto& prop : templates) {
        if (prop.name == "model") hasModel = true;
        if (prop.name == "W") hasW = true;
        if (prop.name == "L") hasL = true;
    }

    EXPECT_TRUE(hasModel);
    EXPECT_TRUE(hasW);
    EXPECT_TRUE(hasL);
}

TEST(Builtin, Nmos4SpiceTemplate) {
    auto nmos = BuiltinSymbols::createNmos4();
    ASSERT_NE(nmos, nullptr);
    EXPECT_FALSE(nmos->spiceTemplate().isEmpty());
}

TEST(Builtin, Pmos4Exists) {
    auto pmos = BuiltinSymbols::createPmos4();
    ASSERT_NE(pmos, nullptr);
    EXPECT_EQ(pmos->pins().size(), 4);
}

TEST(Builtin, ResistorPins) {
    auto res = BuiltinSymbols::createResistor();
    ASSERT_NE(res, nullptr);
    ASSERT_NE(res->pinByName("P"), nullptr);
    ASSERT_NE(res->pinByName("N"), nullptr);
}

TEST(Builtin, CapacitorPins) {
    auto cap = BuiltinSymbols::createCapacitor();
    ASSERT_NE(cap, nullptr);
    ASSERT_NE(cap->pinByName("P"), nullptr);
    ASSERT_NE(cap->pinByName("N"), nullptr);
}

TEST(Builtin, DiodePins) {
    auto diode = BuiltinSymbols::createDiode();
    ASSERT_NE(diode, nullptr);
    ASSERT_NE(diode->pinByName("A"), nullptr);
    ASSERT_NE(diode->pinByName("K"), nullptr);
}

TEST(Builtin, NpnPins) {
    auto npn = BuiltinSymbols::createNpn();
    ASSERT_NE(npn, nullptr);
    ASSERT_NE(npn->pinByName("B"), nullptr);
    ASSERT_NE(npn->pinByName("C"), nullptr);
    ASSERT_NE(npn->pinByName("E"), nullptr);
}

TEST(Builtin, OpampPins) {
    auto opamp = BuiltinSymbols::createOpamp();
    ASSERT_NE(opamp, nullptr);
    ASSERT_NE(opamp->pinByName("INP"), nullptr);
    ASSERT_NE(opamp->pinByName("INM"), nullptr);
    ASSERT_NE(opamp->pinByName("OUT"), nullptr);
}

TEST(Builtin, GndPin) {
    auto gnd = BuiltinSymbols::createGnd();
    ASSERT_NE(gnd, nullptr);
    EXPECT_EQ(gnd->pins().size(), 1);

    const SymbolPin* pin = gnd->pinByName("PIN");
    ASSERT_NE(pin, nullptr);
    EXPECT_EQ(pin->electricalType, ElectricalType::Power);
}

TEST(Builtin, VddPin) {
    auto vdd = BuiltinSymbols::createVdd();
    ASSERT_NE(vdd, nullptr);
    EXPECT_EQ(vdd->pins().size(), 1);

    const SymbolPin* pin = vdd->pinByName("PIN");
    ASSERT_NE(pin, nullptr);
    EXPECT_EQ(pin->electricalType, ElectricalType::Power);
}

TEST(Builtin, AllSymbolsValid) {
    // All 17 factory methods
    std::vector<std::shared_ptr<SymbolDef>> symbols = {
        BuiltinSymbols::createNmos4(),
        BuiltinSymbols::createPmos4(),
        BuiltinSymbols::createResistor(),
        BuiltinSymbols::createCapacitor(),
        BuiltinSymbols::createInductor(),
        BuiltinSymbols::createDiode(),
        BuiltinSymbols::createNpn(),
        BuiltinSymbols::createPnp(),
        BuiltinSymbols::createVSource(),
        BuiltinSymbols::createISource(),
        BuiltinSymbols::createVcvs(),
        BuiltinSymbols::createVccs(),
        BuiltinSymbols::createCcvs(),
        BuiltinSymbols::createCccs(),
        BuiltinSymbols::createOpamp(),
        BuiltinSymbols::createGnd(),
        BuiltinSymbols::createVdd(),
    };

    EXPECT_EQ(symbols.size(), 17u);

    for (size_t i = 0; i < symbols.size(); ++i) {
        ASSERT_NE(symbols[i], nullptr) << "Symbol at index " << i << " is null";
        EXPECT_FALSE(symbols[i]->name().isEmpty()) << "Symbol at index " << i << " has empty name";
        EXPECT_GE(symbols[i]->pins().size(), 1) << "Symbol " << symbols[i]->name().toStdString() << " has no pins";
    }
}

TEST(Builtin, BuiltinLibrary) {
    auto lib = BuiltinSymbols::createBuiltinLibrary();
    ASSERT_NE(lib, nullptr);
    EXPECT_EQ(lib->symbolNames().size(), 17);
}
