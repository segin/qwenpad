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

void SyntaxHighlighter::addJSONRules()
{
    QTextCharFormat keywordFormat = createKeywordFormat(Qt::darkBlue);
    QTextCharFormat punctuationFormat = createOperatorFormat(Qt::magenta);
    QTextCharFormat quotationFormat = createQuotationFormat(Qt::darkRed);
    QTextCharFormat numberFormat = createNumberFormat(Qt::darkMagenta);

    QString keywordStr = QString("\\b(?:true|false|null)\\b");
    QRegularExpression keywordPattern(keywordStr);
    HighlightingRule rule;
    rule.format = keywordFormat;
    rule.pattern = keywordPattern;
    rules.append(rule);

    QString braceStr = QString("[{}]");
    QRegularExpression bracePattern(braceStr);
    rule.format = punctuationFormat;
    rule.pattern = bracePattern;
    rules.append(rule);

    QString bracketStr = QString("[\\[\\]]");
    QRegularExpression bracketPattern(bracketStr);
    rule.format = punctuationFormat;
    rule.pattern = bracketPattern;
    rules.append(rule);

    QString colonStr = QString(":");
    QRegularExpression colonPattern(colonStr);
    rule.format = punctuationFormat;
    rule.pattern = colonPattern;
    rules.append(rule);

    QString commaStr = QString(",");
    QRegularExpression commaPattern(commaStr);
    rule.format = punctuationFormat;
    rule.pattern = commaPattern;
    rules.append(rule);

    QString stringStr = QString("\"(?:[^\"\\n]|\\\\.)*\"");
    QRegularExpression stringPattern(stringStr);
    rule.format = quotationFormat;
    rule.pattern = stringPattern;
    rules.append(rule);

    QString numberStr = QString("(?:-)?\\d+(?:\\.\\d+)?(?:[eE][+-]?\\d+)?");
    QRegularExpression numberPattern(numberStr);
    rule.format = numberFormat;
    rule.pattern = numberPattern;
    rules.append(rule);
}

void SyntaxHighlighter::addShellRules()
{
    QTextCharFormat keywordFormat = createKeywordFormat(Qt::darkBlue);
    QTextCharFormat singleLineCommentFormat = createSingleLineCommentFormat(Qt::gray);
    QTextCharFormat commentFormat = createCommentFormat(Qt::green);
    QTextCharFormat quotationFormat = createQuotationFormat(Qt::darkRed);
    QTextCharFormat numberFormat = createNumberFormat(Qt::darkMagenta);
    QTextCharFormat builtInFormat = createBuiltInFormat(Qt::darkCyan);

    QString keywordStr = QString("\\b(?:if|then|else|elif|fi|for|while|until|do|done|case|esac|return|in|export|function|shift|source|alias|unalias)\\b");
    QRegularExpression keywordPattern(keywordStr);
    HighlightingRule rule;
    rule.format = keywordFormat;
    rule.pattern = keywordPattern;
    rules.append(rule);

    QString commentStr = QString("#.*$");
    QRegularExpression commentPattern(commentStr);
    rule.format = singleLineCommentFormat;
    rule.pattern = commentPattern;
    rules.append(rule);

    QString heredocStr = QString("<<\\s*-?\\s*\\w+");
    QRegularExpression heredocPattern(heredocStr);
    rule.format = builtInFormat;
    rule.pattern = heredocPattern;
    rules.append(rule);

    QString doubleQuoteStr = QString("\"(?:[^\"\\n]|\\\\.)*\"");
    QRegularExpression doubleQuotationPattern(doubleQuoteStr);
    rule.format = quotationFormat;
    rule.pattern = doubleQuotationPattern;
    rules.append(rule);

    QString singleQuoteStr = QString("'[^']*'");
    QRegularExpression singleQuotationPattern(singleQuoteStr);
    rule.format = quotationFormat;
    rule.pattern = singleQuotationPattern;
    rules.append(rule);

    QString numberStr = QString("\\d+");
    QRegularExpression numberPattern(numberStr);
    rule.format = numberFormat;
    rule.pattern = numberPattern;
    rules.append(rule);
}

