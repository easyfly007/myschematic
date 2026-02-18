#pragma once

#include <QList>
#include <QMap>
#include <QString>
#include <QStringList>
#include <memory>

namespace myschematic {

class SymbolDef;

class SymbolLibrary {
public:
    explicit SymbolLibrary(const QString& name);

    QString name() const;
    void addSymbol(std::shared_ptr<SymbolDef> symbol);
    void removeSymbol(const QString& name);
    SymbolDef* findSymbol(const QString& name) const;
    QList<SymbolDef*> symbols() const;
    QStringList symbolNames() const;

private:
    QString m_name;
    QMap<QString, std::shared_ptr<SymbolDef>> m_symbols;
};

} // namespace myschematic
