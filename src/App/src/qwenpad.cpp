#include "qwenpad.h"
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

Qwenpad::Qwenpad(QWidget *parent)
    : QMainWindow(parent)
    , editor(nullptr)
    , lineNumberWidget(nullptr)
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
    , aboutAction(nullptr)
    , quitAction(nullptr)
    , wrapAction(nullptr)
    , lineNumbersAction(nullptr)
    , fontAction(nullptr)
    , undoAction(nullptr)
    , redoAction(nullptr)
    , findAction(nullptr)
    , toolbarNewAction(nullptr)
    , toolbarOpenAction(nullptr)
    , toolbarSaveAction(nullptr)
    , bufferDirty(false)
    , wordWrapEnabled(true)
    , lineNumbersEnabled(false)
    , currentLineEndingType(0)
{
    setupUI();
}

void Qwenpad::setupUI()
{
    setupActions();
    setupMenuBar();
    setupToolBar();
    setupStatusBar();
    setupEditor();
    setupFindDialog();
}

void Qwenpad::setupEditor()
{
    editor = new QTextEdit(this);

    QFont font;
    font.setFamily("Monospace");
    font.setFixedPitch(true);
    font.setPointSize(10);
    editor->setFont(font);

    lineNumberWidget = new LineEditWidget(editor, this);
    lineNumberWidget->setFont(editor->font());
    lineNumberWidget->setEnabled(false);

    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    QHBoxLayout *editorLayout = new QHBoxLayout();
    editorLayout->setContentsMargins(0, 0, 0, 0);
    editorLayout->setSpacing(0);
    editorLayout->addWidget(lineNumberWidget);
    editorLayout->addWidget(editor);

    mainLayout->addLayout(editorLayout);
    setCentralWidget(centralWidget);

    editor->installEventFilter(this);
    connect(editor, &QTextEdit::cursorPositionChanged, this, &Qwenpad::updateLineInfo);
    connect(editor->document(), &QTextDocument::contentsChanged, this, &Qwenpad::drawLineNumbers);

    if (lineNumbersEnabled) {
        drawLineNumbers();
    }
}

