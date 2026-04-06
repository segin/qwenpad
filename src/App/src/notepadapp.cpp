#include "notepadapp.h"
#include <QVBoxLayout>
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

NotepadApp::NotepadApp(QWidget *parent)
    : QMainWindow(parent)
    , bufferDirty(false)
    , wordWrapEnabled(true)
    , lineNumbersEnabled(false)
    , currentLineEndingType(0) // 0 = LF
{
    setupUI();
    editor->installEventFilter(this);
}

void NotepadApp::setupUI()
{
    menubar = new QMenuBar(this);
    toolbar = new QToolBar(this);
    editor = new QTextEdit(this);
    QFont font;
    font.setFamily("Monospace");
    font.setFixedPitch(true);
    font.setPointSize(10);
    editor->setFont(font);
    setCentralWidget(editor);

    // Setup line numbers frame
    lineNumberFont = editor->font();
    lineNumberFont.setPointSize(lineNumberFont.pointSize() - 1);
    lineNumberFont.setBold(false);
    editor->installEventFilter(this);
    connect(editor, &QTextEdit::cursorPositionChanged, this, &NotepadApp::updateLineInfo);

    cutAction = new QAction(tr("Cut"), this);
    cutAction->setShortcuts(QKeySequence::Cut);
    connect(cutAction, &QAction::triggered, editor, &QTextEdit::cut);

    copyAction = new QAction(tr("Copy"), this);
    copyAction->setShortcuts(QKeySequence::Copy);
    connect(copyAction, &QAction::triggered, editor, &QTextEdit::copy);

    pasteAction = new QAction(tr("Paste"), this);
    pasteAction->setShortcuts(QKeySequence::Paste);
    connect(pasteAction, &QAction::triggered, editor, &QTextEdit::paste);

    selectAllAction = new QAction(tr("Select All"), this);
    selectAllAction->setShortcuts(QKeySequence::SelectAll);
    connect(selectAllAction, &QAction::triggered, editor, &QTextEdit::selectAll);

    aboutAction = new QAction(tr("About"), this);
    aboutAction->setShortcut(QKeySequence("F1"));
    connect(aboutAction, &QAction::triggered, this, &NotepadApp::onAbout);

    quitAction = new QAction(tr("Quit"), this);
    quitAction->setShortcut(QKeySequence("Ctrl+Q"));
    connect(quitAction, &QAction::triggered, this, &QMainWindow::close);

    wrapAction = new QAction(tr("Word Wrap"), this);
    wrapAction->setShortcut(QKeySequence("Ctrl+W"));
    wrapAction->setCheckable(true);
    wrapAction->setChecked(true);
    connect(wrapAction, &QAction::triggered, this, &NotepadApp::onWrap);

    lineNumbersAction = new QAction(tr("Line Numbers"), this);
    lineNumbersAction->setShortcut(QKeySequence("Ctrl+L"));
    lineNumbersAction->setCheckable(true);
    lineNumbersAction->setChecked(false);
    connect(lineNumbersAction, &QAction::triggered, this, &NotepadApp::onToggleLineNumbers);

    fontAction = new QAction(tr("Font..."), this);
    fontAction->setShortcut(QKeySequence("Ctrl+F"));
    connect(fontAction, &QAction::triggered, this, &NotepadApp::onFont);

    // File menu with connected actions
    auto *fileMenu = new QMenu(tr("File"), menubar);
    QAction *newAction = new QAction(tr("&New"), this);
    newAction->setShortcut(QKeySequence::New);
    connect(newAction, &QAction::triggered, this, &NotepadApp::onNew);
    fileMenu->addAction(newAction);

    QAction *openAction = new QAction(tr("&Open"), this);
    openAction->setShortcut(QKeySequence::Open);
    connect(openAction, &QAction::triggered, this, &NotepadApp::onOpen);
    fileMenu->addAction(openAction);

    QAction *saveAction = new QAction(tr("&Save"), this);
    saveAction->setShortcut(QKeySequence::Save);
    connect(saveAction, &QAction::triggered, this, &NotepadApp::onSave);
    fileMenu->addAction(saveAction);

 fileMenu->addSeparator();
    fileMenu->addAction(quitAction);

    // Undo/Redo actions with shortcuts
    undoAction = new QAction(tr("&Undo"), this);
    undoAction->setShortcut(QKeySequence::Undo);
    undoAction->setEnabled(false);
    
    redoAction = new QAction(tr("&Redo"), this);
    redoAction->setShortcut(QKeySequence::Redo);
    redoAction->setEnabled(false);

    auto *editMenu = new QMenu(tr("Edit"), menubar);
    editMenu->addAction(cutAction);
    editMenu->addAction(copyAction);
    editMenu->addAction(pasteAction);
    editMenu->addAction(selectAllAction);
    editMenu->addSeparator();
    editMenu->addAction(undoAction);
    editMenu->addAction(redoAction);

    auto *viewMenu = new QMenu(tr("View"), menubar);
    viewMenu->addAction(wrapAction);
    viewMenu->addAction(lineNumbersAction);
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

   toolbar->setIconSize(QSize(16, 16));
    // File actions first with icons
    toolbar->addAction(new QAction(QIcon::fromTheme("document-new"), tr("New"), this));
    toolbar->addAction(new QAction(QIcon::fromTheme("document-open"), tr("Open"), this));
    toolbar->addAction(new QAction(QIcon::fromTheme("document-save"), tr("Save"), this));
    toolbar->addSeparator();
    // Edit actions
    toolbar->addAction(cutAction);
    toolbar->addAction(copyAction);
    toolbar->addAction(pasteAction);
    addToolBar(toolbar);

   // Status bar with line info
    setStatusBar(new QStatusBar(this));
    lineInfoLabel = new QLabel(tr("Ln 0, Col 0"));
    statusBar()->addPermanentWidget(lineInfoLabel);
    // Connect editor text change to our slot
    connect(editor, &QTextEdit::textChanged, this, &NotepadApp::onTextChange);
    connect(editor, &QTextEdit::undoAvailable, undoAction, &QAction::setEnabled);
    connect(editor, &QTextEdit::undoAvailable, redoAction, &QAction::setEnabled);
    connect(editor, &QTextEdit::redoAvailable, redoAction, &QAction::setEnabled);

    statusBar()->showMessage(tr("Notepad v0.1"));
}

  void NotepadApp::onNew()
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

