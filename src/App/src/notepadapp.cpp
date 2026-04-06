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

NotepadApp::NotepadApp(QWidget *parent) : QMainWindow(parent)
{
    setupUI();
}

void NotepadApp::setupUI()
{
    menubar = new QMenuBar(this);
    toolbar = new QToolBar(this);
    editor = new QTextEdit(this);
    QFont font;
    font.setFamily("Monospace");
    font.setFixedPitch(true);
    editor->setFont(font);
    bufferDirty = false;

   

    cutAction = new QAction("Cut", this);
    copyAction = new QAction("Copy", this);
    pasteAction = new QAction("Paste", this);
    selectAllAction = new QAction("Select All", this);
    aboutAction = new QAction("About", this);
    aboutAction->setShortcut(QKeySequence("F1"));
    quitAction = new QAction("Quit", this);
    quitAction->setShortcut(QKeySequence("Ctrl+Q"));
    wrapAction = new QAction("Word Wrap", this);
    wrapAction->setShortcut(QKeySequence("Ctrl+W"));
    wrapAction->setCheckable(true);
    wrapAction->setChecked(true);
    fontAction = new QAction("Font...", this);
    fontAction->setShortcut(QKeySequence("Ctrl+F"));

    cutAction->setShortcut(QKeySequence::Cut);
    copyAction->setShortcut(QKeySequence::Copy);
    pasteAction->setShortcut(QKeySequence::Paste);
    selectAllAction->setShortcut(QKeySequence::SelectAll);

    connect(cutAction, SIGNAL(triggered()), this, SLOT(onCut()));
    connect(copyAction, SIGNAL(triggered()), this, SLOT(onCopy()));
    connect(pasteAction, SIGNAL(triggered()), this, SLOT(onPaste()));
    connect(selectAllAction, SIGNAL(triggered()), this, SLOT(onselectAll()));

    connect(cutAction, SIGNAL(triggered()), editor, SLOT(cut()));
    connect(copyAction, SIGNAL(triggered()), editor, SLOT(copy()));
    connect(pasteAction, SIGNAL(triggered()), editor, SLOT(paste()));
    connect(selectAllAction, SIGNAL(triggered()), editor, SLOT(selectAll()));
       connect(editor, SIGNAL(textChanged()), this, SLOT(setBufferDirty()));
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(onAbout()));
    connect(quitAction, SIGNAL(triggered()), this, SLOT(close()));
    connect(wrapAction, SIGNAL(triggered()), this, SLOT(onWrap()));
    connect(fontAction, SIGNAL(triggered()), this, SLOT(onFont()));

    cutAction->setIcon(QIcon::fromTheme("edit-cut"));
    copyAction->setIcon(QIcon::fromTheme("edit-copy"));
    pasteAction->setIcon(QIcon::fromTheme("edit-paste"));
    
    QAction *newAction = new QAction("New", this);
    newAction->setIcon(QIcon::fromTheme("document-new"));
    newAction->setShortcut(QKeySequence::New);
    connect(newAction, SIGNAL(triggered()), this, SLOT(onNew()));
    
    QAction *openAction = new QAction("Open", this);
    openAction->setIcon(QIcon::fromTheme("document-open"));
    openAction->setShortcut(QKeySequence::Open);
    connect(openAction, SIGNAL(triggered()), this, SLOT(onOpen()));
    
    QAction *saveAction = new QAction("Save", this);
    saveAction->setIcon(QIcon::fromTheme("document-save"));
    saveAction->setShortcut(QKeySequence::Save);
    connect(saveAction, SIGNAL(triggered()), this, SLOT(onSave()));
    
    toolbar->addAction(newAction);
    toolbar->addAction(openAction);
    toolbar->addAction(saveAction);
    toolbar->addSeparator();
    toolbar->addAction(cutAction);
    toolbar->addAction(copyAction);
    toolbar->addAction(pasteAction);

    QMenu *fileMenu = new QMenu("File", menubar);
    QAction *fileNew = new QAction("&New", this);
    fileNew->setShortcut(QKeySequence::New);
    connect(fileNew, SIGNAL(triggered()), this, SLOT(onNew()));
    fileMenu->addAction(fileNew);
    QAction *fileOpen = new QAction("&Open", this);
    fileOpen->setShortcut(QKeySequence::Open);
    connect(fileOpen, SIGNAL(triggered()), this, SLOT(onOpen()));
    fileMenu->addAction(fileOpen);
    QAction *fileSave = new QAction("&Save", this);
    fileSave->setShortcut(QKeySequence::Save);
    connect(fileSave, SIGNAL(triggered()), this, SLOT(onSave()));
    fileMenu->addAction(fileSave);
    fileMenu->addSeparator();
    fileMenu->addAction(quitAction);
    
    QMenu *editMenu = new QMenu("Edit", menubar);
    QMenu *helpMenu = new QMenu("Help", menubar);
    QMenu *viewMenu = new QMenu("View", menubar);

    editMenu->addAction(cutAction);
    editMenu->addAction(copyAction);
    editMenu->addAction(pasteAction);
    editMenu->addAction(selectAllAction);
    editMenu->addSeparator();
    QAction *undoAction = new QAction("&Undo", this);
    undoAction->setShortcut(QKeySequence::Undo);
    undoAction->setEnabled(false);
    editMenu->addAction(undoAction);
    QAction *redoAction = new QAction("&Redo", this);
    redoAction->setShortcut(QKeySequence::Redo);
    redoAction->setEnabled(false);
    editMenu->addAction(redoAction);
    
    helpMenu->addAction(aboutAction);
    helpMenu->addSeparator();
    QAction *docAction = new QAction("&Documentation", this);
    docAction->setShortcut(QKeySequence("F1"));
    helpMenu->addAction(docAction);
    
    viewMenu->addAction(wrapAction);
    viewMenu->addSeparator();
    viewMenu->addAction(fontAction);

    menubar->addMenu(fileMenu);
    menubar->addMenu(editMenu);
    menubar->addMenu(viewMenu);
    menubar->addMenu(helpMenu);

    setMenuBar(menubar);
    addToolBar(toolbar);
    setCentralWidget(editor);

    statusBar()->showMessage("Notepad v0.1");
}

