#ifndef EDITORTAB_H
#define EDITORTAB_H

#include <QWidget>
#include <QTextEdit>
#include <QString>
#include <QEvent>
#include <QKeyEvent>
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
    bool getAutoIndentEnabled() const;
    void setAutoIndentEnabled(bool enabled);

    void setupEditor();
    void loadFile(const QString &fileName);
    void setHighlighterLanguage(const QString &language);
    QString getHighlighterLanguage() const;
    void saveFile(const QString &fileName);
    void clear();

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

signals:
    void dirtyChanged(bool dirty);

private:
    QTextEdit *editor;
    SyntaxHighlighter *highlighter;
    QString currentFile;
    QString initialLoadedText;
    bool bufferDirty;
    int lineEndingType;
    bool autoIndentEnabled;
};

#endif // EDITORTAB_H
