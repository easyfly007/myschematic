#include <gtest/gtest.h>
#include <myschematic/sheet.h>
#include <myschematic/component.h>
#include <QPointF>

// Test move command logic at the model level (without GUI)
// MoveElementCmd depends on SchematicScene which requires Qt Widgets.
// For Phase 1 model-only testing, we test undo/redo semantics at the model level.

using namespace myschematic;

TEST(MoveModel, SetPosition) {
    Sheet sheet("test");
    auto* comp = sheet.addComponent(QPointF(0, 0));
    EXPECT_EQ(comp->position(), QPointF(0, 0));

    comp->setPosition(QPointF(100, 200));
    EXPECT_EQ(comp->position(), QPointF(100, 200));
}

TEST(MoveModel, MoveAndVerify) {
    Sheet sheet("test");
    auto* comp = sheet.addComponent(QPointF(50, 50));
    QPointF oldPos = comp->position();
    QPointF newPos(150, 250);

    comp->setPosition(newPos);
    EXPECT_EQ(comp->position(), newPos);

    // Simulate undo by restoring old position
    comp->setPosition(oldPos);
    EXPECT_EQ(comp->position(), oldPos);
}

TEST(MoveModel, MultipleElements) {
    Sheet sheet("test");
    auto* c1 = sheet.addComponent(QPointF(0, 0));
    auto* c2 = sheet.addComponent(QPointF(100, 100));

    c1->setPosition(QPointF(50, 50));
    c2->setPosition(QPointF(200, 200));

    EXPECT_EQ(c1->position(), QPointF(50, 50));
    EXPECT_EQ(c2->position(), QPointF(200, 200));
}

TEST(MoveModel, SignalOnMove) {
    Sheet sheet("test");
    auto* comp = sheet.addComponent(QPointF(0, 0));

    int signalCount = 0;
    QObject::connect(comp, &SchematicElement::changed, [&signalCount]() { ++signalCount; });
    comp->setPosition(QPointF(10, 20));
    EXPECT_EQ(signalCount, 1);
}
