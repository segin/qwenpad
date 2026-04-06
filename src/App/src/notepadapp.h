#ifndef NOTEPADAPP_H
#define NOTEPADAPP_H

#include <QMainWindow>
#include <QMenuBar>
#include <QToolBar>
#include <QAction>
#include <QMenu>
#include <QPushButton>
#include <QTextEdit>
#include <QString>
#include <QResizeEvent>
#include <QCloseEvent>
#include <QKeySequence>

class NotepadApp : public QMainWindow
{
    Q_OBJECT

private:
    QMenuBar *menubar;
    QToolBar *toolbar;
    QTextEdit *editor;
    QAction *cutAction, *copyAction, *pasteAction, *selectAllAction, *aboutAction, *quitAction, *wrapAction, *fontAction;
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
    void onselectAll();
    void onAbout();
    void onWrap();
    void onFont();

private:
    void updateUI();
    bool isBufferDirty() const;
    void setBufferDirty(bool dirty);
    QString askSave();

protected:
    void resizeEvent(QResizeEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
};

#endif // NOTEPADAPP_H
