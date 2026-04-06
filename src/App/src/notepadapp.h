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
    QAction *fontAction;
    QTextEdit *editor;
    QString currentFile;
    bool bufferDirty;

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
    void onFont();
    void onTextChange();

private:
    void setDirty(bool dirty);
    QString askSave();

protected:
    void resizeEvent(QResizeEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
};

#endif // NOTEPADAPP_H
