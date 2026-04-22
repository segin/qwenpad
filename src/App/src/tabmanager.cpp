#include "tabmanager.h"
#include "utils.h"
#include <QFile>
#include <QTextStream>
#include <QFileInfo>
#include <QLabel>
#include <QMessageBox>
#include <QFileDialog>
#include <QFont>
#include <QTextOption>
#include <QTabWidget>

TabManager::TabManager(QWidget *parent)
    : QTabWidget(parent)
    , lineInfoLabel(nullptr)
    , lineNumbersEnabled(false)
{
    addTab(tr("(untitled)"));
    connect(this, &QTabWidget::currentChanged, this, &TabManager::updateCurrentTabTitle);
}

EditorTab *TabManager::currentTab() const
{
    if (count() > 0) {
        return static_cast<EditorTab *>(currentWidget());
    }
    return nullptr;
}

EditorTab *TabManager::getEditorTab(int index) const
{
    if (index >= 0 && index < count()) {
        return static_cast<EditorTab *>(QTabWidget::widget(index));
    }
    return nullptr;
}

EditorTab *TabManager::addTab(const QString &title)
{
    EditorTab *tab = new EditorTab(title);
    tab->setupEditor();
    QTabWidget::addTab(tab, title);
    setCurrentWidget(tab);
    emit tabCountChanged(count());
    return tab;
}

void TabManager::closeCurrentTab()
{
   if (count() > 1) {
        EditorTab *tab = currentTab();
        if (tab->isDirty()) {
            int reply = QMessageBox::question(this, tr("Save Changes"),
                tr("The current file has unsaved changes. Do you want to save them?"),
                QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

            if (reply == QMessageBox::Save) {
                QString file = tab->getFile();
                if (!file.isEmpty()) {
                    tab->saveFile(file);
                } else {
                    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                        QString(), tr("Text Files (*.txt)"));
                    if (!fileName.isEmpty()) {
                        tab->saveFile(fileName);
                    }
                }
                if (tab->isDirty()) {
                    return;
                }
            } else if (reply == QMessageBox::Cancel) {
                return;
            }
        }
        int index = QTabWidget::indexOf(tab);
        if (index >= 0) {
            QTabWidget::removeTab(index);
        }
        emit tabCountChanged(count());
    } else {
        EditorTab *tab = currentTab();
        int index = QTabWidget::indexOf(tab);
        if (index >= 0) {
            if (tab->isDirty()) {
                int reply = QMessageBox::question(this, tr("Save Changes"),
                    tr("The current file has unsaved changes. Do you want to save them?"),
                    QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

                if (reply == QMessageBox::Save) {
                    QString file = tab->getFile();
                    if (!file.isEmpty()) {
                        tab->saveFile(file);
                    } else {
                        QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                            QString(), tr("Text Files (*.txt)"));
                        if (!fileName.isEmpty()) {
                            tab->saveFile(fileName);
                        }
                    }
                    if (tab->isDirty()) {
                        return;
                    }
                } else if (reply == QMessageBox::Cancel) {
                    return;
                }
            }
            QTabWidget::removeTab(index);
            emit tabCountChanged(count());
        }
        addTab(tr("(untitled)"));
    }
}

bool TabManager::closeTab(EditorTab *tab)
{
    if (tab == currentTab() && count() > 1) {
        return true;
    }
    int index = QTabWidget::indexOf(tab);
    if (index >= 0) {
        QTabWidget::removeTab(index);
        emit tabCountChanged(count());
        return true;
    }
    return false;
}

int TabManager::count() const
{
    return QTabWidget::count();
}

void TabManager::find(const QString &text)
{
    EditorTab *tab = currentTab();
    if (!tab) return;

    if (text.isEmpty()) return;

    QTextDocument *doc = tab->getEditor()->document();
    QTextCursor cursor = tab->getEditor()->textCursor();
    QTextCursor foundCursor = doc->find(text, cursor);

    if (foundCursor.isNull()) {
        cursor.setPosition(0);
        tab->getEditor()->setTextCursor(cursor);
        foundCursor = doc->find(text, cursor);
    }

    if (!foundCursor.isNull()) {
        tab->getEditor()->setTextCursor(foundCursor);
        tab->getEditor()->ensureCursorVisible();
    }
}

bool TabManager::findNext(const QString &text)
{
    EditorTab *tab = currentTab();
    if (!tab) return false;

    if (text.isEmpty()) return false;

    QTextDocument *doc = tab->getEditor()->document();
    QTextCursor cursor = tab->getEditor()->textCursor();
    QTextCursor foundCursor = doc->find(text, cursor);

    if (foundCursor.isNull()) {
        cursor = tab->getEditor()->textCursor();
        cursor.setPosition(0);
        tab->getEditor()->setTextCursor(cursor);
        foundCursor = doc->find(text, cursor);
    }

    if (!foundCursor.isNull()) {
        tab->getEditor()->setTextCursor(foundCursor);
        tab->getEditor()->ensureCursorVisible();
        return true;
    }
    return false;
}

