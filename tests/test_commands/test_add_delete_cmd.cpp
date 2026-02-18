#include <gtest/gtest.h>
#include <myschematic/sheet.h>
#include <myschematic/component.h>
#include <QPointF>

// Test add/delete logic at the model level (without GUI commands)

using namespace myschematic;

TEST(AddDelete, AddComponent) {
    Sheet sheet("test");
    EXPECT_EQ(sheet.components().size(), 0);

    auto* comp = sheet.addComponent(QPointF(100, 200));
    ASSERT_NE(comp, nullptr);
    EXPECT_EQ(sheet.components().size(), 1);
    EXPECT_EQ(comp->position(), QPointF(100, 200));
}

TEST(AddDelete, DeleteComponent) {
    Sheet sheet("test");
    auto* comp = sheet.addComponent(QPointF(0, 0));
    QString id = comp->id();

    EXPECT_EQ(sheet.components().size(), 1);
    sheet.removeElement(id);
    EXPECT_EQ(sheet.components().size(), 0);
    EXPECT_EQ(sheet.elementById(id), nullptr);
}

TEST(AddDelete, AddDeleteAdd) {
    Sheet sheet("test");
    auto* comp = sheet.addComponent(QPointF(100, 100));
    QString id = comp->id();

    sheet.removeElement(id);
    EXPECT_EQ(sheet.components().size(), 0);

    // Re-add (simulating redo of add)
    auto* comp2 = sheet.addComponent(QPointF(100, 100));
    EXPECT_EQ(sheet.components().size(), 1);
    EXPECT_NE(comp2->id(), id); // New component gets new id
}

TEST(AddDelete, DeleteMultiple) {
    Sheet sheet("test");
    auto* c1 = sheet.addComponent(QPointF(0, 0));
    auto* c2 = sheet.addComponent(QPointF(100, 100));
    auto* c3 = sheet.addComponent(QPointF(200, 200));

    sheet.removeElement(c2->id());
    EXPECT_EQ(sheet.components().size(), 2);

    // c1 and c3 should still be there
    EXPECT_NE(sheet.elementById(c1->id()), nullptr);
    EXPECT_NE(sheet.elementById(c3->id()), nullptr);
}

TEST(AddDelete, SignalsOnAddDelete) {
    Sheet sheet("test");
    int addCount = 0;
    int removeCount = 0;
    QObject::connect(&sheet, &Sheet::elementAdded, [&addCount](SchematicElement*) { ++addCount; });
    QObject::connect(&sheet, &Sheet::elementRemoved, [&removeCount](const QString&) { ++removeCount; });

    auto* comp = sheet.addComponent(QPointF(0, 0));
    EXPECT_EQ(addCount, 1);

    sheet.removeElement(comp->id());
    EXPECT_EQ(removeCount, 1);
}

TEST(AddDelete, ComponentProperties) {
    Sheet sheet("test");
    auto* comp = sheet.addComponent(QPointF(50, 75));
    comp->setInstanceName("R1");
    comp->setRotation(90);
    comp->setMirrored(true);

    auto* found = sheet.componentById(comp->id());
    ASSERT_NE(found, nullptr);
    EXPECT_EQ(found->instanceName(), "R1");
    EXPECT_EQ(found->rotation(), 90);
    EXPECT_TRUE(found->isMirrored());
    EXPECT_EQ(found->position(), QPointF(50, 75));
}