void Qwenpad::setupActions()
{
   cutAction = new QAction(QIcon::fromTheme("edit-cut", QIcon::fromTheme("edit_cut")), tr("Cut"), this);
    cutAction->setShortcuts(QKeySequence::Cut);
    cutAction->setToolTip(tr("Cut"));
    connect(cutAction, &QAction::triggered, editor, &QTextEdit::cut);

    copyAction = new QAction(QIcon::fromTheme("edit-copy", QIcon::fromTheme("edit_copy")), tr("Copy"), this);
    copyAction->setShortcuts(QKeySequence::Copy);
    copyAction->setToolTip(tr("Copy"));
    connect(copyAction, &QAction::triggered, editor, &QTextEdit::copy);

    pasteAction = new QAction(QIcon::fromTheme("edit-paste", QIcon::fromTheme("edit_paste")), tr("Paste"), this);
    pasteAction->setShortcut(QKeySequence::Paste);
    pasteAction->setToolTip(tr("Paste"));
    connect(pasteAction, &QAction::triggered, editor, &QTextEdit::paste);

    selectAllAction = new QAction(QIcon::fromTheme("edit-select-all", QIcon::fromTheme("select_all")), tr("Select All"), this);
    selectAllAction->setShortcuts(QKeySequence::SelectAll);
    connect(selectAllAction, &QAction::triggered, editor, &QTextEdit::selectAll);

    newAction = new QAction(QIcon::fromTheme("document-new"), tr("&New"), this);
    newAction->setShortcut(QKeySequence::New);
    connect(newAction, &QAction::triggered, this, &Qwenpad::onNew);

    openAction = new QAction(QIcon::fromTheme("document-open"), tr("&Open"), this);
    openAction->setShortcut(QKeySequence::Open);
    connect(openAction, &QAction::triggered, this, &Qwenpad::onOpen);

    saveAction = new QAction(QIcon::fromTheme("document-save"), tr("&Save"), this);
    saveAction->setShortcut(QKeySequence::Save);
    connect(saveAction, &QAction::triggered, this, &Qwenpad::onSave);

    toolbarNewAction = new QAction(QIcon::fromTheme("document-new"), tr("New"), this);
    connect(toolbarNewAction, &QAction::triggered, this, &Qwenpad::onNew);

    toolbarOpenAction = new QAction(QIcon::fromTheme("document-open"), tr("Open"), this);
    connect(toolbarOpenAction, &QAction::triggered, this, &Qwenpad::onOpen);

    toolbarSaveAction = new QAction(QIcon::fromTheme("document-save"), tr("Save"), this);
    connect(toolbarSaveAction, &QAction::triggered, this, &Qwenpad::onSave);

    aboutAction = new QAction(QIcon::fromTheme("help-about", QIcon::fromTheme("system-help")), tr("About"), this);
    aboutAction->setShortcut(QKeySequence("F1"));
    connect(aboutAction, &QAction::triggered, this, &Qwenpad::onAbout);

    quitAction = new QAction(tr("Quit"), this);
    quitAction->setShortcut(QKeySequence("Ctrl+Q"));
    connect(quitAction, &QAction::triggered, this, &QMainWindow::close);

    wrapAction = new QAction(tr("Word Wrap"), this);
    wrapAction->setShortcut(QKeySequence("Ctrl+W"));
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

    undoAction = new QAction(QIcon::fromTheme("edit-undo", QIcon::fromTheme("edit_undo")), tr("&Undo"), this);
    undoAction->setShortcut(QKeySequence::Undo);
    undoAction->setEnabled(false);

    redoAction = new QAction(QIcon::fromTheme("edit-redo", QIcon::fromTheme("edit_redo")), tr("&Redo"), this);
    redoAction->setShortcut(QKeySequence::Redo);
    redoAction->setEnabled(false);

    findAction = new QAction(QIcon::fromTheme("edit-find", QIcon::fromTheme("edit_find")), tr("Find/Replace..."), this);
    findAction->setShortcut(QKeySequence("Ctrl+F"));
    connect(findAction, &QAction::triggered, this, &Qwenpad::onFind);
}

void Qwenpad::setupMenuBar()
{
    menubar = new QMenuBar(this);

    auto *fileMenu = new QMenu(tr("File"), menubar);
    fileMenu->addAction(newAction);
    fileMenu->addAction(openAction);
    fileMenu->addAction(saveAction);

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
    viewMenu->addAction(fontAction);

    auto *helpMenu = new QMenu(tr("Help"), menubar);
    helpMenu->addAction(aboutAction);
    helpMenu->addSeparator();
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

    connect(editor, &QTextEdit::textChanged, this, &Qwenpad::onTextChange);
    connect(editor, &QTextEdit::undoAvailable, undoAction, &QAction::setEnabled);
    connect(editor, &QTextEdit::undoAvailable, redoAction, &QAction::setEnabled);
    connect(editor, &QTextEdit::redoAvailable, redoAction, &QAction::setEnabled);

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
    if (bufferDirty) {
        int reply = QMessageBox::question(this, tr("New File"),
            tr("The current file has unsaved changes. Do you really want to create a new file?"),
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

        if (reply == QMessageBox::Save) {
            onSave();
        } else if (reply == QMessageBox::Discard) {
            bufferDirty = false;
            setDirty(false);
        } else if (reply == QMessageBox::Cancel) {
            return;
        }
    }

    editor->clear();
    currentFile.clear();
    setDirty(false);
}

void Qwenpad::onOpen()
{
    if (bufferDirty) {
        QString savedFile = askSave();
        if (savedFile.isEmpty()) {
            return;
        }
    }

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
        QString(), tr("Text Files (*.txt)"));

    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream stream(&file);
            QString content = stream.readAll();
            file.close();
            editor->setPlainText(content);
            currentFile = fileName;

            detectLineEndings();

            QSettings settings(fileName + ".meta", QSettings::IniFormat);
            if (settings.contains("FontFamily")) {
                QFont restoredFont;
                restoredFont.setFamily(settings.value("FontFamily").toString());
                restoredFont.setPointSize(settings.value("FontSize").toInt());
                editor->setFont(restoredFont);
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
}

void Qwenpad::onSave()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
        currentFile.isEmpty() ? QString() : currentFile, tr("Text Files (*.txt)"));

    if (!fileName.isEmpty()) {
        QString content = editor->toPlainText();

        content = convertLineEndings(content, currentLineEndingType);

        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream stream(&file);
            stream << content;
            file.close();

            QSettings settings(fileName + ".meta", QSettings::IniFormat);
            settings.setValue("FontFamily", editor->font().family());
            settings.setValue("FontSize", editor->font().pointSize());
            settings.setValue("WordWrapEnabled", wrapAction->isChecked());
            settings.setValue("LineNumbersEnabled", lineNumbersAction->isChecked());
            settings.setValue("LineEndingType", currentLineEndingType);

            currentFile = fileName;
            setDirty(false);
            QMessageBox::information(this, tr("Save"), tr("File saved successfully"));
        }
    }
}

