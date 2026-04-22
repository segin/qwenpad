#include "qwenpad.h"
#include "utils.h"
#include <QActionGroup>
#include <QVBoxLayout>
#include <QFrame>
#include <QRect>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QScrollBar>
#include <QMessageBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QFile>
#include <QTextStream>
#include <QStatusBar>
#include <QFontDialog>
#include <QFontDatabase>
#include <QIcon>
#include <QKeySequence>
#include <QSettings>
#include <QPaintEvent>
#include <QCloseEvent>
#include <QResizeEvent>

Qwenpad::Qwenpad(QWidget *parent)
    : QMainWindow(parent)
    , tabManager(nullptr)
    , menubar(nullptr)
    , toolbar(nullptr)
    , lineInfoLabel(nullptr)
    , findDialog(nullptr)
    , findLineEdit(nullptr)
    , replaceLineEdit(nullptr)
    , findNextButton(nullptr)
    , replaceButton(nullptr)
    , replaceAllButton(nullptr)
    , closeButton(nullptr)
    , cutAction(nullptr)
    , copyAction(nullptr)
    , pasteAction(nullptr)
    , selectAllAction(nullptr)
    , undoAction(nullptr)
    , redoAction(nullptr)
    , aboutAction(nullptr)
    , quitAction(nullptr)
    , wrapAction(nullptr)
    , lineNumbersAction(nullptr)
    , fontAction(nullptr)
    , findAction(nullptr)
    , toolbarNewAction(nullptr)
    , toolbarOpenAction(nullptr)
    , toolbarSaveAction(nullptr)
    , newAction(nullptr)
    , openAction(nullptr)
    , saveAction(nullptr)
    , closeTabAction(nullptr)
    , previousEditor(nullptr)
    , bufferDirty(false)
    , wordWrapEnabled(true)
    , lineNumbersEnabled(false)
    , currentLineEndingType(0)
{
    setMinimumSize(620, 400);
    setupUI();
}

void Qwenpad::setupUI()
{
    setupActions();
    setupMenuBar();
    setupToolBar();
    setupStatusBar();
    tabManager = new TabManager(this);
    setCentralWidget(tabManager);
    setupFindDialog();

    connect(tabManager, &TabManager::currentTabDirtyChanged, this, &Qwenpad::updateLineInfo);
    connect(tabManager, &TabManager::currentChanged, this, &Qwenpad::onCurrentTabChanged);

    onCurrentTabChanged();
}

QTextEdit *Qwenpad::currentEditor()
{
    if (tabManager && tabManager->currentTab()) {
        return tabManager->currentTab()->getEditor();
    }
    return nullptr;
}

