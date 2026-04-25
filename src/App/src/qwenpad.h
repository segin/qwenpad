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
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QKeySequence>
#include <QTimer>
#include "tabmanager.h"

class SyntaxHighlighter;

class Qwenpad : public QMainWindow
{
    Q_OBJECT

  public:
    explicit Qwenpad(QWidget *parent = nullptr);
    void openFiles(const QStringList &files);

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
 
    void onFont();
    void onFind();
    void onFindNext();
    void onReplace();
    void onReplaceAll();
    void onCloseFindDialog();
    void onGoToLine();
    void updateLineInfo();
    void detectLineEndings();
    void detectLanguage();
    void onConvertCrlf();
    void onConvertLf();
    void onConvertCr();
    void onCloseTab();
    void onZoomIn();
    void onZoomOut();
    void updateStatusBar();
    void onStatusLineEndingClicked();
    void onStatusLanguageClicked();
    void onCurrentTabChanged();
    void onUndo();
    void onRedo();

    void saveSession();
    void restoreSession();
    QStringList getSessionFiles();

    private:

    QTextEdit *currentEditor();
    QTextEdit *previousEditor;
    TabManager *tabManager;
    QMenuBar *menubar;
    QToolBar *toolbar;
    QLabel *lineInfoLabel;
    QPushButton *statusLineEndingLabel;
    QPushButton *statusLanguageLabel;
    QDialog *findDialog;
    QDialog *goToLineDialog;
    QLineEdit *findLineEdit;
    QLineEdit *replaceLineEdit;
    QLineEdit *goToLineEdit;
    QPushButton *findNextButton;
    QPushButton *replaceButton;
    QPushButton *replaceAllButton;
    QPushButton *closeButton;
    QAction *goToLineAction;
    QCheckBox *regexCheckBox;

    bool bufferDirty;
    bool wordWrapEnabled;
    int untitledTabCount;
    int currentLineEndingType;
    QString currentHighlighterLanguage;
    QFont currentFont;
    QTimer *detectLanguageTimer;

protected:
    void resizeEvent(QResizeEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

    void keyPressEvent(QKeyEvent *event) override;

private:
    void setupUI();
    void setupActions();
    void setupMenuBar();
    void setupToolBar();
    void setupStatusBar();
    void setupFindDialog();
    void setupGoToLineDialog();
    void setDirty(bool dirty);
    bool askSave(EditorTab *tab);

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
    QAction *fontAction;
    QAction *findAction;
    QAction *toolbarNewAction;
    QAction *toolbarOpenAction;
    QAction *toolbarSaveAction;
};

#endif // QWENPAD_H
