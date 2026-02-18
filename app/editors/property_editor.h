#pragma once

#include <QDockWidget>

class QTableWidget;

namespace myschematic {
class Component;
}

class PropertyEditor : public QDockWidget {
    Q_OBJECT

public:
    explicit PropertyEditor(QWidget* parent = nullptr);

    void setComponent(myschematic::Component* component);
    void clear();

signals:
    void propertyChanged(myschematic::Component* component, const QString& name,
                         const QString& oldValue, const QString& newValue);

private slots:
    void onCellChanged(int row, int column);

private:
    void populateTable();

    QTableWidget* m_table;
    myschematic::Component* m_component = nullptr;
    bool m_updatingTable = false;
};
