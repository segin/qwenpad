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

Qwenpad::Qwenpad(QWidget *parent)
    : QMainWindow(parent)
    , bufferDirty(false)
    , wordWrapEnabled(true)
    , lineNumbersEnabled(false)
    , currentLineEndingType(0) // 0 = LF
{
    setupUI();
    editor->installEventFilter(this);
}

void Qwenpad::setupUI()
{
    menubar = new QMenuBar(this);
    toolbar = new QToolBar(this);

    // Create main layout with line numbers frame and editor
    lineNumbersContainer = new QWidget(this);
    editor = new QTextEdit(this);
    
    QFont font;
    font.setFamily("Monospace");
    font.setFixedPitch(true);
    font.setPointSize(10);
    editor->setFont(font);

    // Setup line numbers styling
    lineNumberFont = editor->font();
    lineNumberFont.setPointSize(lineNumberFont.pointSize() - 1);
    lineNumberFont.setBold(false);
    
    // Create line numbers display frame
    lineNumbersFrame = new QFrame(this);
    lineNumbersFrame->setStyleSheet("background-color: #f0f0f0;");
    lineNumbersFrame->setFrameStyle(QFrame::NoFrame);
    
    // Create layout for line numbers container
    mainLayout = new QVBoxLayout(lineNumbersContainer);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    
    // Layout with line numbers frame and editor
    QHBoxLayout *editorLayout = new QHBoxLayout();
    editorLayout->setSpacing(0);
    editorLayout->setContentsMargins(0, 0, 0, 0);
    
    editorLayout->addWidget(lineNumbersFrame);
    editorLayout->addWidget(editor);
    
    mainLayout->addLayout(editorLayout);
    mainLayout->setStretch(1, 1);
    
    // Set the lineNumbersContainer as central widget
    setCentralWidget(lineNumbersContainer);

    editor->installEventFilter(this);
    connect(editor, &QTextEdit::cursorPositionChanged, this, &Qwenpad::updateLineInfo);
    
    // Sync scroll between line numbers and editor
    editor->installEventFilter(this);
    connect(editor->verticalScrollBar(), SIGNAL(valueChanged(int)), 
            this, SLOT(syncLineNumbersScroll(int)));
    
    // Connect document change to redraw line numbers
    connect(editor->document(), &QTextDocument::contentsChanged, 
            this, &Qwenpad::drawLineNumbers);
    
    // Draw initial line numbers if enabled
    if (lineNumbersEnabled) {
        drawLineNumbers();
    }

 cutAction = new QAction(QIcon::fromTheme("edit-cut", QIcon::fromTheme("edit_cut")), tr("Cut"), this);
    cutAction->setShortcuts(QKeySequence::Cut);
    cutAction->setToolTip(tr("Cut"));
    connect(cutAction, &QAction::triggered, editor, &QTextEdit::cut);

    copyAction = new QAction(QIcon::fromTheme("edit-copy", QIcon::fromTheme("edit_copy")), tr("Copy"), this);
    copyAction->setShortcuts(QKeySequence::Copy);
    copyAction->setToolTip(tr("Copy"));
    connect(copyAction, &QAction::triggered, editor, &QTextEdit::copy);

    pasteAction = new QAction(QIcon::fromTheme("edit-paste", QIcon::fromTheme("edit_paste")), tr("Paste"), this);
    pasteAction->setShortcuts(QKeySequence::Paste);
    pasteAction->setToolTip(tr("Paste"));
    connect(pasteAction, &QAction::triggered, editor, &QTextEdit::paste);

    selectAllAction = new QAction(QIcon::fromTheme("edit-select-all", QIcon::fromTheme("select_all")), tr("Select All"), this);
    selectAllAction->setShortcuts(QKeySequence::SelectAll);
    connect(selectAllAction, &QAction::triggered, editor, &QTextEdit::selectAll);

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

   // File menu with connected actions
    auto *fileMenu = new QMenu(tr("File"), menubar);
    QAction *newAction = new QAction(QIcon::fromTheme("document-new"), tr("&New"), this);
    newAction->setShortcut(QKeySequence::New);
    connect(newAction, &QAction::triggered, this, &Qwenpad::onNew);
    fileMenu->addAction(newAction);

    QAction *openAction = new QAction(QIcon::fromTheme("document-open"), tr("&Open"), this);
    openAction->setShortcut(QKeySequence::Open);
    connect(openAction, &QAction::triggered, this, &Qwenpad::onOpen);
    fileMenu->addAction(openAction);

    QAction *saveAction = new QAction(QIcon::fromTheme("document-save"), tr("&Save"), this);
    saveAction->setShortcut(QKeySequence::Save);
    connect(saveAction, &QAction::triggered, this, &Qwenpad::onSave);
    fileMenu->addAction(saveAction);

    fileMenu->addSeparator();
    fileMenu->addAction(quitAction);

    // Undo/Redo actions with shortcuts
    undoAction = new QAction(QIcon::fromTheme("edit-undo", QIcon::fromTheme("edit_undo")), tr("&Undo"), this);
    undoAction->setShortcut(QKeySequence::Undo);
    undoAction->setEnabled(false);
    
    redoAction = new QAction(QIcon::fromTheme("edit-redo", QIcon::fromTheme("edit_redo")), tr("&Redo"), this);
    redoAction->setShortcut(QKeySequence::Redo);
    redoAction->setEnabled(false);

  // Find action (renamed and moved to bottom)
    findAction = new QAction(QIcon::fromTheme("edit-find", QIcon::fromTheme("edit_find")), tr("Find/Replace..."), this);
    findAction->setShortcut(QKeySequence("Ctrl+F"));
    connect(findAction, &QAction::triggered, this, &Qwenpad::onFind);
    
    // Create Find/Replace dialog
    createFindDialog();
    
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
    helpMenu->addAction(new QAction(tr("&Documentation"), this));

    menubar->addMenu(fileMenu);
    menubar->addMenu(editMenu);
    menubar->addMenu(viewMenu);
    menubar->addMenu(helpMenu);
    setMenuBar(menubar);

 toolbar->setIconSize(QSize(16, 16));
     // File actions first with icons
     QAction *toolbarNew = new QAction(QIcon::fromTheme("document-new"), tr("New"), this);
     toolbarNew->setShortcut(QKeySequence::New);
     connect(toolbarNew, &QAction::triggered, this, &Qwenpad::onNew);
     toolbar->addAction(toolbarNew);
     
     QAction *toolbarOpen = new QAction(QIcon::fromTheme("document-open"), tr("Open"), this);
     toolbarOpen->setShortcut(QKeySequence::Open);
     connect(toolbarOpen, &QAction::triggered, this, &Qwenpad::onOpen);
     toolbar->addAction(toolbarOpen);
     
     QAction *toolbarSave = new QAction(QIcon::fromTheme("document-save"), tr("Save"), this);
     toolbarSave->setShortcut(QKeySequence::Save);
     connect(toolbarSave, &QAction::triggered, this, &Qwenpad::onSave);
     toolbar->addAction(toolbarSave);
     
     toolbar->addSeparator();
     // Edit actions with icons only
     toolbar->addAction(cutAction);
     toolbar->addAction(copyAction);
     toolbar->addAction(pasteAction);
     addToolBar(toolbar);

   // Status bar with line info
    setStatusBar(new QStatusBar(this));
    lineInfoLabel = new QLabel(tr("Ln 0, Col 0"));
    statusBar()->addPermanentWidget(lineInfoLabel);
    // Connect editor text change to our slot
    connect(editor, &QTextEdit::textChanged, this, &Qwenpad::onTextChange);
    connect(editor, &QTextEdit::undoAvailable, undoAction, &QAction::setEnabled);
    connect(editor, &QTextEdit::undoAvailable, redoAction, &QAction::setEnabled);
    connect(editor, &QTextEdit::redoAvailable, redoAction, &QAction::setEnabled);

    statusBar()->showMessage(tr("Notepad v0.1"));
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

void Qwenpad::onSave()
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
    QMessageBox::information(this, tr("About"), tr("Notepad v0.1"));
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
    
    if (lineNumbersEnabled) {
        drawLineNumbers();
    } else {
        // Hide line numbers
        if (lineNumbersFrame) {
            lineNumbersFrame->setVisible(false);
        }
    }
}

