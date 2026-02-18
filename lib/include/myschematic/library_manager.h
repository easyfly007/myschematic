#pragma once

#include <QList>
#include <QMap>
#include <QString>
#include <memory>

namespace myschematic {

class SymbolDef;
class SymbolLibrary;

class LibraryManager {
public:
    LibraryManager();

    void addLibrary(std::shared_ptr<SymbolLibrary> library);
    void removeLibrary(const QString& name);
    SymbolLibrary* library(const QString& name) const;
    QList<SymbolLibrary*> libraries() const;

    SymbolDef* findSymbol(const QString& libraryName, const QString& symbolName) const;

    void loadBuiltins();

private:
    QMap<QString, std::shared_ptr<SymbolLibrary>> m_libraries;
};

} // namespace myschematic
