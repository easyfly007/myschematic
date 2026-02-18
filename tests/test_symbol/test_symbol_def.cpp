#include <gtest/gtest.h>
#include <myschematic/symbol_def.h>
#include <myschematic/symbol_graphic.h>
#include <myschematic/symbol_pin.h>
#include <myschematic/property.h>

using namespace myschematic;

TEST(SymbolDef, Creation) {
    SymbolDef def("test");
    EXPECT_EQ(def.name(), "test");
}

TEST(SymbolDef, AddPins) {
    SymbolDef def("test");

    SymbolPin pin1;
    pin1.name = "A";
    pin1.pinNumber = 1;
    pin1.offset = QPointF(-30, 0);

    SymbolPin pin2;
    pin2.name = "B";
    pin2.pinNumber = 2;
    pin2.offset = QPointF(30, 0);

    def.addPin(pin1);
    def.addPin(pin2);

    EXPECT_EQ(def.pins().size(), 2);
}

TEST(SymbolDef, PinByName) {
    SymbolDef def("test");

    SymbolPin pin;
    pin.name = "G";
    pin.pinNumber = 1;
    pin.offset = QPointF(-30, 0);
    def.addPin(pin);

    const SymbolPin* found = def.pinByName("G");
    ASSERT_NE(found, nullptr);
    EXPECT_DOUBLE_EQ(found->offset.x(), -30.0);
    EXPECT_DOUBLE_EQ(found->offset.y(), 0.0);

    EXPECT_EQ(def.pinByName("X"), nullptr);
}

TEST(SymbolDef, AddGraphics) {
    SymbolDef def("test");

    auto line = std::make_shared<GraphicLine>(QPointF(0, 0), QPointF(10, 10));
    auto rect = std::make_shared<GraphicRect>(QRectF(-5, -5, 10, 10));

    def.addGraphic(line);
    def.addGraphic(rect);

    EXPECT_EQ(def.graphics().size(), 2);
}

TEST(SymbolDef, BoundingRect) {
    SymbolDef def("test");
    auto rect = std::make_shared<GraphicRect>(QRectF(-10, -10, 20, 20));
    def.addGraphic(rect);

    QRectF br = def.boundingRect();
    EXPECT_DOUBLE_EQ(br.x(), -10.0);
    EXPECT_DOUBLE_EQ(br.y(), -10.0);
    EXPECT_DOUBLE_EQ(br.width(), 20.0);
    EXPECT_DOUBLE_EQ(br.height(), 20.0);
}

TEST(SymbolDef, BoundingRectUnion) {
    SymbolDef def("test");

    // Two non-overlapping rects
    auto rect1 = std::make_shared<GraphicRect>(QRectF(0, 0, 10, 10));
    auto rect2 = std::make_shared<GraphicRect>(QRectF(20, 20, 10, 10));

    def.addGraphic(rect1);
    def.addGraphic(rect2);

    QRectF br = def.boundingRect();
    // Union should span from (0,0) to (30,30)
    EXPECT_DOUBLE_EQ(br.x(), 0.0);
    EXPECT_DOUBLE_EQ(br.y(), 0.0);
    EXPECT_DOUBLE_EQ(br.width(), 30.0);
    EXPECT_DOUBLE_EQ(br.height(), 30.0);
}

TEST(SymbolDef, EmptyBoundingRect) {
    SymbolDef def("test");
    QRectF br = def.boundingRect();
    EXPECT_TRUE(br.isNull() || br.isEmpty());
}

TEST(SymbolDef, PropertyTemplates) {
    SymbolDef def("test");

    Property p1;
    p1.name = "model";
    p1.value = "nch";
    p1.defaultValue = "nch";

    Property p2;
    p2.name = "W";
    p2.value = "1u";
    p2.defaultValue = "1u";

    def.addPropertyTemplate(p1);
    def.addPropertyTemplate(p2);

    EXPECT_EQ(def.propertyTemplates().size(), 2);
    EXPECT_EQ(def.propertyTemplates().at(0).name, "model");
    EXPECT_EQ(def.propertyTemplates().at(1).name, "W");
}

TEST(SymbolDef, SpiceTemplate) {
    SymbolDef def("test");
    EXPECT_TRUE(def.spiceTemplate().isEmpty());

    def.setSpiceTemplate("M{name} {D} {G} {S} {B} {model} W={W} L={L}");
    EXPECT_EQ(def.spiceTemplate(), "M{name} {D} {G} {S} {B} {model} W={W} L={L}");
}

TEST(SymbolDef, LibraryName) {
    SymbolDef def("test");
    EXPECT_TRUE(def.libraryName().isEmpty());

    def.setLibraryName("basic_analog");
    EXPECT_EQ(def.libraryName(), "basic_analog");
}