void Qwenpad::onFont()
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok, this);
    if (ok) {
        editor->setFont(font);
    }
}

void Qwenpad::onConvertCrlf()
{
    QString content = editor->toPlainText();
    QString converted = convertLineEndings(content, 1); // 1 = CRLF
    editor->setPlainText(converted);
    currentLineEndingType = 1;
    setDirty(true);
    detectLineEndings();
}

void Qwenpad::onConvertLf()
{
    QString content = editor->toPlainText();
    QString converted = convertLineEndings(content, 0); // 0 = LF
    editor->setPlainText(converted);
    currentLineEndingType = 0;
    setDirty(true);
    detectLineEndings();
}

void Qwenpad::onConvertCr()
{
    QString content = editor->toPlainText();
    QString converted = convertLineEndings(content, 2); // 2 = CR
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
        currentLineEndingType = 1; // CRLF
    } else if (crCount > lfCount) {
        currentLineEndingType = 2; // CR
    } else {
        currentLineEndingType = 0; // LF
    }
}

QString Qwenpad::convertLineEndings(const QString &text, int target)
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

void Qwenpad::onTextChange()
{
    setDirty(true);
    updateLineInfo();
}

void Qwenpad::setDirty(bool dirty)
{
    bufferDirty = dirty;
    QString title = currentFile.isEmpty() ? tr("Notepad") : QFileInfo(currentFile).fileName();
    if (bufferDirty) {
        title += tr(" *");
    }
    title += tr(" - Notepad");
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
    setWindowTitle(currentFile.isEmpty() ? tr("Notepad") : QFileInfo(currentFile).fileName() + tr(" - Notepad"));
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
        // Discard - clear dirty and continue closing
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

void Qwenpad::syncLineNumbersScroll(int value)
{
    if (lineNumbersEnabled) {
        // No need to sync - editor scrolls automatically
        (void)value;
    }
}

void Qwenpad::drawLineNumbers()
{
    if (!lineNumbersEnabled || !lineNumbersFrame) return;
    
    QTextDocument *doc = editor->document();
    int blockCount = doc->blockCount();
    static QLabel *lineLabel = nullptr;
    
    if (!lineLabel) {
        lineLabel = new QLabel(lineNumbersFrame);
        lineLabel->setStyleSheet("background-color: #f0f0f0;");
        lineLabel->setFont(lineNumberFont);
        
        // Create layout
        QVBoxLayout *layout = new QVBoxLayout(lineNumbersFrame);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(0);
        layout->addWidget(lineLabel);
    }
    
    // Build line numbers text
    QString lineNumbersText;
    for (int i = 1; i <= blockCount; i++) {
        lineNumbersText += QString::number(i) + "\n";
    }
    
    lineLabel->setText(lineNumbersText);
    
    // Set width based on max line number
    QFontMetrics fm = editor->fontMetrics();
    int digits = QString::number(blockCount).length();
  int width = (digits + 2) * fm.horizontalAdvance('0') + 5;
     lineNumbersFrame->setFixedWidth(width);
    
    // Sync scrolling
    connect(editor->verticalScrollBar(), SIGNAL(valueChanged(int)), 
            lineLabel, SLOT(scrollContentsBy(int)));
    
     // Update on document change
    connect(doc, &QTextDocument::blockCountChanged, this, &Qwenpad::drawLineNumbers);
}

void Qwenpad::createFindDialog()
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
    
    if (foundCursor.position() >= 0) {
        editor->setTextCursor(foundCursor);
        editor->ensureCursorVisible();
    } else {
        // Reset to start of document
        cursor.setPosition(0);
        editor->setTextCursor(cursor);
        foundCursor = cursor.document()->find(searchText);
        
        if (foundCursor.position() >= 0) {
            editor->setTextCursor(foundCursor);
            editor->ensureCursorVisible();
        } else {
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
    
    QTextCursor cursor = editor->textCursor();
    QTextCursor foundCursor = cursor.document()->find(searchText);
    
    if (foundCursor.position() >= 0) {
        cursor = editor->textCursor();
        cursor.insertText(replaceText);
        editor->setTextCursor(cursor);
        editor->ensureCursorVisible();
    } else {
        // Reset to start of document
        cursor.setPosition(0);
        editor->setTextCursor(cursor);
        foundCursor = cursor.document()->find(searchText);
        
        if (foundCursor.position() < 0) {
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
    
    QString originalText = editor->toPlainText();
    QString modifiedText = originalText.replace(searchText, replaceText);
    
    editor->setPlainText(modifiedText);
    setDirty(true);
}

void Qwenpad::onCloseFindDialog()
{
    findDialog->close();
}