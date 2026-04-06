#ifndef NOTEPADAPP_H
#define NOTEPADAPP_H

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

class NotepadApp : public QMainWindow
{
    Q_OBJECT

private:
    QMenuBar *menubar;
    QToolBar *toolbar;
    QAction *cutAction;
    QAction *copyAction;
    QAction *pasteAction;
    QAction *selectAllAction;
    QAction *aboutAction;
    QAction *quitAction;
    QAction *wrapAction;
    QAction *lineNumbersAction;
    QAction *fontAction;
    QTextEdit *editor;
    QString currentFile;
    bool bufferDirty;
    bool wordWrapEnabled;
    bool lineNumbersEnabled;
    QStatusBar *statusBar;
    QLabel *lineInfoLabel;
    QLabel *crLfInfoLabel;
    QLabel *lfInfoLabel;
    QFont lineNumberFont;

public:
    explicit NotepadApp(QWidget *parent = nullptr);
    void setupUI();

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

    void updateLineInfo();

private:
   void setDirty(bool dirty);
   QString askSave();

protected:
    void resizeEvent(QResizeEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;
};

#endif // NOTEPADAPP_H
