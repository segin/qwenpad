#include <QTimer>
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
#include <QUrl>
#include <QDrag>
#include <QMimeData>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QClipboard>

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
    , fontAction(nullptr)
    , findAction(nullptr)
    , toolbarNewAction(nullptr)
    , toolbarOpenAction(nullptr)
    , toolbarSaveAction(nullptr)
    , newAction(nullptr)
    , openAction(nullptr)
    , saveAction(nullptr)
    , closeTabAction(nullptr)
    , statusLineEndingLabel(nullptr)
    , statusLanguageLabel(nullptr)
    , goToLineDialog(nullptr)
    , goToLineEdit(nullptr)
    , goToLineAction(nullptr)
    , previousEditor(nullptr)
    ,    bufferDirty(false)
    , wordWrapEnabled(true)
    , untitledTabCount(0)
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
    setupGoToLineDialog();

    connect(tabManager, &TabManager::currentTabDirtyChanged, this, &Qwenpad::updateLineInfo);
    connect(tabManager, &TabManager::currentChanged, this, &Qwenpad::onCurrentTabChanged);
    connect(tabManager, &TabManager::currentChanged, this, &Qwenpad::updateStatusBar);

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
    connect(quitAction, &QAction::triggered, this, [this]() {
        QTimer::singleShot(0, this, &QMainWindow::close);
    });

    wrapAction = new QAction(tr("Word Wrap"), this);
    wrapAction->setShortcut(QKeySequence("Ctrl+Shift+W"));
    wrapAction->setCheckable(true);
    wrapAction->setChecked(true);
    connect(wrapAction, &QAction::triggered, this, &Qwenpad::onWrap);

    fontAction = new QAction(tr("Font..."), this);
    fontAction->setShortcut(QKeySequence("Ctrl+F"));
    connect(fontAction, &QAction::triggered, this, &Qwenpad::onFont);

    findAction = new QAction(QIcon::fromTheme("edit-find", QIcon::fromTheme("edit_find")), tr("Find/Replace..."), this);
    findAction->setShortcut(QKeySequence("Ctrl+F"));
    connect(findAction, &QAction::triggered, this, &Qwenpad::onFind);

    goToLineAction = new QAction(tr("Go to Line..."), this);
    goToLineAction->setShortcut(QKeySequence("Ctrl+G"));
    connect(goToLineAction, &QAction::triggered, this, &Qwenpad::onGoToLine);

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
    editMenu->addAction(goToLineAction);

    auto *viewMenu = new QMenu(tr("View"), menubar);
    viewMenu->addAction(wrapAction);
    viewMenu->addSeparator();
    {
        QAction *zoomInAction = new QAction(tr("Zoom In"), viewMenu);
        zoomInAction->setShortcut(QKeySequence("Ctrl+Plus"));
        connect(zoomInAction, &QAction::triggered, this, &Qwenpad::onZoomIn);
        viewMenu->addAction(zoomInAction);
    }
    {
        QAction *zoomOutAction = new QAction(tr("Zoom Out"), viewMenu);
        zoomOutAction->setShortcut(QKeySequence("Ctrl+Minus"));
        connect(zoomOutAction, &QAction::triggered, this, &Qwenpad::onZoomOut);
        viewMenu->addAction(zoomOutAction);
    }
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

    statusLineEndingLabel = new QPushButton(tr("LF"));
    statusLineEndingLabel->setFlat(true);
    QMenu *leMenu = new QMenu(this);
    QAction *leLf = leMenu->addAction(tr("LF (Unix)"));
    leLf->setData("LF");
    QAction *leCrlf = leMenu->addAction(tr("CRLF (Windows)"));
    leCrlf->setData("CRLF");
    QAction *leCr = leMenu->addAction(tr("CR (Mac)"));
    leCr->setData("CR");
    connect(leLf, &QAction::triggered, this, &Qwenpad::onStatusLineEndingClicked);
    connect(leCrlf, &QAction::triggered, this, &Qwenpad::onStatusLineEndingClicked);
    connect(leCr, &QAction::triggered, this, &Qwenpad::onStatusLineEndingClicked);
    leMenu->setStyleSheet("QMenu { border: none; }");
    statusLineEndingLabel->setMenu(leMenu);
    statusBar()->addPermanentWidget(statusLineEndingLabel);

    statusLanguageLabel = new QPushButton(tr("Text"));
    statusLanguageLabel->setFlat(true);
    QMenu *langMenu = new QMenu(this);
    QAction *langNone = langMenu->addAction(tr("None"));
    langNone->setData("Text");
    QAction *langCpp = langMenu->addAction(tr("C/C++"));
    langCpp->setData("C++");
    QAction *langPy = langMenu->addAction(tr("Python"));
    langPy->setData("Python");
    QAction *langJson = langMenu->addAction(tr("JSON"));
    langJson->setData("JSON");
    QAction *langShell = langMenu->addAction(tr("Shell"));
    langShell->setData("Shell");
    QAction *langXml = langMenu->addAction(tr("XML/HTML"));
    langXml->setData("XML");
    connect(langNone, &QAction::triggered, this, &Qwenpad::onStatusLanguageClicked);
    connect(langCpp, &QAction::triggered, this, &Qwenpad::onStatusLanguageClicked);
    connect(langPy, &QAction::triggered, this, &Qwenpad::onStatusLanguageClicked);
    connect(langJson, &QAction::triggered, this, &Qwenpad::onStatusLanguageClicked);
    connect(langShell, &QAction::triggered, this, &Qwenpad::onStatusLanguageClicked);
    connect(langXml, &QAction::triggered, this, &Qwenpad::onStatusLanguageClicked);
    langMenu->setStyleSheet("QMenu { border: none; }");
    statusLanguageLabel->setMenu(langMenu);
    statusBar()->addPermanentWidget(statusLanguageLabel);

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

    regexCheckBox = new QCheckBox(tr("Use Regular Expressions"));
    layout->addWidget(regexCheckBox, 2, 0);

    findNextButton = new QPushButton(tr("Find Next"));
    layout->addWidget(findNextButton, 3, 0);

    replaceButton = new QPushButton(tr("Replace"));
    layout->addWidget(replaceButton, 4, 0);

    replaceAllButton = new QPushButton(tr("Replace All"));
    layout->addWidget(replaceAllButton, 5, 0);

    closeButton = new QPushButton(tr("Close"));
    layout->addWidget(closeButton, 6, 0);

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

