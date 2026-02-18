#include "myschematic/symbol_library.h"

#include "myschematic/symbol_def.h"

namespace myschematic {

SymbolLibrary::SymbolLibrary(const QString& name)
    : m_name(name)
{
}

QString SymbolLibrary::name() const
{
    return m_name;
}

void SymbolLibrary::addSymbol(std::shared_ptr<SymbolDef> symbol)
{
    symbol->setLibraryName(m_name);
    m_symbols.insert(symbol->name(), std::move(symbol));
}

void SymbolLibrary::removeSymbol(const QString& name)
{
    m_symbols.remove(name);
}

SymbolDef* SymbolLibrary::findSymbol(const QString& name) const
{
    auto it = m_symbols.constFind(name);
    if (it != m_symbols.constEnd()) {
        return it->get();
    }
    return nullptr;
}

QList<SymbolDef*> SymbolLibrary::symbols() const
{
    QList<SymbolDef*> result;
    for (const auto& sym : m_symbols) {
        result.append(sym.get());
    }
    return result;
}

QStringList SymbolLibrary::symbolNames() const
{
    return m_symbols.keys();
}

} // namespace myschematic
