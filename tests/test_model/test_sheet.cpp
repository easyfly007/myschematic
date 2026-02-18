#include <gtest/gtest.h>
#include <myschematic/sheet.h>
#include <myschematic/component.h>
#include <QSignalSpy>
#include <QPointF>

using namespace myschematic;

TEST(Sheet, AddComponent) {
    Sheet sheet("test");
    Component* comp = sheet.addComponent(QPointF(100, 200));
    ASSERT_NE(comp, nullptr);
    EXPECT_FALSE(comp->id().isEmpty());
    EXPECT_EQ(comp->position(), QPointF(100, 200));
    EXPECT_EQ(sheet.components().size(), 1);
    EXPECT_EQ(sheet.elements().size(), 1);
}

TEST(Sheet, AddMultipleComponents) {
    Sheet sheet("test");
    Component* c1 = sheet.addComponent(QPointF(0, 0));
    Component* c2 = sheet.addComponent(QPointF(100, 100));
    EXPECT_NE(c1->id(), c2->id());
    EXPECT_EQ(sheet.components().size(), 2);
}

TEST(Sheet, RemoveElement) {
    Sheet sheet("test");
    Component* comp = sheet.addComponent(QPointF(0, 0));
    QString id = comp->id();
    sheet.removeElement(id);
    EXPECT_EQ(sheet.elementById(id), nullptr);
    EXPECT_EQ(sheet.components().size(), 0);
}

TEST(Sheet, RemoveNonexistent) {
    Sheet sheet("test");
    sheet.addComponent(QPointF(0, 0));
    sheet.removeElement("nonexistent-id");
    EXPECT_EQ(sheet.components().size(), 1);
}

TEST(Sheet, ElementById) {
    Sheet sheet("test");
    Component* comp = sheet.addComponent(QPointF(0, 0));
    EXPECT_EQ(sheet.elementById(comp->id()), comp);
    EXPECT_EQ(sheet.componentById(comp->id()), comp);
}

TEST(Sheet, ElementsInRect) {
    Sheet sheet("test");
    Component* c1 = sheet.addComponent(QPointF(50, 50));
    sheet.addComponent(QPointF(500, 500));

    QRectF searchRect(0, 0, 100, 100);
    auto found = sheet.elementsInRect(searchRect);
    EXPECT_EQ(found.size(), 1);
    EXPECT_EQ(found.first(), c1);
}

TEST(Sheet, ElementsAt) {
    Sheet sheet("test");
    Component* c1 = sheet.addComponent(QPointF(100, 100));
    sheet.addComponent(QPointF(500, 500));

    auto found = sheet.elementsAt(QPointF(102, 102), 5.0);
    EXPECT_EQ(found.size(), 1);
    EXPECT_EQ(found.first(), c1);
}

TEST(Sheet, SignalEmission) {
    Sheet sheet("test");
    QSignalSpy addSpy(&sheet, &Sheet::elementAdded);
    QSignalSpy removeSpy(&sheet, &Sheet::elementRemoved);

    Component* comp = sheet.addComponent(QPointF(0, 0));
    EXPECT_EQ(addSpy.count(), 1);

    sheet.removeElement(comp->id());
    EXPECT_EQ(removeSpy.count(), 1);
}

TEST(Sheet, IdAndName) {
    Sheet sheet("mysheet");
    EXPECT_FALSE(sheet.id().isEmpty());
    EXPECT_EQ(sheet.name(), "mysheet");
}
