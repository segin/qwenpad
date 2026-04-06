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

NotepadApp::NotepadApp(QWidget *parent)
    : QMainWindow(parent)
    , bufferDirty(false)
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
    setCentralWidget(editor);

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

    fontAction = new QAction(tr("Font..."), this);
    fontAction->setShortcut(QKeySequence("Ctrl+F"));
    connect(fontAction, &QAction::triggered, this, &NotepadApp::onFont);

    auto *fileMenu = new QMenu(tr("File"), menubar);
    fileMenu->addAction(new QAction(tr("&New"), this));
    fileMenu->addAction(new QAction(tr("&Open"), this));
    fileMenu->addAction(new QAction(tr("&Save"), this));
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
        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream stream(&file);
            editor->setPlainText(stream.readAll());
            file.close();
            currentFile = fileName;
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
            stream << editor->toPlainText();
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