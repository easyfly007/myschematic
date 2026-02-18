#pragma once

#include <QList>
#include <QRectF>
#include <QString>
#include <memory>

#include "property.h"
#include "symbol_graphic.h"
#include "symbol_pin.h"

namespace myschematic {

class SymbolDef {
public:
    explicit SymbolDef(const QString& name);

    QString name() const;
    QString libraryName() const;
    void setLibraryName(const QString& lib);
    QString description() const;
    void setDescription(const QString& desc);
    QString category() const;
    void setCategory(const QString& cat);

    // Pins
    void addPin(const SymbolPin& pin);
    const SymbolPin* pinByName(const QString& name) const;
    const QList<SymbolPin>& pins() const;

    // Graphics
    void addGraphic(std::shared_ptr<SymbolGraphic> graphic);
    const QList<std::shared_ptr<SymbolGraphic>>& graphics() const;

    // Property templates
    void addPropertyTemplate(const Property& prop);
    const QList<Property>& propertyTemplates() const;

    // SPICE template
    void setSpiceTemplate(const QString& tmpl);
    QString spiceTemplate() const;

    // Bounding rect computed from all graphics
    QRectF boundingRect() const;

private:
    QString m_name;
    QString m_libraryName;
    QString m_description;
    QString m_category;
    QList<SymbolPin> m_pins;
    QList<std::shared_ptr<SymbolGraphic>> m_graphics;
    QList<Property> m_propertyTemplates;
    QString m_spiceTemplate;
};

} // namespace myschematic
