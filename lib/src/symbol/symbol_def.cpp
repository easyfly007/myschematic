#include "myschematic/symbol_def.h"

namespace myschematic {

SymbolDef::SymbolDef(const QString& name)
    : m_name(name)
{
}

QString SymbolDef::name() const
{
    return m_name;
}

QString SymbolDef::libraryName() const
{
    return m_libraryName;
}

void SymbolDef::setLibraryName(const QString& lib)
{
    m_libraryName = lib;
}

QString SymbolDef::description() const
{
    return m_description;
}

void SymbolDef::setDescription(const QString& desc)
{
    m_description = desc;
}

QString SymbolDef::category() const
{
    return m_category;
}

void SymbolDef::setCategory(const QString& cat)
{
    m_category = cat;
}

void SymbolDef::addPin(const SymbolPin& pin)
{
    m_pins.append(pin);
}

const SymbolPin* SymbolDef::pinByName(const QString& name) const
{
    for (const auto& pin : m_pins) {
        if (pin.name == name) {
            return &pin;
        }
    }
    return nullptr;
}

const QList<SymbolPin>& SymbolDef::pins() const
{
    return m_pins;
}

void SymbolDef::addGraphic(std::shared_ptr<SymbolGraphic> graphic)
{
    m_graphics.append(std::move(graphic));
}

const QList<std::shared_ptr<SymbolGraphic>>& SymbolDef::graphics() const
{
    return m_graphics;
}

void SymbolDef::addPropertyTemplate(const Property& prop)
{
    m_propertyTemplates.append(prop);
}

const QList<Property>& SymbolDef::propertyTemplates() const
{
    return m_propertyTemplates;
}

void SymbolDef::setSpiceTemplate(const QString& tmpl)
{
    m_spiceTemplate = tmpl;
}

QString SymbolDef::spiceTemplate() const
{
    return m_spiceTemplate;
}

QRectF SymbolDef::boundingRect() const
{
    if (m_graphics.isEmpty()) {
        return QRectF();
    }

    QRectF rect = m_graphics.first()->boundingRect();
    for (int i = 1; i < m_graphics.size(); ++i) {
        rect = rect.united(m_graphics[i]->boundingRect());
    }
    return rect;
}

} // namespace myschematic
