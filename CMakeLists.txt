cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(Qwenpad LANGUAGES CXX)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

set(MOC_SRCS ${CMAKE_CURRENT_BINARY_DIR}/qwenpad_moc.cpp)
add_custom_command(
    OUTPUT ${MOC_SRCS}
    COMMAND /usr/lib/qt6/moc -o ${MOC_SRCS} ${CMAKE_SOURCE_DIR}/src/App/src/qwenpad.h
    DEPENDS ${CMAKE_SOURCE_DIR}/src/App/src/qwenpad.h
)

add_executable(Qwenpad
    src/App/src/main.cpp
    src/App/src/qwenpad.cpp
    ${MOC_SRCS}
)

target_include_directories(Qwenpad PRIVATE
    /usr/include/qt6
    /usr/include/qt6/QtWidgets
    /usr/include/qt6/QtGui
    /usr/include/qt6/QtCore
)

target_link_libraries(Qwenpad PRIVATE Qt6::Core Qt6::Widgets)

target_compile_definitions(Qwenpad PRIVATE
    QT_CORE_LIB
    QT_GUI_LIB
    QT_WIDGETS_LIB
)