void Qwenpad::setupActions()
{
    undoAction = new QAction(QIcon::fromTheme("edit-undo", QIcon::fromTheme("edit_undo")), tr("Undo"), this);
    undoAction->setShortcuts(QKeySequence::Undo);
    undoAction->setToolTip(tr("Undo"));

    redoAction = new QAction(QIcon::fromTheme("edit-redo", QIcon::fromTheme("edit_redo")), tr("Redo"), this);
    redoAction->setShortcuts(QKeySequence::Redo);
    redoAction->setToolTip(tr("Redo"));

    cutAction = new QAction(QIcon::fromTheme("edit-cut", QIcon::fromTheme("edit_cut")), tr("Cut"), this);
    cutAction->setShortcuts(QKeySequence::Cut);
    cutAction->setToolTip(tr("Cut"));

    copyAction = new QAction(QIcon::fromTheme("edit-copy", QIcon::fromTheme("edit_copy")), tr("Copy"), this);
    copyAction->setShortcuts(QKeySequence::Copy);
    copyAction->setToolTip(tr("Copy"));

    pasteAction = new QAction(QIcon::fromTheme("edit-paste", QIcon::fromTheme("edit_paste")), tr("Paste"), this);
    pasteAction->setShortcut(QKeySequence::Paste);
    pasteAction->setToolTip(tr("Paste"));

    selectAllAction = new QAction(QIcon::fromTheme("edit-select-all", QIcon::fromTheme("select_all")), tr("Select All"), this);
    selectAllAction->setShortcuts(QKeySequence::SelectAll);

    newAction = new QAction(QIcon::fromTheme("document-new"), tr("&New"), this);
    newAction->setShortcut(QKeySequence::New);
    connect(newAction, &QAction::triggered, this, &Qwenpad::onNew);

    openAction = new QAction(QIcon::fromTheme("document-open"), tr("&Open"), this);
    openAction->setShortcut(QKeySequence::Open);
    connect(openAction, &QAction::triggered, this, &Qwenpad::onOpen);

    saveAction = new QAction(QIcon::fromTheme("document-save"), tr("&Save"), this);
    saveAction->setShortcut(QKeySequence::Save);
    connect(saveAction, &QAction::triggered, this, &Qwenpad::onSave);

    closeTabAction = new QAction(tr("Close Tab"), this);
    closeTabAction->setShortcut(QKeySequence("Ctrl+W"));
    connect(closeTabAction, &QAction::triggered, this, &Qwenpad::onCloseTab);

    toolbarNewAction = new QAction(QIcon::fromTheme("document-new"), tr("New"), this);
    connect(toolbarNewAction, &QAction::triggered, this, &Qwenpad::onNew);

    toolbarOpenAction = new QAction(QIcon::fromTheme("document-open"), tr("Open"), this);
    connect(toolbarOpenAction, &QAction::triggered, this, &Qwenpad::onOpen);

    toolbarSaveAction = new QAction(QIcon::fromTheme("document-save"), tr("Save"), this);
    connect(toolbarSaveAction, &QAction::triggered, this, &Qwenpad::onSave);

    aboutAction = new QAction(QIcon::fromTheme("help-about", QIcon::fromTheme("system_help")), tr("About"), this);
    aboutAction->setShortcut(QKeySequence("F1"));
    connect(aboutAction, &QAction::triggered, this, &Qwenpad::onAbout);

    quitAction = new QAction(tr("Quit"), this);
    quitAction->setShortcut(QKeySequence("Ctrl+Q"));
    connect(quitAction, &QAction::triggered, this, &QMainWindow::close);

    wrapAction = new QAction(tr("Word Wrap"), this);
    wrapAction->setShortcut(QKeySequence("Ctrl+Shift+W"));
    wrapAction->setCheckable(true);
    wrapAction->setChecked(true);
    connect(wrapAction, &QAction::triggered, this, &Qwenpad::onWrap);

    lineNumbersAction = new QAction(tr("Line Numbers"), this);
    lineNumbersAction->setShortcut(QKeySequence("Ctrl+L"));
    lineNumbersAction->setCheckable(true);
    lineNumbersAction->setChecked(false);
    connect(lineNumbersAction, &QAction::triggered, this, &Qwenpad::onToggleLineNumbers);

    fontAction = new QAction(tr("Font..."), this);
    fontAction->setShortcut(QKeySequence("Ctrl+F"));
    connect(fontAction, &QAction::triggered, this, &Qwenpad::onFont);

    findAction = new QAction(QIcon::fromTheme("edit-find", QIcon::fromTheme("edit_find")), tr("Find/Replace..."), this);
    findAction->setShortcut(QKeySequence("Ctrl+F"));
    connect(findAction, &QAction::triggered, this, &Qwenpad::onFind);

    connect(undoAction, &QAction::triggered, this, &Qwenpad::onUndo);
    connect(redoAction, &QAction::triggered, this, &Qwenpad::onRedo);
}

