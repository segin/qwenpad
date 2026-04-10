# ARCHITECTURE.md

## 1. Project Structure

```
[Project Root]/
├── src/              # Main source code
│   └── App/          # Application module
│       └── src/      # Application source files
│           ├── main.cpp
│           ├── notepadapp.cpp
│           └── notepadapp.h
├── build/            # Build directory
├── CMakeLists.txt    # CMake configuration
├── .gitignore        # Git ignore rules
└── ARCHITECTURE.md   # This document
```

## 2. High-Level System Diagram

```
[User] --> [NotepadApp (Qt6 GUI)]
                  |
                  +--> [File Operations]
                  +--> [Text Buffer]
```

## 3. Core Components

### 3.1. NotepadApp

**Name:** Notepad Application

**Description:** A simple notepad application providing a graphical user interface for creating, editing, and saving text files. The main window contains a text buffer area where users can input and edit text content.

**Technologies:** C++ (Qt6), Widgets, Core

**Deployment:** Local desktop application

### 3.2. notepadapp.h/cpp

**Name:** Main Application Logic

**Description:** Contains the core window implementation, text buffer management, file operations (open/save), and menu handling.

**Technologies:** Qt6::Widgets, Qt6::Core, C++17

**Deployment:** N/A (part of application)

## 4. Data Stores

### 4.1. Text Buffer

**Name:** Application Memory Buffer

**Type:** In-memory data structure

**Purpose:** Stores the current text content being edited in the application.

**Key Schemas:** QString-based text storage

## 5. External Integrations / APIs

**Service Name:** Qt6 Framework

**Purpose:** Provides GUI widgets, event handling, and file I/O functionality.

**Integration Method:** Library linking (Qt6::Core, Qt6::Widgets)

## 6. Deployment & Infrastructure

**Build System:** CMake

**Qt Version:** Qt6 (Qt6::Core, Qt6::Widgets)

**Compiler:** C++17 compliant compiler

## 7. Security Considerations

**File Access:** Standard file open/save dialogs with user confirmation

**Data Handling:** Text content stored in memory until explicitly saved

**No Encryption:** Plain text storage

## 8. Development & Testing Environment

**Local Setup:** CMake-based build system

**Testing Framework:** Not yet configured

**Code Quality Tools:** Not yet configured

## 9. Future Considerations / Roadmap

- [ ] Add syntax highlighting for different file types
- [ ] Implement search and replace functionality
- [ ] Add auto-save feature
- [ ] Support for multiple file tabs
- [ ] Add undo/redo functionality
- [ ] Add line number display

## 10. Project Identification

**Project Name:** NotepadApp

**Repository URL:** git@github.com:segin/qwenpad.git

**Primary Contact/Team:** segin

**Date of Last Update:** 2026-04-09

## 11. Glossary / Acronyms

**Qt6:** Cross-platform GUI framework used for building the application

**CMake:** Cross-platform build system generator

**C++17:** C++ programming language standard used in the project

**GUI:** Graphical User Interface provided by Qt6 Widgets

**MOC:** Meta-Object Compiler - Qt-specific preprocessor for Qt's signal/slot mechanism