void Qwenpad::onCut()
{
    editor->cut();
}

void Qwenpad::onCopy()
{
    editor->copy();
}

void Qwenpad::onPaste()
{
    editor->paste();
}

void Qwenpad::onSelectAll()
{
    editor->selectAll();
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
    editor->setWordWrapMode(mode);
}

void Qwenpad::onToggleLineNumbers()
{
    lineNumbersEnabled = lineNumbersAction->isChecked();
    lineNumberWidget->setEnabled(lineNumbersEnabled);

    if (lineNumbersEnabled) {
        drawLineNumbers();
    }
}

void Qwenpad::onFont()
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok, this);
    if (ok) {
        editor->setFont(font);
        lineNumberWidget->setFont(editor->font());
    }
}

void Qwenpad::onConvertCrlf()
{
    QString content = editor->toPlainText();
    QString converted = convertLineEndings(content, 1);
    editor->setPlainText(converted);
    currentLineEndingType = 1;
    setDirty(true);
    detectLineEndings();
}

void Qwenpad::onConvertLf()
{
    QString content = editor->toPlainText();
    QString converted = convertLineEndings(content, 0);
    editor->setPlainText(converted);
    currentLineEndingType = 0;
    setDirty(true);
    detectLineEndings();
}

void Qwenpad::onConvertCr()
{
    QString content = editor->toPlainText();
    QString converted = convertLineEndings(content, 2);
    editor->setPlainText(converted);
    currentLineEndingType = 2;
    setDirty(true);
    detectLineEndings();
}

void Qwenpad::detectLineEndings()
{
    QString content = editor->toPlainText();
    int crCount = content.count('\r');
    int lfCount = content.count('\n');
    int crlfCount = content.count("\r\n");

    if (crlfCount > crCount - crlfCount) {
        currentLineEndingType = 1;
    } else if (crCount > lfCount) {
        currentLineEndingType = 2;
    } else {
        currentLineEndingType = 0;
    }
}

QString Qwenpad::convertLineEndings(const QString &text, int target)
{
    QString result = text;

    switch (target) {
    case 0:
        result.replace("\r\n", "\n").replace("\r", "\n");
        break;
    case 1:
        result.replace("\r", "\r\n").replace("\n", "\r\n");
        break;
    case 2:
        result.replace("\r\n", "\r").replace("\n", "\r");
        break;
    }

    return result;
}

void Qwenpad::onTextChange()
{
    setDirty(true);
    updateLineInfo();
}