void Qwenpad::setupMenuBar()
{
    menubar = new QMenuBar(this);

    auto *fileMenu = new QMenu(tr("File"), menubar);
    fileMenu->addAction(newAction);
    fileMenu->addAction(openAction);
    fileMenu->addAction(saveAction);
    fileMenu->addSeparator();
    fileMenu->addAction(closeTabAction);

    fileMenu->addSeparator();
    fileMenu->addAction(quitAction);

    auto *editMenu = new QMenu(tr("Edit"), menubar);
    editMenu->addAction(undoAction);
    editMenu->addAction(redoAction);
    editMenu->addSeparator();
    editMenu->addAction(cutAction);
    editMenu->addAction(copyAction);
    editMenu->addAction(pasteAction);
    editMenu->addSeparator();
    editMenu->addAction(selectAllAction);
    editMenu->addSeparator();
    editMenu->addAction(findAction);

    auto *viewMenu = new QMenu(tr("View"), menubar);
    viewMenu->addAction(wrapAction);
    viewMenu->addAction(lineNumbersAction);
    viewMenu->addSeparator();
    viewMenu->addSection(tr("Syntax Highlighting"));

    auto *syntaxGroup = new QActionGroup(this);
    QAction *syntaxNone = syntaxGroup->addAction(tr("None"));
    syntaxNone->setCheckable(true);
    syntaxNone->setChecked(true);
    QAction *syntaxCpp = syntaxGroup->addAction(tr("C/C++"));
    syntaxCpp->setCheckable(true);
    QAction *syntaxPython = syntaxGroup->addAction(tr("Python"));
    syntaxPython->setCheckable(true);

    syntaxGroup->addAction(syntaxNone);
    syntaxGroup->addAction(syntaxCpp);
    syntaxGroup->addAction(syntaxPython);

    connect(syntaxNone, &QAction::triggered, this, [this]() {
        EditorTab *tab = tabManager->currentTab();
        if (tab) {
            tab->setHighlighterLanguage("Text");
        }
    });
    connect(syntaxCpp, &QAction::triggered, this, [this]() {
        EditorTab *tab = tabManager->currentTab();
        if (tab) {
            tab->setHighlighterLanguage("C++");
        }
    });
    connect(syntaxPython, &QAction::triggered, this, [this]() {
        EditorTab *tab = tabManager->currentTab();
        if (tab) {
            tab->setHighlighterLanguage("Python");
        }
    });

    viewMenu->addAction(syntaxNone);
    viewMenu->addAction(syntaxCpp);
    viewMenu->addAction(syntaxPython);
    viewMenu->addSeparator();
    viewMenu->addAction(fontAction);

    auto *helpMenu = new QMenu(tr("Help"), menubar);
    helpMenu->addAction(aboutAction);
    helpMenu->addSeparator();
    helpMenu->addAction(new QAction(tr("&Documentation"), this));

    menubar->addMenu(fileMenu);
    menubar->addMenu(editMenu);
    menubar->addMenu(viewMenu);
    menubar->addMenu(helpMenu);
    setMenuBar(menubar);
}

void Qwenpad::setupToolBar()
{
    toolbar = new QToolBar(this);
    toolbar->setIconSize(QSize(16, 16));

    toolbar->addAction(toolbarNewAction);
    toolbar->addAction(toolbarOpenAction);
    toolbar->addAction(toolbarSaveAction);
    toolbar->addSeparator();
    toolbar->addAction(undoAction);
    toolbar->addAction(redoAction);
    toolbar->addSeparator();
    toolbar->addAction(cutAction);
    toolbar->addAction(copyAction);
    toolbar->addAction(pasteAction);
    addToolBar(toolbar);
}

void Qwenpad::setupStatusBar()
{
    setStatusBar(new QStatusBar(this));
    lineInfoLabel = new QLabel(tr("Ln 0, Col 0"));
    statusBar()->addPermanentWidget(lineInfoLabel);
    statusBar()->showMessage(tr("Qwenpad v0.1"));
}

void Qwenpad::setupFindDialog()
{
    findDialog = new QDialog(this);
    findDialog->setWindowTitle(tr("Find"));
    findDialog->setMinimumWidth(300);

    QGridLayout *layout = new QGridLayout(findDialog);

    findLineEdit = new QLineEdit();
    findLineEdit->setPlaceholderText(tr("Enter text to find..."));
    layout->addWidget(findLineEdit, 0, 0);

    replaceLineEdit = new QLineEdit();
    replaceLineEdit->setPlaceholderText(tr("Enter replacement text (optional)..."));
    layout->addWidget(replaceLineEdit, 1, 0);

    findNextButton = new QPushButton(tr("Find Next"));
    layout->addWidget(findNextButton, 2, 0);

    replaceButton = new QPushButton(tr("Replace"));
    layout->addWidget(replaceButton, 3, 0);

    replaceAllButton = new QPushButton(tr("Replace All"));
    layout->addWidget(replaceAllButton, 4, 0);

    closeButton = new QPushButton(tr("Close"));
    layout->addWidget(closeButton, 5, 0);

    connect(findNextButton, &QPushButton::clicked, this, &Qwenpad::onFindNext);
    connect(replaceButton, &QPushButton::clicked, this, &Qwenpad::onReplace);
    connect(replaceAllButton, &QPushButton::clicked, this, &Qwenpad::onReplaceAll);
    connect(closeButton, &QPushButton::clicked, this, &Qwenpad::onCloseFindDialog);
    connect(findLineEdit, &QLineEdit::returnPressed, this, &Qwenpad::onFindNext);
}

