#include "myschematic/library_manager.h"

#include "myschematic/builtin_symbols.h"
#include "myschematic/symbol_library.h"

namespace myschematic {

LibraryManager::LibraryManager() = default;

void LibraryManager::addLibrary(std::shared_ptr<SymbolLibrary> library)
{
    m_libraries.insert(library->name(), std::move(library));
}

void LibraryManager::removeLibrary(const QString& name)
{
    m_libraries.remove(name);
}

SymbolLibrary* LibraryManager::library(const QString& name) const
{
    auto it = m_libraries.constFind(name);
    if (it != m_libraries.constEnd()) {
        return it->get();
    }
    return nullptr;
}

QList<SymbolLibrary*> LibraryManager::libraries() const
{
    QList<SymbolLibrary*> result;
    for (const auto& lib : m_libraries) {
        result.append(lib.get());
    }
    return result;
}

SymbolDef* LibraryManager::findSymbol(const QString& libraryName,
                                      const QString& symbolName) const
{
    auto* lib = library(libraryName);
    if (lib) {
        return lib->findSymbol(symbolName);
    }
    return nullptr;
}

void LibraryManager::loadBuiltins()
{
    addLibrary(BuiltinSymbols::createBuiltinLibrary());
}

} // namespace myschematic
