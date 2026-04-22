#include "edittab.h"
#include "utils.h"
#include <QFile>
#include <QTextStream>
#include <QFont>
#include <QVBoxLayout>
#include <QTextOption>
#include <QFileInfo>

EditorTab::EditorTab(const QString &title, QWidget *parent)
    : QWidget(parent)
    , editor(nullptr)
    , lineNumberWidget(nullptr)
    , highlighter(nullptr)
    , currentFile(title)
    , bufferDirty(false)
    , lineEndingType(0)
{
    setupEditor();
}

QTextEdit *EditorTab::getEditor() const
{
    return editor;
}

LineEditWidget *EditorTab::getLineNumberWidget() const
{
    return lineNumberWidget;
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

void EditorTab::setupEditor()
{
    editor = new QTextEdit(nullptr);

    QFont font;
    font.setFamily("Monospace");
    font.setFixedPitch(true);
    font.setPointSize(10);
    editor->setFont(font);
    editor->setWordWrapMode(QTextOption::WrapMode::WordWrap);
    editor->setPlainText("");

    lineNumberWidget = new LineEditWidget(editor, nullptr);
    lineNumberWidget->setFont(editor->font());
    lineNumberWidget->setEnabled(false);

    highlighter = new SyntaxHighlighter(editor->document());

    connect(editor, &QTextEdit::textChanged, this, [this]() {
        setDirty(true);
    });

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(lineNumberWidget);
    layout->addWidget(editor);
    layout->setContentsMargins(0, 0, 0, 0);
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

void EditorTab::clear()
{
    editor->clear();
    currentFile.clear();
    bufferDirty = false;
    emit dirtyChanged(false);
}
