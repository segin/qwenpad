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
#include <QLabel>
#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QKeySequence>

class Qwenpad : public QMainWindow
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
    QAction *convertLfAction;
    QAction *convertCrAction;
    QAction *findAction;
    QTextEdit *editor;
    QMenuBar *menubar;
    QToolBar *toolbar;
    QString currentFile;
    bool bufferDirty;
    bool wordWrapEnabled;
    bool lineNumbersEnabled;
    QFont lineNumberFont;
    QLabel *lineInfoLabel;
    QFrame *lineNumbersFrame;
    QVBoxLayout *mainLayout;
    QWidget *lineNumbersContainer;
    QDialog *findDialog;
    QLineEdit *findLineEdit;
    QLineEdit *replaceLineEdit;
    QPushButton *findNextButton;
    QPushButton *replaceButton;
    QPushButton *replaceAllButton;
    QPushButton *closeButton;

public:
    explicit Qwenpad(QWidget *parent = nullptr);
    void setupUI();
    void createFindDialog();

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

    void updateLineInfo();
    void detectLineEndings();
    void syncLineNumbersScroll(int value);
    void drawLineNumbers();
    void onFindNext();
    void onReplace();
    void onReplaceAll();
    void onCloseFindDialog();

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

#endif // QWENPAD_H