void NotepadApp::onNew()
{
    if (bufferDirty) {
        int reply = QMessageBox::question(this, "New File",
            "The current file has unsaved changes. Do you really want to create a new file?",
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        
        if (reply == QMessageBox::Save) {
            onSave();
        } else if (reply == QMessageBox::Cancel) {
            return;
        }
    }
    
    editor->clear();
    currentFile.clear();
    setWindowTitle("Notepad");
}

void NotepadApp::onOpen()
{
    if (bufferDirty) {
        QString savedFile = askSave();
        if (savedFile.isEmpty()) {
            return;
        }
    }
    
    QString fileName = QFileDialog::getOpenFileName(this, "Open File", "", "Text Files (*.txt)");
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream stream(&file);
            editor->setPlainText(stream.readAll());
            file.close();
            currentFile = fileName;
            setDirty(false);
            setWindowTitle(QFileInfo(fileName).fileName() + " - Notepad");
        }
    }
}

void NotepadApp::onSave()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Save File", currentFile.isEmpty() ? "" : currentFile, "Text Files (*.txt)");
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream stream(&file);
            stream << editor->toPlainText();
            file.close();
            currentFile = fileName;
            setDirty(false);
            setWindowTitle(QFileInfo(fileName).fileName() + " - Notepad");
            QMessageBox::information(this, "Save", "File saved successfully");
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

void NotepadApp::onselectAll()
{
    editor->selectAll();
}

void NotepadApp::onAbout()
{
    QMessageBox::information(this, "About", "Notepad v0.1");
}

void NotepadApp::onWrap()
{
    QTextOption::WrapMode mode = wrapAction->isChecked() ? QTextOption::WrapMode::WordWrap : QTextOption::WrapMode::NoWrap;
    editor->setWordWrapMode(mode);
}

void NotepadApp::onFont()
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok, this);
    if (ok) {
        editor->setFont(font);
    }
}

void NotepadApp::updateUI()
{
    statusBar()->showMessage("Notepad v1.0");
}

void NotepadApp::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
}

bool NotepadApp::isBufferDirty() const
{
    return bufferDirty;
}

void NotepadApp::setBufferDirty()
{
    setDirty(true);
}

void NotepadApp::setDirty(bool dirty)
{
    bufferDirty = dirty;
    QString title = currentFile.isEmpty() ? "Notepad" : QFileInfo(currentFile).fileName();
    if (bufferDirty) {
        title += " *";
    }
    title += " - Notepad";
    setWindowTitle(title);
}

QString NotepadApp::askSave()
{
    if (!bufferDirty) {
        return QString();
    }
    
    int reply = QMessageBox::question(this, "Save Changes",
        "The current file has unsaved changes. Do you want to save them?",
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    
    if (reply == QMessageBox::Save) {
        onSave();
        return currentFile;
    } else if (reply == QMessageBox::Cancel) {
        return QString();
    }
    
    bufferDirty = false;
    setWindowTitle(currentFile.isEmpty() ? "Notepad" : QFileInfo(currentFile).fileName() + " - Notepad");
    return QString();
}

void NotepadApp::closeEvent(QCloseEvent *event)
{
    if (isBufferDirty()) {
        QString savedFile = askSave();
        if (savedFile.isEmpty()) {
            event->ignore();
            return;
        }
    }
    setDirty(false);
    QMainWindow::closeEvent(event);
}