void SyntaxHighlighter::addXMLRules()
{
    QTextCharFormat keywordFormat = createKeywordFormat(Qt::darkBlue);
    QTextCharFormat commentFormat = createCommentFormat(Qt::green);
    QTextCharFormat quotationFormat = createQuotationFormat(Qt::darkRed);
    QTextCharFormat builtInFormat = createBuiltInFormat(Qt::darkCyan);
    QTextCharFormat punctuationFormat = createOperatorFormat(Qt::magenta);
    QTextCharFormat cdataFormat = createQuotationFormat(Qt::darkCyan);

    QString keywordStr = QString("\\b(?:html|head|body|div|span|p|a|h|ul|ol|li|table|tr|td|th|img|script|style|link|meta|form|input|button|label|select|option|textarea|nav|header|footer|section|article|aside)\\b");
    QRegularExpression keywordPattern(keywordStr);
    HighlightingRule rule;
    rule.format = keywordFormat;
    rule.pattern = keywordPattern;
    rules.append(rule);

    QString xmlCommentStr = QString("<!--[^>]*-->");
    QRegularExpression xmlCommentPattern(xmlCommentStr);
    rule.format = commentFormat;
    rule.pattern = xmlCommentPattern;
    rules.append(rule);

    QString cdataStartStr = QString("<!\\[CDATA\\[");
    QRegularExpression cdataStartPattern(cdataStartStr);
    rule.format = cdataFormat;
    rule.pattern = cdataStartPattern;
    rules.append(rule);

    QString cdataEndStr = QString("\\]\\]>");
    QRegularExpression cdataEndPattern(cdataEndStr);
    rule.format = cdataFormat;
    rule.pattern = cdataEndPattern;
    rules.append(rule);

    QString attributeStr = QString("\\b[a-zA-Z_][a-zA-Z0-9_-]*=");
    QRegularExpression attributePattern(attributeStr);
    rule.format = builtInFormat;
    rule.pattern = attributePattern;
    rules.append(rule);

    QString simpleQuoteStr = QString("\"[^\"]*\"");
    QRegularExpression simpleQuotationPattern(simpleQuoteStr);
    rule.format = quotationFormat;
    rule.pattern = simpleQuotationPattern;
    rules.append(rule);

    QString punctuationStr = QString("[<>]");
    QRegularExpression punctuationPattern(punctuationStr);
    rule.format = punctuationFormat;
    rule.pattern = punctuationPattern;
    rules.append(rule);
}