void Qwenpad::setDirty(bool dirty)
{
    bufferDirty = dirty;
    QString title = currentFile.isEmpty() ? tr("Qwenpad") : QFileInfo(currentFile).fileName();
    if (bufferDirty) {
        title += tr(" *");
    }
    title += tr(" - Qwenpad");
    setWindowTitle(title);
}

QString Qwenpad::askSave()
{
    if (!bufferDirty) {
        return QString();
    }

    int reply = QMessageBox::question(this, tr("Save Changes"),
        tr("The current file has unsaved changes. Do you want to save them?"),
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

    if (reply == QMessageBox::Save) {
        onSave();
        return currentFile;
    } else if (reply == QMessageBox::Cancel) {
        return QString();
    }

    bufferDirty = false;
    setWindowTitle(currentFile.isEmpty() ? tr("Qwenpad") : QFileInfo(currentFile).fileName() + tr(" - Qwenpad"));
    return QString();
}

void Qwenpad::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
}

void Qwenpad::closeEvent(QCloseEvent *event)
{
    if (bufferDirty) {
        int reply = QMessageBox::question(this, tr("Save Changes"),
            tr("The current file has unsaved changes. Do you want to save them?"),
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

        if (reply == QMessageBox::Save) {
            onSave();
        } else if (reply == QMessageBox::Cancel) {
            event->ignore();
            return;
        }
        bufferDirty = false;
        setDirty(false);
    }
    QMainWindow::closeEvent(event);
}

void Qwenpad::updateLineInfo()
{
    QTextCursor cursor = editor->textCursor();
    int line = cursor.blockNumber() + 1;
    int col = cursor.positionInBlock() + 1;
    lineInfoLabel->setText(tr("Ln %1, Col %2").arg(line).arg(col));
}

bool Qwenpad::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == editor && event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        Q_UNUSED(keyEvent);
        updateLineInfo();
        return false;
    }

    return QObject::eventFilter(obj, event);
}

void Qwenpad::drawLineNumbers()
{
    if (!lineNumbersEnabled || !lineNumberWidget) {
        return;
    }

    lineNumberWidget->update();
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

    QTextCursor cursor = editor->textCursor();
    QTextCursor foundCursor = cursor.document()->find(searchText);

    if (foundCursor.isNull()) {
        cursor = editor->textCursor();
        cursor.setPosition(0);
        editor->setTextCursor(cursor);
        foundCursor = cursor.document()->find(searchText);

        if (foundCursor.isNull()) {
            QMessageBox::information(findDialog, tr("Find"), tr("Text not found"));
        } else {
            editor->setTextCursor(foundCursor);
            editor->ensureCursorVisible();
        }
    } else {
        editor->setTextCursor(foundCursor);
        editor->ensureCursorVisible();
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

    QTextCursor cursor = editor->textCursor();
    QTextCursor foundCursor = cursor.document()->find(searchText);

    if (foundCursor.isNull()) {
        cursor.setPosition(0);
        editor->setTextCursor(cursor);
        foundCursor = cursor.document()->find(searchText);

        if (foundCursor.isNull()) {
            QMessageBox::information(findDialog, tr("Replace"), tr("Text not found"));
        } else {
            if (!replaceText.isEmpty()) {
                cursor.insertText(replaceText);
            }
            editor->setTextCursor(cursor);
            editor->ensureCursorVisible();
        }
    } else {
        if (!replaceText.isEmpty()) {
            cursor.insertText(replaceText);
        }
        editor->setTextCursor(cursor);
        editor->ensureCursorVisible();
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

    QString originalText = editor->toPlainText();
    int replacementCount = originalText.count(searchText);

    if (replacementCount > 0) {
        editor->undo();
        QString modifiedText = originalText;
        editor->setPlainText(modifiedText.replace(searchText, replaceText));
        setDirty(true);
    } else {
        QMessageBox::information(findDialog, tr("Replace All"), tr("Text not found"));
    }
}

void Qwenpad::onCloseFindDialog()
{
    findDialog->close();
}
