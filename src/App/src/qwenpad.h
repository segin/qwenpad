#ifndef QWENPAD_H
#define QWENPAD_H

#include <QMainWindow>
#include <QMenuBar>
#include <QToolBar>
#include <QAction>
#include <QMenu>
#include <QLabel>
#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QKeySequence>
#include "tabmanager.h"

class SyntaxHighlighter;

class Qwenpad : public QMainWindow
{
    Q_OBJECT

  public:
    explicit Qwenpad(QWidget *parent = nullptr);

private slots:
    void onNew();
    void onOpen();
    void onSave();
    void onCut();
    void onCopy();
    void onPaste();
    void onSelectAll();
    void onAbout();
    void onWrap();
    void onToggleLineNumbers();
    void onFont();
    void onFind();
    void onFindNext();
    void onReplace();
    void onReplaceAll();
    void onCloseFindDialog();
    void updateLineInfo();
    void detectLineEndings();
    void onConvertCrlf();
    void onConvertLf();
    void onConvertCr();
    void onCloseTab();
    void onCurrentTabChanged();
    void onUndo();
    void onRedo();

    private:

    QTextEdit *currentEditor();
    TabManager *tabManager;
    QMenuBar *menubar;
    QToolBar *toolbar;
    QLabel *lineInfoLabel;
    QDialog *findDialog;
    QLineEdit *findLineEdit;
    QLineEdit *replaceLineEdit;
    QPushButton *findNextButton;
    QPushButton *replaceButton;
    QPushButton *replaceAllButton;
    QPushButton *closeButton;

    bool bufferDirty;
    bool wordWrapEnabled;
    bool lineNumbersEnabled;
    int currentLineEndingType;

protected:
    void resizeEvent(QResizeEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private:
    void setupUI();
    void setupActions();
    void setupMenuBar();
    void setupToolBar();
    void setupStatusBar();
    void setupFindDialog();
    void setDirty(bool dirty);
    QString askSave(EditorTab *tab);

    QAction *newAction;
    QAction *openAction;
    QAction *saveAction;
    QAction *closeTabAction;
    QAction *cutAction;
    QAction *copyAction;
    QAction *pasteAction;
    QAction *selectAllAction;
    QAction *undoAction;
    QAction *redoAction;
    QAction *aboutAction;
    QAction *quitAction;
    QAction *wrapAction;
    QAction *lineNumbersAction;
    QAction *fontAction;
    QAction *findAction;
    QAction *toolbarNewAction;
    QAction *toolbarOpenAction;
    QAction *toolbarSaveAction;
};

#endif // QWENPAD_H
