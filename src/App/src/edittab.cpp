#include "edittab.h"
#include "utils.h"
#include <QFile>
#include <QTextStream>
#include <QVBoxLayout>
#include <QTextOption>
#include <QFileInfo>
#include <QRegularExpression>

EditorTab::EditorTab(const QString &title, QWidget *parent)
    : QWidget(parent)
    , editor(nullptr)
    , highlighter(nullptr)
    , currentFile(title)
    , bufferDirty(false)
    , lineEndingType(0)
    , autoIndentEnabled(true)
{
    setupEditor();
}

QTextEdit *EditorTab::getEditor() const
{
    return editor;
}

QString EditorTab::getFile() const
{
    return currentFile;
}

void EditorTab::setFile(const QString &file)
{
    currentFile = file;
}

bool EditorTab::isDirty() const
{
    return bufferDirty;
}

void EditorTab::setDirty(bool dirty)
{
    bufferDirty = dirty;
    emit dirtyChanged(dirty);
}

void EditorTab::setLineEndingType(int type)
{
    lineEndingType = type;
}

int EditorTab::getLineEndingType() const
{
    return lineEndingType;
}

bool EditorTab::getAutoIndentEnabled() const
{
    return autoIndentEnabled;
}

void EditorTab::setAutoIndentEnabled(bool enabled)
{
    autoIndentEnabled = enabled;
}

void EditorTab::setupEditor()
{
    editor = new QTextEdit(nullptr);

    QFont font;
    font.setFamily("Monospace");
    font.setPointSize(10);
    editor->setFont(font);
    editor->setWordWrapMode(QTextOption::WrapMode::WordWrap);
    editor->setPlainText("");

    highlighter = new SyntaxHighlighter(editor->document());

    editor->installEventFilter(this);
    editor->viewport()->installEventFilter(this);

    connect(editor, &QTextEdit::textChanged, this, [this]() {
        QString currentText = editor->toPlainText();
        if (initialLoadedText.isEmpty() && currentFile.isEmpty()) {
            return;
        }
        bufferDirty = (currentText != initialLoadedText);
        emit dirtyChanged(bufferDirty);
    });

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(editor);
}

void EditorTab::loadFile(const QString &fileName)
{
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        QString content = stream.readAll();
        file.close();
        editor->setPlainText(content);
        currentFile = fileName;
        initialLoadedText = content;
        bufferDirty = false;
        emit dirtyChanged(false);

        QString extension = QFileInfo(fileName).suffix().toLower();
        if (extension == "cpp" || extension == "hpp" || extension == "h" || extension == "c") {
            highlighter->setLanguage("C++");
        } else if (extension == "py" || extension == "pyw") {
            highlighter->setLanguage("Python");
        } else {
            highlighter->setLanguage("Text");
        }
    }
}

void EditorTab::saveFile(const QString &fileName)
{
    QString content = editor->toPlainText();
    content = Utils::convertLineEndings(content, lineEndingType);

    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        stream << content;
        file.close();

        currentFile = fileName;
        initialLoadedText = content;
        bufferDirty = false;
        emit dirtyChanged(false);
    }
}

void EditorTab::setHighlighterLanguage(const QString &language)
{
    if (highlighter) {
        highlighter->setLanguage(language);
        highlighter->rehighlight();
    }
}

 QString EditorTab::getHighlighterLanguage() const
{
    if (highlighter) {
        return highlighter->getLanguage();
    }
    return "Text";
}

void EditorTab::clear()
{
    editor->clear();
    currentFile.clear();
    initialLoadedText.clear();
    bufferDirty = false;
    emit dirtyChanged(false);
}

 bool EditorTab::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == editor && event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if (autoIndentEnabled && (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter)) {
            QTextCursor cursor = editor->textCursor();
            QString previousText = cursor.block().text();
            QRegularExpressionMatch match = QRegularExpression("^(\\s*)").match(previousText);
            
            if (match.hasMatch()) {
                QString indentation = match.captured(1);
                cursor.insertText(indentation);
                cursor.insertBlock();
                return true;
            }
        }
    }
    
    if (obj == editor->viewport() && event->type() == QEvent::Wheel) {
        QWheelEvent *wheel = static_cast<QWheelEvent *>(event);
        if (wheel->modifiers() & Qt::ControlModifier) {
            if (wheel->angleDelta().y() > 0) {
                editor->zoomIn();
            } else {
                editor->zoomOut();
            }
            return true;
        }
    }
    return QWidget::eventFilter(obj, event);
}