void Qwenpad::onNew()
{
    tabManager->addTab(tr("(untitled)"));
    setDirty(false);
}

void Qwenpad::onOpen()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
        QString(), tr("All Files (*);;Text Files (*.txt);;C/C++ Files (*.c *.cpp *.hpp *.h);;Python Files (*.py *.pyw)"));

    if (!fileName.isEmpty()) {
        EditorTab *tab = nullptr;
        EditorTab *current = tabManager->currentTab();
        if (current && (current->getFile().isEmpty() || current->getFile() == tr("(untitled)")) && !current->isDirty()) {
            tab = current;
        } else {
            tab = tabManager->addTab(tr("(untitled)"));
        }
        tab->loadFile(fileName);
        tab->setFile(fileName);
        tabManager->updateCurrentTabTitle();

        detectLineEndings();

        QSettings settings("Qwenpad", "Qwenpad");
        if (settings.contains("FontFamily")) {
            QFont restoredFont;
            restoredFont.setFamily(settings.value("FontFamily").toString());
            restoredFont.setPointSize(settings.value("FontSize").toInt());
            tabManager->setupFont(restoredFont);
        }
        if (settings.contains("WordWrapEnabled")) {
            wrapAction->setChecked(settings.value("WordWrapEnabled").toBool());
            onWrap();
        }
        if (settings.contains("LineNumbersEnabled")) {
            lineNumbersAction->setChecked(settings.value("LineNumbersEnabled").toBool());
            onToggleLineNumbers();
        }
        if (settings.contains("LineEndingType")) {
            currentLineEndingType = settings.value("LineEndingType").toInt();
        }
        setDirty(false);
    }
}

void Qwenpad::onSave()
{
    if (tabManager) {
        tabManager->saveCurrentFile();
    }
}

void Qwenpad::onCut()
{
    QTextEdit *editor = currentEditor();
    if (editor) {
        editor->cut();
    }
}

void Qwenpad::onCopy()
{
    QTextEdit *editor = currentEditor();
    if (editor) {
        editor->copy();
    }
}

void Qwenpad::onPaste()
{
    QTextEdit *editor = currentEditor();
    if (editor) {
        editor->paste();
    }
}

void Qwenpad::onSelectAll()
{
    QTextEdit *editor = currentEditor();
    if (editor) {
        editor->selectAll();
    }
}

void Qwenpad::onAbout()
{
    QMessageBox::information(this, tr("About"), tr("Qwenpad v0.1"));
}

void Qwenpad::onWrap()
{
    QTextOption::WrapMode mode = wrapAction->isChecked()
        ? QTextOption::WrapMode::WordWrap
        : QTextOption::WrapMode::NoWrap;
    for (int i = 0; i < tabManager->count(); i++) {
        EditorTab *tab = tabManager->getEditorTab(i);
        if (tab) {
            tab->getEditor()->setWordWrapMode(mode);
        }
    }
}

void Qwenpad::onToggleLineNumbers()
{
    lineNumbersEnabled = lineNumbersAction->isChecked();
    tabManager->setupLineNumbers(lineNumbersEnabled);
}

void Qwenpad::onFont()
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok, this);
    if (ok) {
        tabManager->setupFont(font);
    }
}

void Qwenpad::onConvertCrlf()
{
    tabManager->convertLineEndings(1);
    currentLineEndingType = 1;
    setDirty(true);
    detectLineEndings();
}

void Qwenpad::onConvertLf()
{
    tabManager->convertLineEndings(0);
    currentLineEndingType = 0;
    setDirty(true);
    detectLineEndings();
}

void Qwenpad::onConvertCr()
{
    tabManager->convertLineEndings(2);
    currentLineEndingType = 2;
    setDirty(true);
    detectLineEndings();
}

void Qwenpad::detectLineEndings()
{
    tabManager->detectLineEndings();
}

void Qwenpad::setDirty(bool dirty)
{
    bufferDirty = dirty;
    tabManager->setCurrentTabDirty(dirty);
    updateLineInfo();
}

void Qwenpad::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
}

