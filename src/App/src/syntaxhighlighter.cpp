#include "syntaxhighlighter.h"
#include <QRegularExpression>
#include <QTextCharFormat>
#include <QColor>

SyntaxHighlighter::SyntaxHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    addTextRules();
}

QTextCharFormat SyntaxHighlighter::createKeywordFormat(const QColor &color)
{
    QTextCharFormat format;
    format.setForeground(color);
    format.setFontWeight(QFont::Bold);
    return format;
}

QTextCharFormat SyntaxHighlighter::createClassFormat(const QColor &color)
{
    QTextCharFormat format;
    format.setForeground(color);
    format.setFontWeight(QFont::Bold);
    format.setFontUnderline(true);
    return format;
}

QTextCharFormat SyntaxHighlighter::createDefaultFormat(const QColor &color)
{
    QTextCharFormat format;
    format.setForeground(color);
    return format;
}

QTextCharFormat SyntaxHighlighter::createCommentFormat(const QColor &color)
{
    QTextCharFormat format;
    format.setForeground(color);
    format.setFontItalic(true);
    return format;
}

QTextCharFormat SyntaxHighlighter::createSingleLineCommentFormat(const QColor &color)
{
    QTextCharFormat format;
    format.setForeground(color);
    format.setFontItalic(true);
    return format;
}

QTextCharFormat SyntaxHighlighter::createQuotationFormat(const QColor &color)
{
    QTextCharFormat format;
    format.setForeground(color);
    return format;
}

QTextCharFormat SyntaxHighlighter::createBuiltInFormat(const QColor &color)
{
    QTextCharFormat format;
    format.setForeground(color);
    format.setFontWeight(QFont::Bold);
    return format;
}

QTextCharFormat SyntaxHighlighter::createOperatorFormat(const QColor &color)
{
    QTextCharFormat format;
    format.setForeground(color);
    return format;
}

QTextCharFormat SyntaxHighlighter::createHexFormat(const QColor &color)
{
    QTextCharFormat format;
    format.setForeground(color);
    return format;
}

QTextCharFormat SyntaxHighlighter::createNumberFormat(const QColor &color)
{
    QTextCharFormat format;
    format.setForeground(color);
    return format;
}

void SyntaxHighlighter::addTextRules()
{
    QTextCharFormat defaultFormat;
    defaultFormat.setForeground(Qt::black);

    HighlightingRule rule;
    rule.format = defaultFormat;
    rule.pattern = QRegularExpression(".");
    rules.append(rule);
}

void SyntaxHighlighter::addCPPRules()
{
    QTextCharFormat keywordFormat = createKeywordFormat(Qt::darkBlue);
    QTextCharFormat classFormat = createClassFormat(Qt::magenta);
    QTextCharFormat commentFormat = createCommentFormat(Qt::green);
    QTextCharFormat singleLineCommentFormat = createSingleLineCommentFormat(Qt::gray);
    QTextCharFormat quotationFormat = createQuotationFormat(Qt::darkRed);
    QTextCharFormat builtInFormat = createBuiltInFormat(Qt::darkCyan);
    QTextCharFormat operatorFormat = createOperatorFormat(Qt::magenta);
    QTextCharFormat numberFormat = createNumberFormat(Qt::darkMagenta);

    QRegularExpression keywordPattern("\\b(?:for|while|if|else|switch|case|return|break|continue|do|default|goto|class|struct|namespace|using|typedef|template|typename|virtual|override|friend|public|protected|private|const|volatile|mutable|explicit|inline|static|extern|register|operator|this|new|delete|throw|try|catch|dynamic_cast|static_cast|reinterpret_cast|const_cast|sizeof|typeid|auto|register|bool|signed|unsigned|char|short|long|int|float|double|void|true|false|nullptr|constexpr)\\b");
    HighlightingRule rule;
    rule.format = keywordFormat;
    rule.pattern = keywordPattern;
    rules.append(rule);

    QRegularExpression classPattern("\\b(?:Q_\\w+|int8_t|uint8_t|int16_t|uint16_t|int32_t|uint32_t|int64_t|uint64_t|qint8|quint8|qint16|quint16|qint32|quint32|qint64|quint64|qlonglong|qulonglong|qintptr|quintptr|qreal)\\b");
    rule.format = builtInFormat;
    rule.pattern = classPattern;
    rules.append(rule);

    QRegularExpression commentPattern("//.*");
    rule.format = singleLineCommentFormat;
    rule.pattern = commentPattern;
    rules.append(rule);

    QRegularExpression multiLineCommentStart("/\\*");
    rule.format = commentFormat;
    rule.pattern = multiLineCommentStart;
    rules.append(rule);

    QRegularExpression quotationPattern("\"\"\"");
    rule.format = quotationFormat;
    rule.pattern = quotationPattern;
    rules.append(rule);

    QRegularExpression simpleQuotationPattern("\"(?:[^\"\\n]|\\\\.)*\"");
    rule.format = quotationFormat;
    rule.pattern = simpleQuotationPattern;
    rules.append(rule);

    QRegularExpression numberPattern("[+-]?(?:0x[0-9A-Fa-f]+|(?:0|[1-9][0-9]*)(?:\\.[0-9]+)?(?:[eE][+-]?[0-9]+)?[fFlL]?)");
    rule.format = numberFormat;
    rule.pattern = numberPattern;
    rules.append(rule);

    QRegularExpression hexPattern("\\b0x[0-9A-Fa-f]+(ul|UL)?\\b");
    rule.format = numberFormat;
    rule.pattern = hexPattern;
    rules.append(rule);
}

