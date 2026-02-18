set(LIB_HEADERS
    include/myschematic/types.h
    include/myschematic/geometry.h
    include/myschematic/schematic_element.h
    include/myschematic/component.h
    include/myschematic/sheet.h
    include/myschematic/circuit.h
    include/myschematic/id_generator.h
    include/myschematic/myschematic.h
    include/myschematic/property.h
    include/myschematic/symbol_pin.h
    include/myschematic/symbol_graphic.h
    include/myschematic/symbol_def.h
    include/myschematic/symbol_library.h
    include/myschematic/library_manager.h
    include/myschematic/builtin_symbols.h
    include/myschematic/pin.h
)

set(LIB_SOURCES
    src/model/id_generator.cpp
    src/model/schematic_element.cpp
    src/model/component.cpp
    src/model/sheet.cpp
    src/model/circuit.cpp
    src/model/property.cpp
    src/model/pin.cpp
    src/symbol/symbol_def.cpp
    src/symbol/symbol_library.cpp
    src/symbol/library_manager.cpp
    src/symbol/builtin_symbols.cpp
)

add_library(myschematic STATIC ${LIB_SOURCES} ${LIB_HEADERS})

target_include_directories(myschematic
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(myschematic PUBLIC Qt6::Core)

set_project_warnings(myschematic)
