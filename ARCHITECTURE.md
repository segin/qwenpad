# ARCHITECTURE.md

## 1. Project Structure

```
[Project Root]/
├── src/              # Main source code
│   └── App/          # Application module
│       ├── src/      # Application source files
│       │   ├── main.cpp
│       │   ├── qwenpad.cpp
│       │   ├── qwenpad.h
│       │   ├── edittab.cpp
│       │   ├── edittab.h
│       │   ├── tabmanager.cpp
│       │   ├── tabmanager.h
│       │   ├── syntaxhighlighter.cpp
│       │   ├── syntaxhighlighter.h
│       │   ├── lineeditwidget.cpp
│       │   └── lineeditwidget.h
│       └── test/     # Test files
│           └── test_qwenpad.cpp
├── build/            # Build output
└── CMakeLists.txt    # Build configuration
```

## 2. High-Level System Diagram

```
[User] --> [Qwenpad (Qt6 GUI)]
                   |
                   +--> [Qwenpad]         Main window (menu bar, toolbar, status bar)
                   +--> [TabManager]      Tab widget managing multiple EditorTabs
                   +--> [EditorTab]       Editor tab (text area + line numbers)
                   +--> [SyntaxHighlighter] Syntax coloring (C/C++, Python)
                   +--> [LineEditWidget]  Custom line number display widget
```

## 3. Core Components

### 3.1. Qwenpad

**Name:** Qwenpad Text Editor

**Description:** A desktop text editor application providing a graphical user interface for creating, editing, and saving text files. The main window contains a menu bar, toolbar, central tab widget, and status bar with line/column info.

**Technologies:** C++ (Qt6), Widgets, Core

**Deployment:** Local desktop application

### 3.2. Qwenpad (qwenpad.h/cpp)

**Name:** Main Window

**Description:** The main application window class extending QMainWindow. Manages the menu bar (File, Edit, View, Help), toolbar (New, Open, Save), status bar, find/replace dialog, and delegates file operations to TabManager. Persists application settings (font, word wrap, line numbers) via QSettings.

**Technologies:** Qt6::Widgets, Qt6::Core, C++17

### 3.3. EditorTab (edittab.h/cpp)

**Name:** Editor Tab Widget

**Description:** A single editor tab widget containing a QTextEdit text area with a custom LineEditWidget for line numbers and a SyntaxHighlighter for syntax coloring. Tracks file path, dirty state, and line ending type. Provides loadFile() and saveFile() operations.

**Technologies:** Qt6::Widgets, Qt6::Core, C++17

### 3.4. TabManager (tabmanager.h/cpp)

**Name:** Tab Manager

**Description:** Extends QTabWidget to manage multiple EditorTab instances. Handles tab creation, closing, enumeration, dirty state tracking, and provides tab-level operations (find/replace, save, font/line number configuration). Emits signals for tab changes and dirty state propagation.

**Technologies:** Qt6::Widgets, Qt6::Core, C++17

### 3.5. SyntaxHighlighter (syntaxhighlighter.h/cpp)

**Name:** Syntax Highlighting Engine

**Description:** Extends QSyntaxHighlighter to provide syntax coloring via QSyntaxHighlighter's document highlighting mechanism. Supports Text (plain), C/C++, and Python languages. Uses regex-based rules for keywords, types, comments, strings, numbers, and hex values. Automatically switches language based on file extension.

**Technologies:** Qt6::Core, Qt6::Widgets, C++17

### 3.6. LineEditWidget (lineeditwidget.h/cpp)

**Name:** Line Number Widget

**Description:** A custom QWidget subclass displaying line numbers alongside the text editor. Synchronized with the editor's vertical scrollbar via signal connections. Repaints only visible line numbers during scroll for performance. Dynamically adjusts width based on the number of blocks in the document.

**Technologies:** Qt6::Widgets, Qt6::Core, C++17

## 4. Data Stores

### 4.1. Text Buffer

**Name:** In-Memory Text Buffer

**Type:** QTextDocument (Qt's flow document model)

**Purpose:** Stores the current text content being edited in the application. QTextDocument provides the underlying document model with block-based structure and cursor-based operations.

**Key Schemas:** QString-based text storage

### 4.2. Application Settings

**Name:** QSettings Configuration

**Type:** Platform-native configuration storage

**Purpose:** Persists application state across sessions: font family, font size, word wrap toggle, line numbers toggle, and line ending type preference.

**Key Schemas:** QSettings (Qwenpad namespace)

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

**Testing Framework:** QtTest (QTest) framework with dedicated test executable (qwenpad_tests). Tests cover line ending conversion, QTextDocument find/replace operations.

**Code Quality Tools:** Not yet configured

## 9. Future Considerations / Roadmap

- [x] Add syntax highlighting for different file types (C/C++, Python)
- [x] Implement search and replace functionality
- [ ] Add auto-save feature
- [x] Support for multiple file tabs
- [x] Add undo/redo functionality
- [x] Add line number display
- [ ] Add recent files list
- [ ] Add color theme support
- [ ] Expand syntax highlighting to more languages (JavaScript, JSON, XML, HTML, etc.)
- [ ] Add file encoding detection
- [ ] Add file watcher for external changes
- [ ] Add zoom in/out support
- [ ] Add mini-map or code folding
- [ ] Add full-text search across all tabs
- [ ] Improve test coverage (EditorTab, TabManager, syntax highlighting rules)

## 10. Project Identification

**Project Name:** Qwenpad

**Repository URL:** git@github.com:segin/qwenpad.git

**Primary Contact/Team:** segin

**Date of Last Update:** 2026-04-21

## 11. Glossary / Acronyms

**Qt6:** Cross-platform GUI framework used for building the application

**CMake:** Cross-platform build system generator

**C++17:** C++ programming language standard used in the project

**GUI:** Graphical User Interface provided by Qt6 Widgets

**MOC:** Meta-Object Compiler - Qt-specific preprocessor for Qt's signal/slot mechanism
