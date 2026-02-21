#include <gtest/gtest.h>
#include <myschematic/component.h>
#include <myschematic/builtin_symbols.h>
#include <myschematic/symbol_def.h>
#include <myschematic/symbol_pin.h>
#include <myschematic/pin.h>
#include <myschematic/property.h>

using namespace myschematic;

// Helper: keep a shared_ptr alive for the duration of a test
static std::shared_ptr<SymbolDef> makeNmos4() {
    return BuiltinSymbols::createNmos4();
}

TEST(ComponentExt, DefaultConstructorStillWorks) {
    Component comp;
    EXPECT_TRUE(comp.pins().empty());
    QRectF rect = comp.boundingRect();
    EXPECT_DOUBLE_EQ(rect.width(), 60.0);
    EXPECT_DOUBLE_EQ(rect.height(), 40.0);
}

TEST(ComponentExt, ConstructWithSymbol) {
    auto nmos = makeNmos4();
    Component comp(nmos.get());
    EXPECT_EQ(comp.pins().size(), 4);
    EXPECT_EQ(comp.symbolDef(), nmos.get());
}

TEST(ComponentExt, SetSymbolDef) {
    auto nmos = makeNmos4();
    Component comp;
    EXPECT_TRUE(comp.pins().empty());

    comp.setSymbolDef(nmos.get());
    EXPECT_EQ(comp.pins().size(), 4);
    EXPECT_EQ(comp.symbolDef(), nmos.get());
}

TEST(ComponentExt, PinByName) {
    auto nmos = makeNmos4();
    Component comp(nmos.get());

    ASSERT_NE(comp.pinByName("G"), nullptr);
    ASSERT_NE(comp.pinByName("D"), nullptr);
    ASSERT_NE(comp.pinByName("S"), nullptr);
    ASSERT_NE(comp.pinByName("B"), nullptr);
    EXPECT_EQ(comp.pinByName("X"), nullptr);
}

TEST(ComponentExt, PinWorldPosition_NoRotation) {
    auto nmos = makeNmos4();
    Component comp(nmos.get());
    comp.setPosition(QPointF(100, 100));

    // G pin has offset (-30, 0) per NMOS4 definition
    const SymbolPin* gPin = nmos->pinByName("G");
    ASSERT_NE(gPin, nullptr);
    double offsetX = gPin->offset.x();
    double offsetY = gPin->offset.y();

    QPointF worldPos = comp.pinWorldPosition("G");
    EXPECT_DOUBLE_EQ(worldPos.x(), 100.0 + offsetX);
    EXPECT_DOUBLE_EQ(worldPos.y(), 100.0 + offsetY);
}

TEST(ComponentExt, PinWorldPosition_Rotated90) {
    auto nmos = makeNmos4();
    Component comp(nmos.get());
    comp.setPosition(QPointF(100, 100));
    comp.setRotation(90);

    // Rotation 90: offset (x,y) -> (-y, x)
    const SymbolPin* gPin = nmos->pinByName("G");
    ASSERT_NE(gPin, nullptr);
    double ox = gPin->offset.x();
    double oy = gPin->offset.y();

    QPointF worldPos = comp.pinWorldPosition("G");
    EXPECT_DOUBLE_EQ(worldPos.x(), 100.0 + (-oy));
    EXPECT_DOUBLE_EQ(worldPos.y(), 100.0 + ox);
}

TEST(ComponentExt, PinWorldPosition_Rotated180) {
    auto nmos = makeNmos4();
    Component comp(nmos.get());
    comp.setPosition(QPointF(100, 100));
    comp.setRotation(180);

    // Rotation 180: offset (x,y) -> (-x, -y)
    const SymbolPin* gPin = nmos->pinByName("G");
    ASSERT_NE(gPin, nullptr);
    double ox = gPin->offset.x();
    double oy = gPin->offset.y();

    QPointF worldPos = comp.pinWorldPosition("G");
    EXPECT_DOUBLE_EQ(worldPos.x(), 100.0 + (-ox));
    EXPECT_DOUBLE_EQ(worldPos.y(), 100.0 + (-oy));
}

