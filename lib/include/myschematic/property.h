#pragma once

#include <QList>
#include <QMap>
#include <QString>
#include <QStringList>

namespace myschematic {

struct Property {
    QString name;
    QString value;
    QString defaultValue;
    bool required = false;
    bool visible = true;
};

class PropertyBag {
public:
    void set(const QString& name, const QString& value);
    QString get(const QString& name, const QString& fallback = QString()) const;
    bool has(const QString& name) const;
    void remove(const QString& name);
    QStringList names() const;
    QList<Property> all() const;
    void initFromTemplates(const QList<Property>& templates);

private:
    QMap<QString, Property> m_properties;
};

} // namespace myschematic