void Qwenpad::openFiles(const QStringList &files)
{
    if (files.isEmpty()) {
        restoreSession();
        return;
    }

    for (const QString &fileName : files) {
        QString path = QUrl(fileName).toLocalFile();
        if (path.isEmpty()) {
            path = fileName;
        }

        if (!QFile::exists(path)) {
            continue;
        }

        EditorTab *tab = nullptr;
        EditorTab *current = tabManager->currentTab();
        if (current && (current->getFile().isEmpty() || current->getFile() == tr("(untitled)")) && !current->isDirty()) {
            tab = current;
        } else {
            tab = tabManager->addTab(tr("(untitled)"));
        }
        tab->loadFile(path);
        tab->setFile(path);
        tabManager->updateCurrentTabTitle();
    }

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
    if (settings.contains("LineEndingType")) {
        int lineEndingType = settings.value("LineEndingType").toInt();
        tabManager->convertLineEndings(lineEndingType);
        currentLineEndingType = lineEndingType;
    }
    if (settings.contains("HighlighterLanguage")) {
        QString savedLanguage = settings.value("HighlighterLanguage").toString();
        currentHighlighterLanguage = savedLanguage;
        EditorTab *tab = tabManager->currentTab();
        if (tab) {
            tab->setHighlighterLanguage(savedLanguage);
        }
    }
}

