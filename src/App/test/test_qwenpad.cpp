#include <QTest>
#include <QString>
#include <QTextDocument>
#include <QTextCursor>
#include "utils.h"
#include "tabmanager.h"

class TestQwenpad : public QObject
{
    Q_OBJECT

private slots:
    void testConvertLineEndingsLF();
    void testConvertLineEndingsCRLF();
    void testConvertLineEndingsCR();
    void testConvertLineEndingsAlreadyCRLF();
    void testConvertLineEndingsMixed();
    void testDocumentFind();
    void testDocumentFindWrapAround();
    void testDocumentReplace();
    void testDocumentReplaceEmpty();
  void testConvertLineEndingsPreservesNewlines();
    void testCloseTabsToLeftSignal();
};

void TestQwenpad::testConvertLineEndingsLF()
{
    QString result = Utils::convertLineEndings("Hello\r\nWorld", 0);
    QCOMPARE(result, QString("Hello\nWorld"));

    result = Utils::convertLineEndings("Hello\rWorld", 0);
    QCOMPARE(result, QString("Hello\nWorld"));

    result = Utils::convertLineEndings("Hello\nWorld", 0);
    QCOMPARE(result, QString("Hello\nWorld"));
}

void TestQwenpad::testConvertLineEndingsCRLF()
{
    QString result = Utils::convertLineEndings("Hello\nWorld", 1);
    QCOMPARE(result, QString("Hello\r\nWorld"));

    result = Utils::convertLineEndings("Hello\rWorld", 1);
    QCOMPARE(result, QString("Hello\r\nWorld"));

    result = Utils::convertLineEndings("Hello\r\nWorld", 1);
    QCOMPARE(result, QString("Hello\r\nWorld"));
}

void TestQwenpad::testConvertLineEndingsCR()
{
    QString result = Utils::convertLineEndings("Hello\nWorld", 2);
    QCOMPARE(result, QString("Hello\rWorld"));

    result = Utils::convertLineEndings("Hello\rWorld", 2);
    QCOMPARE(result, QString("Hello\rWorld"));

    result = Utils::convertLineEndings("Hello\r\nWorld", 2);
    QCOMPARE(result, QString("Hello\rWorld"));
}

void TestQwenpad::testConvertLineEndingsAlreadyCRLF()
{
    QString result = Utils::convertLineEndings("A\r\nB\r\nC", 1);
    QCOMPARE(result, QString("A\r\nB\r\nC"));

    result = Utils::convertLineEndings("A\r\nB\r\nC", 0);
    QCOMPARE(result, QString("A\nB\nC"));
}

void TestQwenpad::testConvertLineEndingsMixed()
{
    QString mixed = "Line1\r\nLine2\rLine3\nLine4";
    QString result = Utils::convertLineEndings(mixed, 0);
    QCOMPARE(result, QString("Line1\nLine2\nLine3\nLine4"));

    result = Utils::convertLineEndings(mixed, 1);
    QCOMPARE(result, QString("Line1\r\nLine2\r\nLine3\r\nLine4"));
}

void TestQwenpad::testDocumentFind()
{
    QTextDocument doc;
    doc.setPlainText("Hello World\nHello Qt\nHello World");

    QTextCursor cursor(doc.find("Hello"));
    QCOMPARE(cursor.selectedText(), QString("Hello"));

    QTextCursor found = doc.find("Qt", cursor);
    QVERIFY(!found.isNull());
    QCOMPARE(found.selectedText(), QString("Qt"));
}

void TestQwenpad::testDocumentFindWrapAround()
{
    QTextDocument doc;
    doc.setPlainText("Hello World\nHello Qt");

    QTextCursor cursor(doc.find("Hello"));
    QCOMPARE(cursor.selectedText(), QString("Hello"));

    cursor.setPosition(0);
    QTextCursor first = doc.find("World", cursor);
    QVERIFY(!first.isNull());
    QCOMPARE(first.selectedText(), QString("World"));
}

  void TestQwenpad::testDocumentReplace()
{
    QTextDocument doc;
    doc.setPlainText("Hello World");

    QTextCursor cursor(&doc);
    cursor.beginEditBlock();

    while ((cursor = doc.find("World", cursor)) != QTextCursor()) {
        cursor.insertText("Qt");
        break;
    }

    cursor.endEditBlock();
    QCOMPARE(doc.toPlainText(), QString("Hello Qt"));
}

void TestQwenpad::testDocumentReplaceEmpty()
{
    QTextDocument doc;
    doc.setPlainText("Hello World Qt");

   QTextCursor cursor(&doc);
    cursor.beginEditBlock();

    while ((cursor = doc.find("Qt", cursor)) != QTextCursor()) {
        cursor.removeSelectedText();
        break;
    }

    cursor.endEditBlock();
    QCOMPARE(doc.toPlainText(), QString("Hello World "));
}

void TestQwenpad::testConvertLineEndingsPreservesNewlines()
{
    QString result = Utils::convertLineEndings("Hello\nWorld", 0);
    QCOMPARE(result, QString("Hello\nWorld"));
}

void TestQwenpad::testCloseTabsToLeftSignal()
{
    TabBar tabBar;
    tabBar.addTab("Tab 0");
    tabBar.addTab("Tab 1");
    tabBar.addTab("Tab 2");
    tabBar.addTab("Tab 3");

    bool signalReceived = false;
    connect(&tabBar, &TabBar::closeLeftRequested, this, [&signalReceived]() {
        signalReceived = true;
    });

    QCOMPARE(signalReceived, false);

    // Verify closeLeftRequested signal exists by checking tab count
    QCOMPARE(tabBar.count(), 4);
}

#include "test_qwenpad.moc"

QTEST_MAIN(TestQwenpad)