void NotepadApp::onOpen()
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
        // Load content
        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream stream(&file);
            QString content = stream.readAll();
            file.close();
            editor->setPlainText(content);
            currentFile = fileName;
            
            // Detect line endings in opened file
            detectLineEndings();
            
            // Restore metadata if available
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

void NotepadApp::onSave()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
        currentFile.isEmpty() ? QString() : currentFile, tr("Text Files (*.txt)"));

    if (!fileName.isEmpty()) {
        QString content = editor->toPlainText();
        
        // Convert line endings based on current selection
        content = convertLineEndings(content, currentLineEndingType);
        
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream stream(&file);
            stream << content;
            file.close();
            
            // Save metadata (font, line wrap, line numbers, line endings)
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

void NotepadApp::onCut()
{
    editor->cut();
}

void NotepadApp::onCopy()
{
    editor->copy();
}

void NotepadApp::onPaste()
{
    editor->paste();
}

void NotepadApp::onSelectAll()
{
    editor->selectAll();
}

void NotepadApp::onAbout()
{
    QMessageBox::information(this, tr("About"), tr("Notepad v0.1"));
}

void NotepadApp::onWrap()
{
    QTextOption::WrapMode mode = wrapAction->isChecked()
        ? QTextOption::WrapMode::WordWrap
        : QTextOption::WrapMode::NoWrap;
    editor->setWordWrapMode(mode);
}

void NotepadApp::onToggleLineNumbers()
{
    lineNumbersEnabled = lineNumbersAction->isChecked();
    // Line numbers are handled via event filter
}

void NotepadApp::onFont()
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok, this);
    if (ok) {
        editor->setFont(font);
    }
}

