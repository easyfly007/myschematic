cmake_minimum_required(VERSION 3.16)
project(myschematic VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

# Include custom cmake modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(CompilerWarnings)

find_package(Qt6 REQUIRED COMPONENTS Core)

add_subdirectory(lib)

option(BUILD_APP "Build GUI application (requires Qt6::Widgets)" ON)
if(BUILD_APP)
    find_package(Qt6 REQUIRED COMPONENTS Widgets)
    add_subdirectory(app)
endif()

option(BUILD_TESTS "Build tests" ON)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