TEST(ComponentExt, PinWorldPosition_Rotated270) {
    auto nmos = makeNmos4();
    Component comp(nmos.get());
    comp.setPosition(QPointF(100, 100));
    comp.setRotation(270);

    // Rotation 270: offset (x,y) -> (y, -x)
    const SymbolPin* gPin = nmos->pinByName("G");
    ASSERT_NE(gPin, nullptr);
    double ox = gPin->offset.x();
    double oy = gPin->offset.y();

    QPointF worldPos = comp.pinWorldPosition("G");
    EXPECT_DOUBLE_EQ(worldPos.x(), 100.0 + oy);
    EXPECT_DOUBLE_EQ(worldPos.y(), 100.0 + (-ox));
}

TEST(ComponentExt, PinWorldPosition_Mirrored) {
    auto nmos = makeNmos4();
    Component comp(nmos.get());
    comp.setPosition(QPointF(100, 100));
    comp.setMirrored(true);

    // Mirror first (flip X): offset (x,y) -> (-x, y), then no rotation
    const SymbolPin* gPin = nmos->pinByName("G");
    ASSERT_NE(gPin, nullptr);
    double ox = gPin->offset.x();
    double oy = gPin->offset.y();

    QPointF worldPos = comp.pinWorldPosition("G");
    EXPECT_DOUBLE_EQ(worldPos.x(), 100.0 + (-ox));
    EXPECT_DOUBLE_EQ(worldPos.y(), 100.0 + oy);
}

TEST(ComponentExt, PinWorldPosition_MirroredAndRotated90) {
    auto nmos = makeNmos4();
    Component comp(nmos.get());
    comp.setPosition(QPointF(100, 100));
    comp.setMirrored(true);
    comp.setRotation(90);

    // Mirror first: (x,y) -> (-x, y), then rotate 90: (-x,y) -> (-y, -x)
    const SymbolPin* gPin = nmos->pinByName("G");
    ASSERT_NE(gPin, nullptr);
    double ox = gPin->offset.x();
    double oy = gPin->offset.y();

    // After mirror: mx = -ox, my = oy
    // After rot90: rx = -my, ry = mx => rx = -oy, ry = -ox
    QPointF worldPos = comp.pinWorldPosition("G");
    EXPECT_DOUBLE_EQ(worldPos.x(), 100.0 + (-oy));
    EXPECT_DOUBLE_EQ(worldPos.y(), 100.0 + (-ox));
}

TEST(ComponentExt, Properties) {
    auto nmos = makeNmos4();
    Component comp(nmos.get());

    // Verify default properties from templates
    EXPECT_EQ(comp.property("model"), "nch");
    EXPECT_EQ(comp.property("W"), "1u");
    EXPECT_EQ(comp.property("L"), "100n");
}

TEST(ComponentExt, SetProperty) {
    auto nmos = makeNmos4();
    Component comp(nmos.get());

    comp.setProperty("W", "10u");
    EXPECT_EQ(comp.property("W"), "10u");
}

TEST(ComponentExt, BoundingRectWithSymbol) {
    auto nmos = makeNmos4();
    Component comp(nmos.get());
    comp.setPosition(QPointF(0, 0));

    QRectF symbolRect = nmos->boundingRect();
    QRectF compRect = comp.boundingRect();

    // Component bounding rect should use the symbol's dimensions
    EXPECT_DOUBLE_EQ(compRect.width(), symbolRect.width());
    EXPECT_DOUBLE_EQ(compRect.height(), symbolRect.height());
}

TEST(ComponentExt, BoundingRectRotated) {
    auto nmos = makeNmos4();
    Component comp(nmos.get());
    comp.setPosition(QPointF(0, 0));

    QRectF normalRect = comp.boundingRect();
    double normalW = normalRect.width();
    double normalH = normalRect.height();

    comp.setRotation(90);
    QRectF rotatedRect = comp.boundingRect();

    // After 90 degree rotation, width and height should swap
    EXPECT_DOUBLE_EQ(rotatedRect.width(), normalH);
    EXPECT_DOUBLE_EQ(rotatedRect.height(), normalW);
}