void SyntaxHighlighter::setLanguage(const QString &language)
{
    currentLanguage = language;
    rules.clear();

    QString lower = language.toLower();

    if (lower == "cpp" || lower == "c" || lower == "c++" || lower == "h" || lower == "hpp") {
        addCPPRules();
    } else if (lower == "python" || lower == "py") {
        addPythonRules();
    } else if (lower == "json") {
        addJSONRules();
    } else if (lower == "shell" || lower == "bash" || lower == "sh" || lower == "zsh") {
        addShellRules();
    } else if (lower == "xml" || lower == "html" || lower == "htm" || lower == "xhtml") {
        addXMLRules();
    } else {
        addTextRules();
    }

    rehighlight();
}

 QString SyntaxHighlighter::getLanguage() const
{
    return currentLanguage;
}

  void SyntaxHighlighter::highlightBlock(const QString &text)
{
    highlightedRegions.clear();
    if (currentLanguage == "C++" || currentLanguage == "C") {
        int state = currentBlockState();
        if (state < 0) state = -1;

        QTextCharFormat commentFormat = createCommentFormat(Qt::green);

        if (state == 1) {
            int endPos = text.indexOf("*/");
            if (endPos >= 0) {
                setFormat(0, endPos + 2, commentFormat);
                highlightedRegions.append(qMakePair(0, endPos + 2));
                state = 0;
                int offset = endPos + 2;
                int commentStart = text.indexOf("/*", offset);
                while (commentStart >= 0) {
                    int commentEnd = text.indexOf("*/", commentStart);
                    if (commentEnd < 0) {
                        setFormat(commentStart, text.length() - commentStart, commentFormat);
                        highlightedRegions.append(qMakePair(commentStart, text.length()));
                        state = 1;
                        break;
                    } else {
                        int regionEnd = commentEnd + 2;
                        setFormat(commentStart, regionEnd - commentStart, commentFormat);
                        highlightedRegions.append(qMakePair(commentStart, regionEnd));
                        offset = regionEnd;
                        commentStart = text.indexOf("/*", offset);
                    }
                }
            } else {
                setFormat(0, text.length(), commentFormat);
                highlightedRegions.append(qMakePair(0, text.length()));
            }
        }

        if (state == 0) {
            int commentStart = text.indexOf("/*");
            while (commentStart >= 0) {
                int commentEnd = text.indexOf("*/", commentStart);
                if (commentEnd < 0) {
                    setFormat(commentStart, text.length() - commentStart, commentFormat);
                    highlightedRegions.append(qMakePair(commentStart, text.length()));
                    state = 1;
                    break;
                } else {
                    int regionEnd = commentEnd + 2;
                    setFormat(commentStart, regionEnd - commentStart, commentFormat);
                    highlightedRegions.append(qMakePair(commentStart, regionEnd));
                    commentStart = text.indexOf("/*", regionEnd);
                }
            }
        }

        for (const HighlightingRule &rule : rules) {
            QRegularExpressionMatchIterator matchIter = rule.pattern.globalMatch(text);
            while (matchIter.hasNext()) {
                QRegularExpressionMatch match = matchIter.next();
                bool overlaps = false;
                for (const auto &region : highlightedRegions) {
                    int regionEnd = region.second;
                    if (match.capturedStart() < regionEnd && match.capturedEnd() > region.first) {
                        overlaps = true;
                        break;
                    }
                }
                if (!overlaps) {
                    setFormat(match.capturedStart(), match.capturedLength(), rule.format);
                }
            }
        }

        setCurrentBlockState(state);
    } else if (currentLanguage == "Shell" || currentLanguage == "bash" || currentLanguage == "sh" || currentLanguage == "zsh") {
        QTextBlock previousBlock = currentBlock().previous();
        QString heredocMarker;
        bool inHeredoc = false;

        if (previousBlock.isValid()) {
            auto userData = previousBlock.userData();
            if (userData) {
                auto shellData = static_cast<ShellHeredocData *>(userData);
                if (!shellData->marker.isEmpty()) {
                    inHeredoc = true;
                    heredocMarker = shellData->marker;
                }
            }
        }

        QRegularExpression heredocPattern("<<\\s*-?\\s*(\\w+)");
        QRegularExpression heredocEndPattern("^\\s*(\\w+)\\s*$");

        if (inHeredoc) {
            QTextCharFormat heredocFormat = createCommentFormat(Qt::green);
            setFormat(0, text.length(), heredocFormat);
            highlightedRegions.append(qMakePair(0, text.length()));

            if (heredocEndPattern.match(text).hasMatch()) {
                QRegularExpressionMatch match = heredocEndPattern.match(text);
                QString endMarker = match.captured(1);
                if (endMarker == heredocMarker) {
                    if (previousBlock.isValid()) {
                        auto userData = previousBlock.userData();
                        if (userData) {
                            static_cast<ShellHeredocData *>(userData)->marker = "";
                        }
                    }
                }
            } else {
                auto userData = currentBlock().userData();
                if (userData) {
                    static_cast<ShellHeredocData *>(userData)->marker = heredocMarker;
                } else {
                    auto newData = new ShellHeredocData;
                    newData->marker = heredocMarker;
                    currentBlock().setUserData(newData);
                }
            }
        } else {
            QRegularExpressionMatchIterator it = heredocPattern.globalMatch(text);
            while (it.hasNext()) {
                QRegularExpressionMatch match = it.next();
                QString marker = match.captured(1);
                QRegularExpression endCheckPattern("^\\s*" + QRegularExpression::escape(marker) + "\\s*$");
                if (!endCheckPattern.match(text).hasMatch()) {
                    auto userData = currentBlock().userData();
                    if (userData) {
                        static_cast<ShellHeredocData *>(userData)->marker = marker;
                    } else {
                        auto newData = new ShellHeredocData;
                        newData->marker = marker;
                        currentBlock().setUserData(newData);
                    }
                    break;
                }
            }
        }

        for (const HighlightingRule &rule : rules) {
            QRegularExpressionMatchIterator iter = rule.pattern.globalMatch(text);
            while (iter.hasNext()) {
                QRegularExpressionMatch match = iter.next();
                bool overlaps = false;
                for (const auto &region : highlightedRegions) {
                    int regionEnd = region.second;
                    if (match.capturedStart() < regionEnd && match.capturedEnd() > region.first) {
                        overlaps = true;
                        break;
                    }
                }
                if (!overlaps) {
                    setFormat(match.capturedStart(), match.capturedLength(), rule.format);
                }
            }
        }

        setCurrentBlockState(inHeredoc ? 1 : 0);
    } else if (currentLanguage == "XML" || currentLanguage == "HTML" || currentLanguage == "XHTML") {
        int state = currentBlockState();
        if (state < 0) state = -1;

        QTextCharFormat commentFormat = createCommentFormat(Qt::green);
        QTextCharFormat cdataFormat = createQuotationFormat(Qt::darkCyan);

        if (state == 1) {
            int endPos = text.indexOf("-->");
            if (endPos >= 0) {
                setFormat(0, endPos + 3, commentFormat);
                highlightedRegions.append(qMakePair(0, endPos + 3));
                state = 0;
                int offset = endPos + 3;
                int commentStart = text.indexOf("<!--", offset);
                while (commentStart >= 0) {
                    int commentEnd = text.indexOf("-->", commentStart);
                    if (commentEnd < 0) {
                        setFormat(commentStart, text.length() - commentStart, commentFormat);
                        highlightedRegions.append(qMakePair(commentStart, text.length()));
                        state = 1;
                        break;
                    } else {
                        int regionEnd = commentEnd + 3;
                        setFormat(commentStart, regionEnd - commentStart, commentFormat);
                        highlightedRegions.append(qMakePair(commentStart, regionEnd));
                        offset = regionEnd;
                        commentStart = text.indexOf("<!--", offset);
                    }
                }
            } else {
                setFormat(0, text.length(), commentFormat);
                highlightedRegions.append(qMakePair(0, text.length()));
            }
        }

        if (state == 2) {
            int endPos = text.indexOf("]]>");
            if (endPos >= 0) {
                setFormat(0, endPos + 3, cdataFormat);
                highlightedRegions.append(qMakePair(0, endPos + 3));
                state = 0;
                int offset = endPos + 3;
                int cdataStart = text.indexOf("<![CDATA[", offset);
                while (cdataStart >= 0) {
                    int cdataEnd = text.indexOf("]]>", cdataStart);
                    if (cdataEnd < 0) {
                        setFormat(cdataStart, text.length() - cdataStart, cdataFormat);
                        highlightedRegions.append(qMakePair(cdataStart, text.length()));
                        state = 2;
                        break;
                    } else {
                        int regionEnd = cdataEnd + 3;
                        setFormat(cdataStart, regionEnd - cdataStart, cdataFormat);
                        highlightedRegions.append(qMakePair(cdataStart, regionEnd));
                        offset = regionEnd;
                        cdataStart = text.indexOf("<![CDATA[", offset);
                    }
                }
            } else {
                setFormat(0, text.length(), cdataFormat);
                highlightedRegions.append(qMakePair(0, text.length()));
            }
        }

        if (state == 0) {
            int commentStart = text.indexOf("<!--");
            while (commentStart >= 0) {
                int commentEnd = text.indexOf("-->", commentStart);
                if (commentEnd < 0) {
                    setFormat(commentStart, text.length() - commentStart, commentFormat);
                    highlightedRegions.append(qMakePair(commentStart, text.length()));
                    state = 1;
                    break;
                } else {
                    int regionEnd = commentEnd + 3;
                    setFormat(commentStart, regionEnd - commentStart, commentFormat);
                    highlightedRegions.append(qMakePair(commentStart, regionEnd));
                    commentStart = text.indexOf("<!--", regionEnd);
                }
            }

            int cdataStartPos = text.indexOf("<![CDATA[");
            while (cdataStartPos >= 0) {
                int cdataEnd = text.indexOf("]]>", cdataStartPos);
                if (cdataEnd < 0) {
                    setFormat(cdataStartPos, text.length() - cdataStartPos, cdataFormat);
                    highlightedRegions.append(qMakePair(cdataStartPos, text.length()));
                    state = 2;
                    break;
                } else {
                    int regionEnd = cdataEnd + 3;
                    setFormat(cdataStartPos, regionEnd - cdataStartPos, cdataFormat);
                    highlightedRegions.append(qMakePair(cdataStartPos, regionEnd));
                    cdataStartPos = text.indexOf("<![CDATA[", regionEnd);
                }
            }
        }

        for (const HighlightingRule &rule : rules) {
            QRegularExpressionMatchIterator iter = rule.pattern.globalMatch(text);
            while (iter.hasNext()) {
                QRegularExpressionMatch match = iter.next();
                bool overlaps = false;
                for (const auto &region : highlightedRegions) {
                    int regionEnd = region.second;
                    if (match.capturedStart() < regionEnd && match.capturedEnd() > region.first) {
                        overlaps = true;
                        break;
                    }
                }
                if (!overlaps) {
                    setFormat(match.capturedStart(), match.capturedLength(), rule.format);
                }
            }
        }

        setCurrentBlockState(state);
    } else {
        for (const HighlightingRule &rule : rules) {
            QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
            while (matchIterator.hasNext()) {
                QRegularExpressionMatch match = matchIterator.next();
                bool overlaps = false;
                for (const auto &region : highlightedRegions) {
                    int regionEnd = region.second;
                    if (match.capturedStart() < regionEnd && match.capturedEnd() > region.first) {
                        overlaps = true;
                        break;
                    }
                }
                if (!overlaps) {
                    setFormat(match.capturedStart(), match.capturedLength(), rule.format);
                }
            }
        }
        setCurrentBlockState(-1);
    }
}
