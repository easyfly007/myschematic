#include <gtest/gtest.h>
#include <myschematic/circuit.h>
#include <myschematic/sheet.h>

using namespace myschematic;

TEST(Circuit, CreateSheet) {
    Circuit circuit;
    Sheet* sheet = circuit.createSheet("top");
    ASSERT_NE(sheet, nullptr);
    EXPECT_FALSE(sheet->id().isEmpty());
    EXPECT_EQ(sheet->name(), "top");
    EXPECT_EQ(circuit.sheets().size(), 1);
}

TEST(Circuit, CreateMultipleSheets) {
    Circuit circuit;
    Sheet* s1 = circuit.createSheet("sheet1");
    Sheet* s2 = circuit.createSheet("sheet2");
    ASSERT_NE(s1, nullptr);
    ASSERT_NE(s2, nullptr);
    EXPECT_NE(s1->id(), s2->id());
    EXPECT_EQ(circuit.sheets().size(), 2);
}

TEST(Circuit, RemoveSheet) {
    Circuit circuit;
    Sheet* sheet = circuit.createSheet("top");
    QString id = sheet->id();
    circuit.removeSheet(id);
    EXPECT_EQ(circuit.sheets().size(), 0);
    EXPECT_EQ(circuit.sheetById(id), nullptr);
}

TEST(Circuit, DuplicateSheetName) {
    Circuit circuit;
    Sheet* s1 = circuit.createSheet("top");
    Sheet* s2 = circuit.createSheet("top");
    EXPECT_NE(s1, nullptr);
    EXPECT_EQ(s2, nullptr);
    EXPECT_EQ(circuit.sheets().size(), 1);
}

TEST(Circuit, SheetById) {
    Circuit circuit;
    Sheet* sheet = circuit.createSheet("top");
    EXPECT_EQ(circuit.sheetById(sheet->id()), sheet);
    EXPECT_EQ(circuit.sheetById("nonexistent"), nullptr);
}

TEST(Circuit, SheetByName) {
    Circuit circuit;
    Sheet* sheet = circuit.createSheet("top");
    EXPECT_EQ(circuit.sheetByName("top"), sheet);
    EXPECT_EQ(circuit.sheetByName("nonexistent"), nullptr);
}
