#ifndef QWENPAD_H
#define QWENPAD_H

#include <QMainWindow>
#include <QMenuBar>
#include <QToolBar>
#include <QAction>
#include <QMenu>
#include <QTextEdit>
#include <QString>
#include <QResizeEvent>
#include <QCloseEvent>
#include <QLabel>
#include <QVBoxLayout>
#include <QFrame>
#include <QFont>
#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QKeySequence>
#include "lineeditwidget.h"

class Qwenpad : public QMainWindow
{
    Q_OBJECT

  public:
    explicit Qwenpad(QWidget *parent = nullptr);
    QString convertLineEndings(const QString &text, int target);

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
    void onTextChange();
    void onConvertCrlf();
    void onConvertLf();
    void onConvertCr();
    void onFind();
    void onFindNext();
    void onReplace();
    void onReplaceAll();
    void onCloseFindDialog();
    void updateLineInfo();
    void detectLineEndings();
    void drawLineNumbers();

private:

    QTextEdit *editor;
    LineEditWidget *lineNumberWidget;
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

    QString currentFile;
    bool bufferDirty;
    bool wordWrapEnabled;
    bool lineNumbersEnabled;
    int currentLineEndingType;

protected:
    void resizeEvent(QResizeEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    void setupUI();
    void setupEditor();
    void setupActions();
    void setupMenuBar();
    void setupToolBar();
    void setupStatusBar();
    void setupFindDialog();
    void setDirty(bool dirty);
    QString askSave();
    QAction *cutAction;
    QAction *copyAction;
    QAction *pasteAction;
    QAction *selectAllAction;
    QAction *aboutAction;
    QAction *quitAction;
    QAction *wrapAction;
    QAction *lineNumbersAction;
    QAction *fontAction;
    QAction *undoAction;
    QAction *redoAction;
    QAction *findAction;
    QAction *toolbarNewAction;
    QAction *toolbarOpenAction;
    QAction *toolbarSaveAction;

    QAction *newAction;
    QAction *openAction;
    QAction *saveAction;
};

#endif // QWENPAD_H
