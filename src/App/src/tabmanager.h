#ifndef TABMANAGER_H
#define TABMANAGER_H

#include <QTabWidget>
#include <QLabel>
#include <QTextCursor>
#include <QFont>
#include <QString>
#include "edittab.h"

class TabManager : public QTabWidget
{
    Q_OBJECT

public:
    explicit TabManager(QWidget *parent = nullptr);

    EditorTab *currentTab() const;
    EditorTab *addTab(const QString &title);
    void closeCurrentTab();
    bool closeTab(EditorTab *tab);
    int count() const;

    EditorTab *getEditorTab(int index) const;
    void find(const QString &text);
    bool findNext(const QString &text);
    bool replace(const QString &searchText, const QString &replaceText);
    int replaceAll(const QString &searchText, const QString &replaceText);

    void saveCurrentFile(const QString &fileName);
    QString saveCurrentFile();

    void setCurrentTabDirty(bool dirty);
    bool isCurrentTabDirty() const;
    QString getCurrentTabTitle() const;
    void updateCurrentTabTitle();

    void detectLineEndings();
    void convertLineEndings(int targetType);

    void setupFont(const QFont &font);
    void setupLineNumbers(bool enable);

signals:
    void tabCountChanged(int count);
    void currentTabDirtyChanged(bool dirty);

private:
    void updateLineInfo();

    QLabel *lineInfoLabel;
    QFont currentFont;
    bool lineNumbersEnabled;
};

#endif // TABMANAGER_H