void SyntaxHighlighter::addPythonRules()
{
    QTextCharFormat keywordFormat = createKeywordFormat(Qt::darkBlue);
    QTextCharFormat classFormat = createClassFormat(Qt::magenta);
    QTextCharFormat commentFormat = createCommentFormat(Qt::green);
    QTextCharFormat singleLineCommentFormat = createSingleLineCommentFormat(Qt::gray);
    QTextCharFormat quotationFormat = createQuotationFormat(Qt::darkRed);
    QTextCharFormat builtInFormat = createBuiltInFormat(Qt::darkCyan);
    QTextCharFormat operatorFormat = createOperatorFormat(Qt::magenta);
    QTextCharFormat numberFormat = createNumberFormat(Qt::darkMagenta);

    QRegularExpression keywordPattern("\\b(?:and|as|assert|async|await|break|case|class|continue|def|del|elif|else|except|exec|finally|for|from|global|goto|if|in|import|lambda|match|nonlocal|not|or|pass|raise|return|try|while|with|yield)\\b");
    HighlightingRule rule;
    rule.format = keywordFormat;
    rule.pattern = keywordPattern;
    rules.append(rule);

    QRegularExpression classPattern("\\b(?:Q_\\w+|int8_t|uint8_t|int16_t|uint16_t|int32_t|uint32_t|int64_t|uint64_t|qint8|quint8|qint16|quint16|qint32|quint32|qint64|quint64|qlonglong|qulonglong|qintptr|quintptr|qreal)\\b");
    rule.format = builtInFormat;
    rule.pattern = classPattern;
    rules.append(rule);

    QRegularExpression commentPattern("#.*$");
    rule.format = singleLineCommentFormat;
    rule.pattern = commentPattern;
    rules.append(rule);

    QRegularExpression tripleQuotationPattern("(?:\"\"\"|''')");
    rule.format = quotationFormat;
    rule.pattern = tripleQuotationPattern;
    rules.append(rule);

    QRegularExpression simpleQuotationPattern("\"(?:[^\"\\n]|\\\\.)*\"");
    rule.format = quotationFormat;
    rule.pattern = simpleQuotationPattern;
    rules.append(rule);

    QRegularExpression numberPattern("[+-]?(?:0x[0-9A-Fa-f]+|(?:0|[1-9][0-9]*)(?:\\.[0-9]+)?(?:[eE][+-]?[0-9]+)?[fFlL]?)");
    rule.format = numberFormat;
    rule.pattern = numberPattern;
    rules.append(rule);

    QRegularExpression hexPattern("\\b0x[0-9A-Fa-f]+(ul|UL)?\\b");
    rule.format = numberFormat;
    rule.pattern = hexPattern;
    rules.append(rule);
}

void SyntaxHighlighter::setLanguage(const QString &language)
{
    rules.clear();
    QString lower = language.toLower();

    if (lower == "cpp" || lower == "c" || lower == "c++" || lower == "h" || lower == "hpp") {
        addCPPRules();
    } else if (lower == "python" || lower == "py") {
        addPythonRules();
    } else {
        addTextRules();
    }

    rehighlight();
}

void SyntaxHighlighter::highlightBlock(const QString &text)
{
    for (const HighlightingRule &rule : rules) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
}
