#include "myschematic/property.h"

namespace myschematic {

void PropertyBag::set(const QString& name, const QString& value)
{
    if (m_properties.contains(name)) {
        m_properties[name].value = value;
    } else {
        Property prop;
        prop.name = name;
        prop.value = value;
        m_properties.insert(name, prop);
    }
}

QString PropertyBag::get(const QString& name, const QString& fallback) const
{
    auto it = m_properties.constFind(name);
    if (it != m_properties.constEnd()) {
        return it->value;
    }
    return fallback;
}

bool PropertyBag::has(const QString& name) const
{
    return m_properties.contains(name);
}

void PropertyBag::remove(const QString& name)
{
    m_properties.remove(name);
}

QStringList PropertyBag::names() const
{
    return m_properties.keys();
}

QList<Property> PropertyBag::all() const
{
    return m_properties.values();
}

void PropertyBag::initFromTemplates(const QList<Property>& templates)
{
    m_properties.clear();
    for (const auto& tmpl : templates) {
        Property prop = tmpl;
        prop.value = tmpl.defaultValue;
        m_properties.insert(prop.name, prop);
    }
}

} // namespace myschematic