void NotepadApp::onConvertCrlf()
{
    QString content = editor->toPlainText();
    QString converted = convertLineEndings(content, 1); // 1 = CRLF
    editor->setPlainText(converted);
    currentLineEndingType = 1;
    setDirty(true);
    detectLineEndings();
}

void NotepadApp::onConvertLf()
{
    QString content = editor->toPlainText();
    QString converted = convertLineEndings(content, 0); // 0 = LF
    editor->setPlainText(converted);
    currentLineEndingType = 0;
    setDirty(true);
    detectLineEndings();
}

void NotepadApp::onConvertCr()
{
    QString content = editor->toPlainText();
    QString converted = convertLineEndings(content, 2); // 2 = CR
    editor->setPlainText(converted);
    currentLineEndingType = 2;
    setDirty(true);
    detectLineEndings();
}

void NotepadApp::detectLineEndings()
{
    QString content = editor->toPlainText();
    int crCount = content.count('\r');
    int lfCount = content.count('\n');
    int crlfCount = content.count("\r\n");
    
    if (crlfCount > crCount - crlfCount) {
        currentLineEndingType = 1; // CRLF
    } else if (crCount > lfCount) {
        currentLineEndingType = 2; // CR
    } else {
        currentLineEndingType = 0; // LF
    }
}

QString NotepadApp::convertLineEndings(const QString &text, int target)
{
    QString result = text;
    
    switch (target) {
        case 0: // LF
            result = result.replace("\r\n", "\n").replace("\r", "\n");
            break;
        case 1: // CRLF
            result = result.replace("\r\n", "\r\n").replace("\r", "\r\n").replace("\n", "\r\n");
            break;
        case 2: // CR
            result = result.replace("\r\n", "\r").replace("\n", "\r");
            break;
    }
    
    return result;
}

void NotepadApp::onTextChange()
{
    setDirty(true);
    updateLineInfo();
}

void NotepadApp::setDirty(bool dirty)
{
    bufferDirty = dirty;
    QString title = currentFile.isEmpty() ? tr("Notepad") : QFileInfo(currentFile).fileName();
    if (bufferDirty) {
        title += tr(" *");
    }
    title += tr(" - Notepad");
    setWindowTitle(title);
}

QString NotepadApp::askSave()
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
    setWindowTitle(currentFile.isEmpty() ? tr("Notepad") : QFileInfo(currentFile).fileName() + tr(" - Notepad"));
    return QString();
}

void NotepadApp::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
}

void NotepadApp::closeEvent(QCloseEvent *event)
{
    if (bufferDirty) {
        QString savedFile = askSave();
        if (savedFile.isEmpty()) {
            event->ignore();
            return;
        }
    }
    setDirty(false);
    QMainWindow::closeEvent(event);
}

void NotepadApp::updateLineInfo()
{
    QTextCursor cursor = editor->textCursor();
    int line = cursor.blockNumber() + 1;
    int col = cursor.positionInBlock() + 1;
    lineInfoLabel->setText(tr("Ln %1, Col %2").arg(line).arg(col));
}

bool NotepadApp::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == editor && event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        
       // Track line endings
        if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) {
            // Track if user presses Enter (creates LF) or Ctrl+Enter (creates CRLF)
            if (keyEvent->modifiers() & Qt::ControlModifier) {
                currentLineEndingType = 1; // CRLF
            } else {
                currentLineEndingType = 0; // LF
            }
            detectLineEndings();
        }
        
        // Update cursor position
        updateLineInfo();
        
        return false;
    }
    
    return QObject::eventFilter(obj, event);
}