void Qwenpad::onOpen()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
        QString(), tr("All Files (*);;Text Files (*.txt);;C/C++ Files (*.c *.cpp *.hpp *.h);;Python Files (*.py *.pyw)"));

    if (!fileName.isEmpty()) {
        openFiles(QStringList{fileName});
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
    saveSession();

    bool allSaved = true;
    for (int i = 0; i < tabManager->count(); i++) {
        EditorTab *tab = tabManager->getEditorTab(i);
        if (tab && tab->isDirty()) {
            bool proceed = askSave(tab);
            if (!proceed) {
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

   bool useRegex = regexCheckBox->isChecked();
    if (useRegex) {
        QTextEdit *editor = currentEditor();
        if (!editor) return;

        QRegularExpression regex(searchText);
        QString documentText = editor->document()->toPlainText();
        int startPos = editor->textCursor().position();
        QRegularExpressionMatch match = regex.match(documentText, startPos);
        if (!match.hasMatch()) {
            match = regex.match(documentText);
            if (match.hasMatch()) {
                startPos = 0;
            }
        }
        if (match.hasMatch()) {
            QTextCursor selCursor(editor->document());
            selCursor.setPosition(match.capturedStart());
            selCursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor, match.capturedLength());
            editor->setTextCursor(selCursor);
            editor->ensureCursorVisible();
        } else {
            QMessageBox::information(findDialog, tr("Find"), tr("Text not found"));
        }
    } else {
        bool found = tabManager->findNext(searchText);
        if (!found) {
            QMessageBox::information(findDialog, tr("Find"), tr("Text not found"));
        }
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

    bool useRegex = regexCheckBox->isChecked();
    if (useRegex) {
        QTextEdit *editor = currentEditor();
        if (!editor) return;

        QTextCursor cursor = editor->textCursor();
        QString selectedText = cursor.selectedText();

        QRegularExpression regex(searchText);
        if (!selectedText.isEmpty() && regex.match(selectedText).hasMatch()) {
            cursor.insertText(replaceText);
            editor->setTextCursor(cursor);
            editor->ensureCursorVisible();
        } else {
            QString content = editor->toPlainText();
            QRegularExpressionMatch match = regex.match(content, cursor.position());
            if (!match.hasMatch()) {
                match = regex.match(content);
            }
            if (match.hasMatch()) {
                QTextCursor selCursor(editor->document());
                selCursor.setPosition(match.capturedStart());
                selCursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor, match.capturedLength());
                selCursor.insertText(replaceText);
                editor->setTextCursor(selCursor);
                editor->ensureCursorVisible();
            } else {
                QMessageBox::information(findDialog, tr("Replace"), tr("Text not found"));
            }
        }
    } else {
        bool found = tabManager->replace(searchText, replaceText);
        if (!found) {
            QMessageBox::information(findDialog, tr("Replace"), tr("Text not found"));
        }
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

    bool useRegex = regexCheckBox->isChecked();
    if (useRegex) {
        QTextEdit *editor = currentEditor();
        if (!editor) return;

        QRegularExpression regex(searchText);
        QString content = editor->toPlainText();

        if (content.count(regex) > 0) {
            content.replace(regex, replaceText);
            QTextCursor cursor(editor->document());
            cursor.beginEditBlock();
            cursor.movePosition(QTextCursor::Start);
            cursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
            cursor.insertText(content);
            cursor.endEditBlock();
            tabManager->updateCurrentTabTitle();
        } else {
            QMessageBox::information(findDialog, tr("Replace All"), tr("Text not found"));
        }
    } else {
        int count = tabManager->replaceAll(searchText, replaceText);
        if (count == 0) {
            QMessageBox::information(findDialog, tr("Replace All"), tr("Text not found"));
        }
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

bool Qwenpad::askSave(EditorTab *tab)
{
    QString fileName = tab->getFile();
    if (fileName.isEmpty()) {
        fileName = tr("(untitled)");
    }
    int reply = QMessageBox::question(this, tr("Save Changes"),
        tr("'%1' has unsaved changes. Do you want to save them?").arg(fileName),
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

    if (reply == QMessageBox::Save) {
        QString file = tab->getFile();
        if (!file.isEmpty()) {
            tab->saveFile(file);
        } else {
            QString savedFile = QFileDialog::getSaveFileName(this, tr("Save File"),
                QString(), tr("Text Files (*.txt)"));
            if (!savedFile.isEmpty()) {
                tab->saveFile(savedFile);
            } else {
                return false;
            }
        }
        if (tab->isDirty()) {
            return false;
        }
        tabManager->updateCurrentTabTitle();
        return true;
    } else if (reply == QMessageBox::Cancel) {
        return false;
    }

    tab->setDirty(false);
    tabManager->updateCurrentTabTitle();
    return true;
}

void Qwenpad::onCurrentTabChanged()
{
    QTextEdit *editor = currentEditor();
    if (previousEditor) {
        disconnect(previousEditor->document(), &QTextDocument::undoAvailable, this, nullptr);
        disconnect(previousEditor->document(), &QTextDocument::redoAvailable, this, nullptr);
    }

    if (editor) {
        QTextDocument *doc = editor->document();
        connect(doc, &QTextDocument::undoAvailable, this, [this](bool available) {
            undoAction->setEnabled(available);
        });
        connect(doc, &QTextDocument::redoAvailable, this, [this](bool available) {
            redoAction->setEnabled(available);
        });
        connect(doc, &QTextDocument::undoAvailable, this, &Qwenpad::detectLanguage);

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

void Qwenpad::setupGoToLineDialog()
{
    goToLineDialog = new QDialog(this);
    goToLineDialog->setWindowTitle(tr("Go to Line"));
    goToLineDialog->setMinimumWidth(350);

    QVBoxLayout *vlayout = new QVBoxLayout(goToLineDialog);
    vlayout->addWidget(new QLabel(tr("Enter line number:")));

    goToLineEdit = new QLineEdit();
    goToLineEdit->setPlaceholderText(tr("Line number"));
    vlayout->addWidget(goToLineEdit);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    vlayout->addWidget(buttonBox);

    connect(buttonBox, &QDialogButtonBox::accepted, this, [this]() {
        QTextEdit *editor = currentEditor();
        if (editor) {
            bool ok;
            int line = goToLineEdit->text().toInt(&ok);
            if (ok) {
                QTextCursor cursor(editor->document());
                cursor.movePosition(QTextCursor::Start);
                cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, line - 1);
                editor->setTextCursor(cursor);
                editor->ensureCursorVisible();
            }
        }
        goToLineDialog->close();
    });
    connect(buttonBox, &QDialogButtonBox::rejected, this, [this]() {
        goToLineDialog->close();
    });
}

void Qwenpad::onGoToLine()
{
    QTextEdit *editor = currentEditor();
    if (!editor) {
        return;
    }

    int totalLines = editor->document()->blockCount();
    goToLineEdit->setPlaceholderText(tr("Line number (1-%1)").arg(totalLines));
    goToLineEdit->setText(QString::number(editor->textCursor().blockNumber() + 1));
    goToLineEdit->selectAll();
    goToLineEdit->setFocus();
    goToLineDialog->show();
}

void Qwenpad::onZoomIn()
{
    EditorTab *tab = tabManager->currentTab();
    if (tab && tab->getEditor()) {
        tab->getEditor()->zoomIn();
    }
}

void Qwenpad::onZoomOut()
{
    EditorTab *tab = tabManager->currentTab();
    if (tab && tab->getEditor()) {
        tab->getEditor()->zoomOut();
    }
}

void Qwenpad::onStatusLineEndingClicked()
{
    QAction *action = static_cast<QAction *>(sender());
    QString id = action->data().toString();
    if (id == "LF") {
        tabManager->convertLineEndings(0);
        currentLineEndingType = 0;
    } else if (id == "CRLF") {
        tabManager->convertLineEndings(1);
        currentLineEndingType = 1;
    } else if (id == "CR") {
        tabManager->convertLineEndings(2);
        currentLineEndingType = 2;
    }
    setDirty(true);
    detectLineEndings();
    updateStatusBar();
}

  void Qwenpad::onStatusLanguageClicked()
{
    QAction *action = static_cast<QAction *>(sender());
    QString language = action->data().toString();
    EditorTab *tab = tabManager->currentTab();
    if (!tab) {
        return;
    }

    tab->setHighlighterLanguage(language);
    currentHighlighterLanguage = language;
    updateStatusBar();
}

  void Qwenpad::detectLanguage()
{
    EditorTab *tab = tabManager->currentTab();
    if (!tab) return;

    if (tab->getEditor()->document()->blockCount() > 0 && tab->getHighlighterLanguage() == "Text") {
        QTextBlock block = tab->getEditor()->document()->firstBlock();
        QString text = block.text().trimmed();
        if (text.startsWith("#!")) {
            if (text.contains("python")) {
                tab->setHighlighterLanguage("Python");
                currentHighlighterLanguage = "Python";
                statusLanguageLabel->setText(tr("Python"));
            } else if (text.contains("bash") || text.contains("sh") || text.contains("zsh")) {
                tab->setHighlighterLanguage("Shell");
                currentHighlighterLanguage = "Shell";
                statusLanguageLabel->setText(tr("Shell"));
            }
        }
    }
}

  void Qwenpad::updateStatusBar()
{

    EditorTab *tab = tabManager->currentTab();
    if (!tab) {
        return;
    }

    int type = tab->getLineEndingType();
    if (type == 0) {
        statusLineEndingLabel->setText(tr("LF"));
    } else if (type == 1) {
        statusLineEndingLabel->setText(tr("CRLF"));
    } else if (type == 2) {
        statusLineEndingLabel->setText(tr("CR"));
    }

    currentHighlighterLanguage = tab->getHighlighterLanguage();

    QString fileName = tab->getFile();
    if (!fileName.isEmpty()) {
        QString extension = QFileInfo(fileName).suffix().toLower();
        if (extension == "cpp" || extension == "hpp" || extension == "h" || extension == "c") {
            currentHighlighterLanguage = "C++";
        } else if (extension == "py" || extension == "pyw") {
            currentHighlighterLanguage = "Python";
        } else if (extension == "json") {
            currentHighlighterLanguage = "JSON";
        } else if (extension == "sh" || extension == "bash" || extension == "zsh") {
            currentHighlighterLanguage = "Shell";
        } else if (extension == "xml" || extension == "html" || extension == "htm" || extension == "xhtml") {
            currentHighlighterLanguage = "XML";
        }
    }

    if (tab->getEditor()->document()->blockCount() > 0 && currentHighlighterLanguage == "Text") {
        QTextBlock block = tab->getEditor()->document()->firstBlock();
        QString text = block.text().trimmed();
        if (text.startsWith("#!")) {
            if (text.contains("python")) {
                currentHighlighterLanguage = "Python";
            } else if (text.contains("bash") || text.contains("sh") || text.contains("zsh")) {
                currentHighlighterLanguage = "Shell";
            }
        }
    }

    tab->setHighlighterLanguage(currentHighlighterLanguage);

    if (currentHighlighterLanguage == "Text") {
        statusLanguageLabel->setText(tr("Text"));
    } else if (currentHighlighterLanguage == "C++") {
        statusLanguageLabel->setText(tr("C/C++"));
    } else if (currentHighlighterLanguage == "Python") {
        statusLanguageLabel->setText(tr("Python"));
    } else if (currentHighlighterLanguage == "JSON") {
        statusLanguageLabel->setText(tr("JSON"));
    } else if (currentHighlighterLanguage == "Shell") {
        statusLanguageLabel->setText(tr("Shell"));
    } else if (currentHighlighterLanguage == "XML") {
        statusLanguageLabel->setText(tr("XML/HTML"));
    } else {
        statusLanguageLabel->setText(tr("Text"));
    }

    detectLanguage();
}

void Qwenpad::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls()) {
        bool isFile = true;
        for (const QUrl &url : event->mimeData()->urls()) {
            QString localPath = url.toLocalFile();
            if (localPath.isEmpty() || !QFile::exists(localPath)) {
                isFile = false;
                break;
            }
        }
        if (isFile) {
            event->acceptProposedAction();
        }
    } else {
        event->ignore();
    }
}

void Qwenpad::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasUrls()) {
        QStringList files;
        for (const QUrl &url : event->mimeData()->urls()) {
            files.append(url.toLocalFile());
        }
        openFiles(files);
        event->acceptProposedAction();
    }
}

void Qwenpad::keyPressEvent(QKeyEvent *event)
{
    QMainWindow::keyPressEvent(event);
}

  void Qwenpad::saveSession()
{
    QSettings settings("Qwenpad", "Qwenpad");
    QStringList files;
    int untitledCount = 0;
    for (int i = 0; i < tabManager->count(); i++) {
        EditorTab *tab = tabManager->getEditorTab(i);
        if (tab && !tab->getFile().isEmpty()) {
            files.append(tab->getFile());
        } else if (tab) {
            untitledCount++;
        }
    }
    settings.setValue("SessionFiles", files);
    settings.setValue("SessionUntitledCount", untitledCount);
}

 void Qwenpad::restoreSession()
{
    QSettings settings("Qwenpad", "Qwenpad");
    if (settings.contains("SessionFiles")) {
        QStringList files = settings.value("SessionFiles").toStringList();
        if (!files.isEmpty()) {
            openFiles(files);
        }
    }
    if (settings.contains("SessionUntitledCount")) {
        int untitledCount = settings.value("SessionUntitledCount").toInt();
        for (int i = 0; i < untitledCount; i++) {
            tabManager->addTab(tr("(untitled)"));
        }
    }
}

QStringList Qwenpad::getSessionFiles()
{
    QSettings settings("Qwenpad", "Qwenpad");
    if (settings.contains("SessionFiles")) {
        return settings.value("SessionFiles").toStringList();
    }
    return QStringList();
}
