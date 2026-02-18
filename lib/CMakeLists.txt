set(LIB_SOURCES
    src/model/id_generator.cpp
    src/model/schematic_element.cpp
    src/model/component.cpp
    src/model/sheet.cpp
    src/model/circuit.cpp
)

add_library(myschematic STATIC ${LIB_SOURCES})

target_include_directories(myschematic
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(myschematic PUBLIC Qt6::Core)

set_project_warnings(myschematic)
