#include "editors/property_editor.h"
#include <myschematic/component.h>
#include <myschematic/property.h>
#include <QTableWidget>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QLabel>

PropertyEditor::PropertyEditor(QWidget* parent)
    : QDockWidget(tr("Properties"), parent)
    , m_table(new QTableWidget(this))
{
    m_table->setColumnCount(2);
    m_table->setHorizontalHeaderLabels({tr("Property"), tr("Value")});
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->verticalHeader()->setVisible(false);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);

    setWidget(m_table);

    connect(m_table, &QTableWidget::cellChanged,
            this, &PropertyEditor::onCellChanged);
}

void PropertyEditor::setComponent(myschematic::Component* component) {
    m_component = component;
    populateTable();
}

void PropertyEditor::clear() {
    m_component = nullptr;
    m_updatingTable = true;
    m_table->setRowCount(0);
    m_updatingTable = false;
}

void PropertyEditor::populateTable() {
    m_updatingTable = true;
    m_table->setRowCount(0);

    if (!m_component) {
        m_updatingTable = false;
        return;
    }

    // Row 0: Instance name
    int row = 0;
    m_table->insertRow(row);

    auto* nameLabel = new QTableWidgetItem(tr("Instance Name"));
    nameLabel->setFlags(nameLabel->flags() & ~Qt::ItemIsEditable);
    m_table->setItem(row, 0, nameLabel);

    auto* nameValue = new QTableWidgetItem(m_component->instanceName());
    m_table->setItem(row, 1, nameValue);
    ++row;

    // Remaining rows: component properties
    const auto& props = m_component->properties();
    QStringList propNames = props.names();
    for (const auto& propName : propNames) {
        m_table->insertRow(row);

        auto* propLabel = new QTableWidgetItem(propName);
        propLabel->setFlags(propLabel->flags() & ~Qt::ItemIsEditable);
        m_table->setItem(row, 0, propLabel);

        auto* propValue = new QTableWidgetItem(props.get(propName));
        m_table->setItem(row, 1, propValue);
        ++row;
    }

    m_updatingTable = false;
}

void PropertyEditor::onCellChanged(int row, int column) {
    if (m_updatingTable || !m_component || column != 1)
        return;

    QTableWidgetItem* nameItem = m_table->item(row, 0);
    QTableWidgetItem* valueItem = m_table->item(row, 1);
    if (!nameItem || !valueItem)
        return;

    QString propName = nameItem->text();
    QString newValue = valueItem->text();

    if (row == 0) {
        // Instance name change
        QString oldValue = m_component->instanceName();
        if (oldValue != newValue) {
            emit propertyChanged(m_component, QStringLiteral("instanceName"),
                                 oldValue, newValue);
        }
    } else {
        // Regular property change
        QString oldValue = m_component->properties().get(propName);
        if (oldValue != newValue) {
            emit propertyChanged(m_component, propName, oldValue, newValue);
        }
    }
}
