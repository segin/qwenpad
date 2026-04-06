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

    auto *editMenu = new QMenu(tr("Edit"), menubar);
    editMenu->addAction(cutAction);
    editMenu->addAction(copyAction);
    editMenu->addAction(pasteAction);
    editMenu->addAction(selectAllAction);
    editMenu->addSeparator();
    editMenu->addAction(new QAction(tr("&Undo"), this));
    editMenu->addAction(new QAction(tr("&Redo"), this));

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
    toolbar->addAction(cutAction);
    toolbar->addAction(copyAction);
    toolbar->addAction(pasteAction);
    toolbar->addSeparator();
    toolbar->addAction(new QAction(tr("New"), this));
    toolbar->addAction(new QAction(tr("Open"), this));
    toolbar->addAction(new QAction(tr("Save"), this));
    addToolBar(toolbar);

    // Status bar with line info and CR/LF info
    statusBar = new QStatusBar(this);
    setStatusBar(statusBar);
    lineInfoLabel = new QLabel(tr("Ln 0, Col 0"));
    crLfInfoLabel = new QLabel(tr("CR/LF (DOS)"));
    lfInfoLabel = new QLabel(tr("LF (Unix)"));
    statusBar->addPermanentWidget(lineInfoLabel);
    statusBar->addPermanentWidget(crLfInfoLabel);
    statusBar->addPermanentWidget(lfInfoLabel);
    // Connect editor text change to our slot
    connect(editor, &QTextEdit::textChanged, this, &NotepadApp::onTextChange);

    statusBar->showMessage(tr("Notepad v0.1"));
}

void NotepadApp::onNew()
{
    if (bufferDirty) {
        int reply = QMessageBox::question(this, tr("New File"),
            tr("The current file has unsaved changes. Do you really want to create a new file?"),
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

        if (reply == QMessageBox::Save) {
            onSave();
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
            editor->setPlainText(stream.readAll());
            file.close();
            currentFile = fileName;
            
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
            
            setDirty(false);
        }
    }
}

void NotepadApp::onSave()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
        currentFile.isEmpty() ? QString() : currentFile, tr("Text Files (*.txt)"));

    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream stream(&file);
            // Save document content
            stream << editor->toPlainText();
            // Save metadata (font, line wrap, line numbers)
            QSettings settings(fileName + ".meta", QSettings::IniFormat);
            settings.setValue("FontFamily", editor->font().family());
            settings.setValue("FontSize", editor->font().pointSize());
            settings.setValue("WordWrapEnabled", wrapAction->isChecked());
            settings.setValue("LineNumbersEnabled", lineNumbersAction->isChecked());
            file.close();
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
                crLfInfoLabel->setText(tr("CR/LF (DOS)"));
            } else {
                lfInfoLabel->setText(tr("LF (Unix)"));
            }
        }
        
        // Update cursor position
        updateLineInfo();
        
        return false;
    }
    
    return QObject::eventFilter(obj, event);
}