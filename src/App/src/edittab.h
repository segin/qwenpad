#ifndef EDITORTAB_H
#define EDITORTAB_H

#include <QWidget>
#include <QTextEdit>
#include <QString>
#include "lineeditwidget.h"
#include "syntaxhighlighter.h"

class EditorTab : public QWidget
{
    Q_OBJECT

public:
    explicit EditorTab(const QString &title, QWidget *parent = nullptr);

    QTextEdit *getEditor() const;
    QString getFile() const;
    void setFile(const QString &file);
    bool isDirty() const;
    void setDirty(bool dirty);
    void setLineEndingType(int type);
    int getLineEndingType() const;
    LineEditWidget *getLineNumberWidget() const;

    void setupEditor();
    void loadFile(const QString &fileName);
    void setHighlighterLanguage(const QString &language);
    void saveFile(const QString &fileName);
    void clear();

signals:
    void dirtyChanged(bool dirty);

private:
    QTextEdit *editor;
    LineEditWidget *lineNumberWidget;
    SyntaxHighlighter *highlighter;
    QString currentFile;
    bool bufferDirty;
    int lineEndingType;
};

#endif // EDITORTAB_H
