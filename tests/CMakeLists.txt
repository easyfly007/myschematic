find_package(GTest REQUIRED)

# Helper function to add a test
function(add_myschematic_test test_name test_source)
    add_executable(${test_name} ${test_source})
    target_link_libraries(${test_name} PRIVATE myschematic GTest::GTest GTest::Main Qt6::Core)
    target_include_directories(${test_name} PRIVATE ${CMAKE_SOURCE_DIR}/app)
    add_test(NAME ${test_name} COMMAND ${test_name})
    set_project_warnings(${test_name})
endfunction()

add_myschematic_test(test_circuit test_model/test_circuit.cpp)
add_myschematic_test(test_sheet test_model/test_sheet.cpp)
add_myschematic_test(test_component test_model/test_component.cpp)
add_myschematic_test(test_move_cmd test_commands/test_move_cmd.cpp)
add_myschematic_test(test_add_delete_cmd test_commands/test_add_delete_cmd.cpp)

add_myschematic_test(test_symbol_def test_symbol/test_symbol_def.cpp)
add_myschematic_test(test_builtin_symbols test_symbol/test_builtin_symbols.cpp)
add_myschematic_test(test_component_extended test_symbol/test_component_extended.cpp)
add_myschematic_test(test_library test_symbol/test_library.cpp)
