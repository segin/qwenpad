#ifndef TABMANAGER_H
#define TABMANAGER_H

#include <QTabWidget>
#include <QTabBar>
#include <QMouseEvent>
#include <QLabel>
#include <QTextCursor>
#include <QFont>
#include <QString>
#include "edittab.h"

class TabBar : public QTabBar
{
    Q_OBJECT

public:
    explicit TabBar(QWidget *parent = nullptr) : QTabBar(parent) {}

protected:
    void mousePressEvent(QMouseEvent *event) override
    {
        if (event->button() == Qt::MiddleButton) {
            int tab = tabAt(event->pos());
            if (tab >= 0) {
                emit closeRequested(tab);
                return;
            }
        }
        QTabBar::mousePressEvent(event);
    }

signals:
    void closeRequested(int index);

private:
    void mouseReleaseEvent(QMouseEvent *event) override
    {
        QTabBar::mouseReleaseEvent(event);
    }

    void mouseMoveEvent(QMouseEvent *event) override
    {
        QTabBar::mouseMoveEvent(event);
    }
};

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
    void tabCloseRequested(int index);

private:
    void updateLineInfo();

    QLabel *lineInfoLabel;
    QFont currentFont;
    bool lineNumbersEnabled;
};

#endif // TABMANAGER_H
