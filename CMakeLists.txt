cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(Qwenpad LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Test)

add_executable(Qwenpad
    src/App/src/main.cpp
    src/App/src/qwenpad.cpp
    src/App/src/qwenpad.h
    src/App/src/lineeditwidget.cpp
    src/App/src/lineeditwidget.h
)

target_link_libraries(Qwenpad PRIVATE Qt6::Core Qt6::Widgets)

add_executable(qwenpad_tests
    src/App/test/test_qwenpad.cpp
    src/App/src/qwenpad.cpp
    src/App/src/lineeditwidget.cpp
)

target_include_directories(qwenpad_tests PRIVATE ${CMAKE_SOURCE_DIR}/src/App/src)
target_link_libraries(qwenpad_tests PRIVATE Qt6::Core Qt6::Widgets Qt6::Test)
set_target_properties(qwenpad_tests PROPERTIES AUTOMOC ON)
