set(APP_SOURCES
    main.cpp
    mainwindow.cpp
    view/schematic_scene.cpp
    view/schematic_view.cpp
    view/items/component_item.cpp
    tools/tool.cpp
    tools/tool_manager.cpp
    tools/select_tool.cpp
    tools/place_tool.cpp
    commands/add_component_cmd.cpp
    commands/delete_element_cmd.cpp
    commands/move_element_cmd.cpp
)

add_executable(myschematic_app ${APP_SOURCES})

target_include_directories(myschematic_app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(myschematic_app PRIVATE myschematic Qt6::Core Qt6::Widgets)

set_target_properties(myschematic_app PROPERTIES OUTPUT_NAME myschematic)

set_project_warnings(myschematic_app)
