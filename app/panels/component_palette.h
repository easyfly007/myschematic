#pragma once

#include <QDockWidget>

class QTreeWidget;
class QTreeWidgetItem;

namespace myschematic {
class LibraryManager;
class SymbolDef;
}

class ComponentPalette : public QDockWidget {
    Q_OBJECT

public:
    explicit ComponentPalette(QWidget* parent = nullptr);

    void setLibraryManager(myschematic::LibraryManager* mgr);

signals:
    void symbolSelected(myschematic::SymbolDef* symbol);

private slots:
    void onItemDoubleClicked(QTreeWidgetItem* item, int column);

private:
    void populateTree();

    QTreeWidget* m_tree;
    myschematic::LibraryManager* m_libraryMgr = nullptr;
};
