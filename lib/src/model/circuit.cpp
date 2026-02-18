#include "myschematic/circuit.h"
#include "myschematic/sheet.h"
#include <algorithm>

namespace myschematic {

Circuit::Circuit(QObject* parent)
    : QObject(parent)
{
}

Circuit::~Circuit() = default;

Sheet* Circuit::createSheet(const QString& name) {
    // Check for duplicate name
    for (const auto* sheet : m_sheets) {
        if (sheet->name() == name) {
            return nullptr;
        }
    }

    auto* sheet = new Sheet(name, this);
    m_sheets.append(sheet);
    return sheet;
}

void Circuit::removeSheet(const QString& id) {
    auto it = std::find_if(m_sheets.begin(), m_sheets.end(),
                           [&id](Sheet* s) { return s->id() == id; });
    if (it != m_sheets.end()) {
        Sheet* sheet = *it;
        m_sheets.erase(it);
        sheet->deleteLater();
    }
}

Sheet* Circuit::sheetById(const QString& id) const {
    auto it = std::find_if(m_sheets.begin(), m_sheets.end(),
                           [&id](Sheet* s) { return s->id() == id; });
    return (it != m_sheets.end()) ? *it : nullptr;
}

Sheet* Circuit::sheetByName(const QString& name) const {
    auto it = std::find_if(m_sheets.begin(), m_sheets.end(),
                           [&name](Sheet* s) { return s->name() == name; });
    return (it != m_sheets.end()) ? *it : nullptr;
}

QList<Sheet*> Circuit::sheets() const {
    return m_sheets;
}

} // namespace myschematic
