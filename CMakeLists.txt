cmake_minimum_required(VERSION 3.20)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set project name
project(NotepadApp LANGUAGES CXX)

# Find Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

# Add executable
add_executable(NotepadApp
    src/App/src/main.cpp
)

# Link libraries
target_link_libraries(NotepadApp PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

# Set public methods using compile_definitions with different macro format
target_compile_definitions(NotepadApp PRIVATE
    PUBLIC_METHODS=setupUI onNew onOpen onSave onCut onCopy onPaste onselectAll
    PUBLIC_MACRO=setupUI onNew onOpen onSave onCut onCopy onPaste onselectAll
)
