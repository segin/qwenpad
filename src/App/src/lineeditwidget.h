#ifndef LINEEDITWIDGET_H
#define LINEEDITWIDGET_H

#include <QWidget>
#include <QTextEdit>
#include <QPainter>
#include <QPaintEvent>
#include <QScrollBar>
#include <QFont>
#include <QTextBlock>
#include <QTextDocument>

class LineEditWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LineEditWidget(QTextEdit *editor, QWidget *parent = nullptr);
    void setFont(const QFont &font);
    void setEnabled(bool enabled);

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    QTextEdit *textEditor;
    QFont displayFont;
    bool isEnabled;

    void updateWidth();
    void updateLineNumbers();
};

#endif // LINEEDITWIDGET_H
