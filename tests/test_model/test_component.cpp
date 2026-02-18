#include <gtest/gtest.h>
#include <myschematic/component.h>
#include <QPointF>
#include <QRectF>

using namespace myschematic;

TEST(Component, DefaultState) {
    Component comp;
    EXPECT_EQ(comp.position(), QPointF(0, 0));
    EXPECT_EQ(comp.rotation(), 0);
    EXPECT_FALSE(comp.isMirrored());
    EXPECT_EQ(comp.type(), ElementType::Component);
    EXPECT_FALSE(comp.id().isEmpty());
}

TEST(Component, SetPosition) {
    Component comp;
    comp.setPosition(QPointF(100, 200));
    EXPECT_EQ(comp.position(), QPointF(100, 200));
}

TEST(Component, PositionChangedSignal) {
    Component comp;
    int signalCount = 0;
    QObject::connect(&comp, &Component::changed, [&signalCount]() { ++signalCount; });

    comp.setPosition(QPointF(50, 50));
    EXPECT_EQ(signalCount, 1);

    // Setting same position should not emit
    comp.setPosition(QPointF(50, 50));
    EXPECT_EQ(signalCount, 1);
}

TEST(Component, BoundingRect) {
    Component comp;
    comp.setPosition(QPointF(100, 100));
    QRectF rect = comp.boundingRect();
    EXPECT_DOUBLE_EQ(rect.width(), 60.0);
    EXPECT_DOUBLE_EQ(rect.height(), 40.0);
    EXPECT_DOUBLE_EQ(rect.center().x(), 100.0);
    EXPECT_DOUBLE_EQ(rect.center().y(), 100.0);
}

TEST(Component, BoundingRectRotated90) {
    Component comp;
    comp.setPosition(QPointF(100, 100));
    comp.setRotation(90);
    QRectF rect = comp.boundingRect();
    EXPECT_DOUBLE_EQ(rect.width(), 40.0);
    EXPECT_DOUBLE_EQ(rect.height(), 60.0);
}

TEST(Component, BoundingRectRotated180) {
    Component comp;
    comp.setPosition(QPointF(100, 100));
    comp.setRotation(180);
    QRectF rect = comp.boundingRect();
    EXPECT_DOUBLE_EQ(rect.width(), 60.0);
    EXPECT_DOUBLE_EQ(rect.height(), 40.0);
}

TEST(Component, BoundingRectRotated270) {
    Component comp;
    comp.setPosition(QPointF(100, 100));
    comp.setRotation(270);
    QRectF rect = comp.boundingRect();
    EXPECT_DOUBLE_EQ(rect.width(), 40.0);
    EXPECT_DOUBLE_EQ(rect.height(), 60.0);
}

TEST(Component, RotationNormalization) {
    Component comp;
    comp.setRotation(360);
    EXPECT_EQ(comp.rotation(), 0);

    comp.setRotation(-90);
    EXPECT_EQ(comp.rotation(), 270);

    comp.setRotation(450);
    EXPECT_EQ(comp.rotation(), 90);
}

TEST(Component, Mirror) {
    Component comp;
    EXPECT_FALSE(comp.isMirrored());
    comp.setMirrored(true);
    EXPECT_TRUE(comp.isMirrored());
}

TEST(Component, InstanceName) {
    Component comp;
    EXPECT_TRUE(comp.instanceName().isEmpty());
    comp.setInstanceName("M1");
    EXPECT_EQ(comp.instanceName(), "M1");
}

TEST(Component, UniqueIds) {
    Component c1;
    Component c2;
    EXPECT_NE(c1.id(), c2.id());
}
