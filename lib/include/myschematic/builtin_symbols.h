#pragma once

#include <memory>

namespace myschematic {

class SymbolDef;
class SymbolLibrary;

class BuiltinSymbols {
public:
    static std::shared_ptr<SymbolDef> createNmos4();
    static std::shared_ptr<SymbolDef> createPmos4();
    static std::shared_ptr<SymbolDef> createResistor();
    static std::shared_ptr<SymbolDef> createCapacitor();
    static std::shared_ptr<SymbolDef> createInductor();
    static std::shared_ptr<SymbolDef> createDiode();
    static std::shared_ptr<SymbolDef> createNpn();
    static std::shared_ptr<SymbolDef> createPnp();
    static std::shared_ptr<SymbolDef> createVSource();
    static std::shared_ptr<SymbolDef> createISource();
    static std::shared_ptr<SymbolDef> createVcvs();
    static std::shared_ptr<SymbolDef> createVccs();
    static std::shared_ptr<SymbolDef> createCcvs();
    static std::shared_ptr<SymbolDef> createCccs();
    static std::shared_ptr<SymbolDef> createOpamp();
    static std::shared_ptr<SymbolDef> createGnd();
    static std::shared_ptr<SymbolDef> createVdd();

    static std::shared_ptr<SymbolLibrary> createBuiltinLibrary();
};

} // namespace myschematic