void Qwenpad::closeEvent(QCloseEvent *event)
{
    bool allSaved = true;
    for (int i = 0; i < tabManager->count(); i++) {
        EditorTab *tab = tabManager->getEditorTab(i);
        if (tab && tab->isDirty()) {
            QString savedFile = askSave(tab);
            if (savedFile.isEmpty()) {
                event->ignore();
                return;
            }
            if (tab->isDirty()) {
                allSaved = false;
            }
        }
    }

    if (allSaved) {
        event->accept();
    } else {
        event->ignore();
    }
}

void Qwenpad::updateLineInfo()
{
    QTextEdit *editor = currentEditor();
    if (editor) {
        QTextCursor cursor = editor->textCursor();
        int line = cursor.blockNumber() + 1;
        int col = cursor.positionInBlock() + 1;
        lineInfoLabel->setText(tr("Ln %1, Col %2").arg(line).arg(col));
    }
}

void Qwenpad::onFind()
{
    if (findDialog) {
        findDialog->show();
        findLineEdit->setFocus();
    }
}

void Qwenpad::onFindNext()
{
    QString searchText = findLineEdit->text();
    if (searchText.isEmpty()) {
        QMessageBox::information(findDialog, tr("Find"), tr("Please enter text to find"));
        return;
    }

    bool found = tabManager->findNext(searchText);
    if (!found) {
        QMessageBox::information(findDialog, tr("Find"), tr("Text not found"));
    }
}

void Qwenpad::onReplace()
{
    QString searchText = findLineEdit->text();
    QString replaceText = replaceLineEdit->text();

    if (searchText.isEmpty()) {
        QMessageBox::information(findDialog, tr("Replace"), tr("Please enter text to find"));
        return;
    }

    bool found = tabManager->replace(searchText, replaceText);
    if (!found) {
        QMessageBox::information(findDialog, tr("Replace"), tr("Text not found"));
    }
}

void Qwenpad::onReplaceAll()
{
    QString searchText = findLineEdit->text();
    QString replaceText = replaceLineEdit->text();

    if (searchText.isEmpty()) {
        QMessageBox::information(findDialog, tr("Replace All"), tr("Please enter text to find"));
        return;
    }

    int count = tabManager->replaceAll(searchText, replaceText);
    if (count == 0) {
        QMessageBox::information(findDialog, tr("Replace All"), tr("Text not found"));
    }
}

void Qwenpad::onCloseFindDialog()
{
    findDialog->close();
}

void Qwenpad::onCloseTab()
{
    tabManager->closeCurrentTab();
}

QString Qwenpad::askSave(EditorTab *tab)
{
    int reply = QMessageBox::question(this, tr("Save Changes"),
        tr("The current file has unsaved changes. Do you want to save them?"),
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

    if (reply == QMessageBox::Save) {
        QString fileName = tab->getFile();
        if (!fileName.isEmpty()) {
            tab->saveFile(fileName);
        } else {
            QString savedFile = QFileDialog::getSaveFileName(this, tr("Save File"),
                QString(), tr("Text Files (*.txt)"));
            if (!savedFile.isEmpty()) {
                tab->saveFile(savedFile);
            } else {
                return QString();
            }
        }
        return tab->getFile();
    } else if (reply == QMessageBox::Cancel) {
        return QString();
    }

    tab->setDirty(false);
    return QString();
}

void Qwenpad::onCurrentTabChanged()
{
    QTextEdit *editor = currentEditor();
    if (previousEditor) {
        disconnect(previousEditor->document(), &QTextDocument::undoAvailable, undoAction, nullptr);
        disconnect(previousEditor->document(), &QTextDocument::redoAvailable, redoAction, nullptr);
    }

    if (editor) {
        QTextDocument *doc = editor->document();
        connect(doc, &QTextDocument::undoAvailable, this, [this](bool available) {
            undoAction->setEnabled(available);
        });
        connect(doc, &QTextDocument::redoAvailable, this, [this](bool available) {
            redoAction->setEnabled(available);
        });

        undoAction->setEnabled(doc->isUndoAvailable());
        redoAction->setEnabled(doc->isRedoAvailable());
    } else {
        undoAction->setEnabled(false);
        redoAction->setEnabled(false);
    }

    previousEditor = editor;
}

void Qwenpad::onUndo()
{
    QTextEdit *editor = currentEditor();
    if (editor) {
        editor->undo();
    }
}

void Qwenpad::onRedo()
{
    QTextEdit *editor = currentEditor();
    if (editor) {
        editor->redo();
    }
}
