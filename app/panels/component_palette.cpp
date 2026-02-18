#include "panels/component_palette.h"
#include <myschematic/library_manager.h>
#include <myschematic/symbol_library.h>
#include <myschematic/symbol_def.h>
#include <QTreeWidget>
#include <QHeaderView>
#include <QVBoxLayout>

ComponentPalette::ComponentPalette(QWidget* parent)
    : QDockWidget(tr("Components"), parent)
    , m_tree(new QTreeWidget(this))
{
    m_tree->setHeaderLabel(tr("Symbol Library"));
    m_tree->setAnimated(true);
    m_tree->setExpandsOnDoubleClick(false); // We handle double-click ourselves

    setWidget(m_tree);

    connect(m_tree, &QTreeWidget::itemDoubleClicked,
            this, &ComponentPalette::onItemDoubleClicked);
}

void ComponentPalette::setLibraryManager(myschematic::LibraryManager* mgr) {
    m_libraryMgr = mgr;
    populateTree();
}

void ComponentPalette::populateTree() {
    m_tree->clear();

    if (!m_libraryMgr)
        return;

    for (auto* library : m_libraryMgr->libraries()) {
        auto* libItem = new QTreeWidgetItem(m_tree);
        libItem->setText(0, library->name());
        libItem->setFlags(libItem->flags() & ~Qt::ItemIsSelectable);

        // Group symbols by category
        QMap<QString, QTreeWidgetItem*> categoryItems;

        for (auto* symbolDef : library->symbols()) {
            QString category = symbolDef->category();
            if (category.isEmpty())
                category = tr("Uncategorized");

            // Find or create category node
            QTreeWidgetItem* catItem = categoryItems.value(category, nullptr);
            if (!catItem) {
                catItem = new QTreeWidgetItem(libItem);
                catItem->setText(0, category);
                catItem->setFlags(catItem->flags() & ~Qt::ItemIsSelectable);
                categoryItems.insert(category, catItem);
            }

            // Add symbol leaf node
            auto* symItem = new QTreeWidgetItem(catItem);
            symItem->setText(0, symbolDef->name());
            // Store pointer to SymbolDef in user data
            symItem->setData(0, Qt::UserRole,
                             QVariant::fromValue(reinterpret_cast<quintptr>(symbolDef)));
        }
    }

    m_tree->expandAll();
}

void ComponentPalette::onItemDoubleClicked(QTreeWidgetItem* item, int /*column*/) {
    if (!item || item->childCount() > 0) {
        // This is a category or library node, not a symbol leaf
        return;
    }

    quintptr ptr = item->data(0, Qt::UserRole).value<quintptr>();
    if (ptr) {
        auto* symbolDef = reinterpret_cast<myschematic::SymbolDef*>(ptr);
        emit symbolSelected(symbolDef);
    }
}
