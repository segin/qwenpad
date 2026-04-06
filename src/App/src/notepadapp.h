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
    QAction *convertCrlfAction;
    QAction *convertLfAction;
    QAction *convertCrAction;
    QTextEdit *editor;
    QMenuBar *menubar;
    QToolBar *toolbar;
    QString currentFile;
    bool bufferDirty;
    bool wordWrapEnabled;
    bool lineNumbersEnabled;
    QFont lineNumberFont;
    QLabel *lineInfoLabel;

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
    void onConvertCrlf();
    void onConvertLf();
    void onConvertCr();

    void updateLineInfo();
    void detectLineEndings();

private:
    void setDirty(bool dirty);
    QString askSave();
    QString convertLineEndings(const QString &text, int target);
    int currentLineEndingType; // 0 = LF, 1 = CRLF, 2 = CR

protected:
    void resizeEvent(QResizeEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;
};

#endif // NOTEPADAPP_H
