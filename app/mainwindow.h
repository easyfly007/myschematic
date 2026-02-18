#pragma once

#include <QMainWindow>

class SchematicScene;
class SchematicView;

namespace myschematic {
class Circuit;
class Sheet;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

private:
    void setupMenus();
    void setupToolbar();
    void setupStatusBar();

    myschematic::Circuit* m_circuit;
    myschematic::Sheet* m_sheet;
    SchematicScene* m_scene;
    SchematicView* m_view;
};
