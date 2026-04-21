#include "lineeditwidget.h"
#include <QPainter>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QScrollBar>
#include <QFontMetrics>
#include <QTextDocument>
#include <QAbstractTextDocumentLayout>

LineEditWidget::LineEditWidget(QTextEdit *editor, QWidget *parent)
    : QWidget(parent)
    , textEditor(editor)
    , isEnabled(false)
{
    connect(editor->verticalScrollBar(), static_cast<void (QAbstractSlider::*)(int)>(&QAbstractSlider::valueChanged), this, [this](int) { updateLineNumbers(); });
    connect(editor->document(), &QTextDocument::contentsChanged, this, &LineEditWidget::updateLineNumbers);
    connect(editor->document(), &QTextDocument::blockCountChanged, this, &LineEditWidget::updateWidth);
}

void LineEditWidget::setFont(const QFont &font)
{
    QFont smallFont = font;
    smallFont.setPointSize(smallFont.pointSize() - 1);
    displayFont = smallFont;
    update();
}

void LineEditWidget::setEnabled(bool enabled)
{
    isEnabled = enabled;
    update();
}

void LineEditWidget::updateWidth()
{
    if (!isEnabled) {
        setFixedWidth(0);
        return;
    }

    QTextDocument *doc = textEditor->document();
    int blockCount = doc->blockCount();
    QFontMetrics fm(displayFont);
    int digits = QString::number(blockCount).length();
    int width = (digits + 2) * fm.horizontalAdvance('0') + 5;
    setFixedWidth(width);
}

void LineEditWidget::updateLineNumbers()
{
    update();
}

void LineEditWidget::paintEvent(QPaintEvent *event)
{
    if (!isEnabled) {
        QWidget::paintEvent(event);
        return;
    }

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.fillRect(event->rect(), QColor("#f0f0f0"));

    painter.setPen(QColor("#666666"));
    painter.setFont(displayFont);

    QTextDocument *doc = textEditor->document();
    QAbstractTextDocumentLayout *layout = doc->documentLayout();
    QTextBlock block = doc->firstBlock();
    QRect clip = event->rect();
    int viewportY = textEditor->verticalScrollBar()->value();

    while (block.isValid()) {
        QRectF blockRect = layout->blockBoundingRect(block);
        int pixelY = static_cast<int>(blockRect.top()) - viewportY;

        if (pixelY > clip.bottom()) {
            break;
        }

        if ((pixelY + static_cast<int>(blockRect.height())) < clip.top()) {
            block = block.next();
            continue;
        }

        QRect textRect(clip.x(), pixelY, clip.width(), static_cast<int>(blockRect.height()));
        painter.drawText(textRect, Qt::AlignRight | Qt::AlignTop, QString::number(block.blockNumber() + 1));

        block = block.next();
    }
}

void LineEditWidget::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    updateWidth();
}