bool TabManager::replace(const QString &searchText, const QString &replaceText)
{
    EditorTab *tab = currentTab();
    if (!tab) return false;

    if (searchText.isEmpty()) return false;

    QTextDocument *doc = tab->getEditor()->document();
    QTextCursor cursor = tab->getEditor()->textCursor();
    QTextCursor foundCursor = doc->find(searchText, cursor);

    if (!foundCursor.isNull()) {
        foundCursor.insertText(replaceText);
        tab->getEditor()->setTextCursor(foundCursor);
        tab->getEditor()->ensureCursorVisible();
        return true;
    } else {
        cursor.setPosition(0);
        tab->getEditor()->setTextCursor(cursor);
        foundCursor = doc->find(searchText, cursor);
        if (!foundCursor.isNull()) {
            foundCursor.insertText(replaceText);
            tab->getEditor()->setTextCursor(foundCursor);
            tab->getEditor()->ensureCursorVisible();
            return true;
        }
    }
    return false;
}

int TabManager::replaceAll(const QString &searchText, const QString &replaceText)
{
    EditorTab *tab = currentTab();
    if (!tab) return 0;

    if (searchText.isEmpty()) return 0;

    QString modifiedText = tab->getEditor()->toPlainText();
    int replacementCount = modifiedText.count(searchText);

    if (replacementCount > 0) {
        QTextCursor cursor(tab->getEditor()->document());
        cursor.beginEditBlock();
        QString text = tab->getEditor()->toPlainText();
        int pos = 0;
        while ((pos = text.indexOf(searchText, pos)) >= 0) {
            cursor.setPosition(pos);
            if (searchText.length() > 0) {
                cursor.setPosition(pos + searchText.length(), QTextCursor::KeepAnchor);
                if (replaceText.isEmpty()) {
                    cursor.removeSelectedText();
                } else {
                    cursor.insertText(replaceText);
                }
            }
            text.replace(pos, searchText.length(), replaceText);
            pos += replaceText.length();
        }
        cursor.endEditBlock();
        tab->setDirty(true);
    }
    return replacementCount;
}

void TabManager::saveCurrentFile(const QString &fileName)
{
    EditorTab *tab = currentTab();
    if (!tab) return;
    tab->saveFile(fileName);
    updateCurrentTabTitle();
}

QString TabManager::saveCurrentFile()
{
    EditorTab *tab = currentTab();
    if (!tab) return QString();

    QString fileName = tab->getFile();
    if (fileName.isEmpty()) {
        fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
            QString(), tr("Text Files (*.txt)"));
        if (!fileName.isEmpty()) {
            tab->saveFile(fileName);
        }
    } else {
        tab->saveFile(fileName);
    }
    updateCurrentTabTitle();
    return tab->getFile();
}

void TabManager::setCurrentTabDirty(bool dirty)
{
    EditorTab *tab = currentTab();
    if (!tab) return;
    tab->setDirty(dirty);
    updateCurrentTabTitle();
}

bool TabManager::isCurrentTabDirty() const
{
    EditorTab *tab = currentTab();
    return tab ? tab->isDirty() : false;
}

QString TabManager::getCurrentTabTitle() const
{
    EditorTab *tab = currentTab();
    return tab ? tab->getFile() : QString();
}

void TabManager::updateCurrentTabTitle()
{
    EditorTab *tab = currentTab();
    if (!tab) return;

    QString title = tab->getFile().isEmpty() ? tr("(untitled)") : QFileInfo(tab->getFile()).fileName();
    if (tab->isDirty()) {
        title += tr(" *");
    }

    int index = QTabWidget::indexOf(tab);
    if (index >= 0) {
        setTabToolTip(index, tab->getFile());
        setTabText(index, title);
    }
}

void TabManager::detectLineEndings()
{
    EditorTab *tab = currentTab();
    if (!tab) return;

    QString content = tab->getEditor()->toPlainText();
    int crCount = content.count('\r');
    int lfCount = content.count('\n');
    int crlfCount = content.count("\r\n");

    if (crlfCount > crCount - crlfCount) {
        tab->setLineEndingType(1);
    } else if (crCount > lfCount) {
        tab->setLineEndingType(2);
    } else {
        tab->setLineEndingType(0);
    }
}

void TabManager::convertLineEndings(int targetType)
{
    EditorTab *tab = currentTab();
    if (!tab) return;

    QString content = tab->getEditor()->toPlainText();
    QString converted = Utils::convertLineEndings(content, targetType);
    tab->getEditor()->setPlainText(converted);
    tab->setLineEndingType(targetType);
    tab->setDirty(true);
}

void TabManager::setupFont(const QFont &font)
{
    for (int i = 0; i < count(); i++) {
        EditorTab *tab = static_cast<EditorTab *>(widget(i));
        if (tab) {
            tab->getEditor()->setFont(font);
            tab->getEditor()->document()->setDefaultFont(font);
            tab->getEditor()->setStyleSheet("QTextEdit { font: " + font.family() + " " + QString::number(font.pointSize()) + "pt; }");
        }
    }
    currentFont = font;
}

void TabManager::setupLineNumbers(bool enable)
{
    lineNumbersEnabled = enable;
    for (int i = 0; i < count(); i++) {
        EditorTab *tab = static_cast<EditorTab *>(widget(i));
        if (tab) {
            tab->getLineNumberWidget()->setVisible(enable);
        }
    }
}
