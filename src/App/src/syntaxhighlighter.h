#ifndef SYNTAXHIGHLIGHTER_H
#define SYNTAXHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QTextDocument>
#include <QHash>
#include <QColor>
#include <QTextCharFormat>
#include <QRegularExpression>

class SyntaxHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    explicit SyntaxHighlighter(QTextDocument *parent = nullptr);
    void setLanguage(const QString &language);

private:
    void highlightBlock(const QString &text) override;

    struct HighlightingRule
    {
        QTextCharFormat format;
        QRegularExpression pattern;
    };

    QVector<HighlightingRule> rules;
    QHash<QString, QVector<HighlightingRule>> languageRules;
    void addCPPRules();
    void addPythonRules();
    void addTextRules();

    QTextCharFormat createKeywordFormat(const QColor &color);
    QTextCharFormat createClassFormat(const QColor &color);
    QTextCharFormat createDefaultFormat(const QColor &color);
    QTextCharFormat createCommentFormat(const QColor &color);
    QTextCharFormat createSingleLineCommentFormat(const QColor &color);
    QTextCharFormat createQuotationFormat(const QColor &color);
    QTextCharFormat createBuiltInFormat(const QColor &color);
    QTextCharFormat createOperatorFormat(const QColor &color);
    QTextCharFormat createHexFormat(const QColor &color);
    QTextCharFormat createNumberFormat(const QColor &color);
};

#endif // SYNTAXHIGHLIGHTER_H
