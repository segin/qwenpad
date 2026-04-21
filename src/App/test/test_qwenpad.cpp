#include <QTest>
#include <QString>
#include "qwenpad.h"

class TestQwenpad : public QObject
{
    Q_OBJECT

private slots:
    void testConvertLineEndingsLF();
    void testConvertLineEndingsCRLF();
    void testConvertLineEndingsCR();
    void testConvertLineEndingsAlreadyCRLF();
    void testConvertLineEndingsMixed();
};

void TestQwenpad::testConvertLineEndingsLF()
{
    Qwenpad app;
    QString result = app.convertLineEndings("Hello\r\nWorld", 0);
    QCOMPARE(result, QString("Hello\nWorld"));

    result = app.convertLineEndings("Hello\rWorld", 0);
    QCOMPARE(result, QString("Hello\nWorld"));

    result = app.convertLineEndings("Hello\nWorld", 0);
    QCOMPARE(result, QString("Hello\nWorld"));
}

void TestQwenpad::testConvertLineEndingsCRLF()
{
    Qwenpad app;
    QString result = app.convertLineEndings("Hello\nWorld", 1);
    QCOMPARE(result, QString("Hello\r\nWorld"));

    result = app.convertLineEndings("Hello\rWorld", 1);
    QCOMPARE(result, QString("Hello\r\nWorld"));

    result = app.convertLineEndings("Hello\r\nWorld", 1);
    QCOMPARE(result, QString("Hello\r\nWorld"));
}

void TestQwenpad::testConvertLineEndingsCR()
{
    Qwenpad app;
    QString result = app.convertLineEndings("Hello\nWorld", 2);
    QCOMPARE(result, QString("Hello\rWorld"));

    result = app.convertLineEndings("Hello\rWorld", 2);
    QCOMPARE(result, QString("Hello\rWorld"));

    result = app.convertLineEndings("Hello\r\nWorld", 2);
    QCOMPARE(result, QString("Hello\rWorld"));
}

void TestQwenpad::testConvertLineEndingsAlreadyCRLF()
{
    Qwenpad app;
    QString result = app.convertLineEndings("A\r\nB\r\nC", 1);
    QCOMPARE(result, QString("A\r\nB\r\nC"));

    result = app.convertLineEndings("A\r\nB\r\nC", 0);
    QCOMPARE(result, QString("A\nB\nC"));
}

void TestQwenpad::testConvertLineEndingsMixed()
{
    Qwenpad app;
    QString mixed = "Line1\r\nLine2\rLine3\nLine4";
    QString result = app.convertLineEndings(mixed, 0);
    QCOMPARE(result, QString("Line1\nLine2\nLine3\nLine4"));

    result = app.convertLineEndings(mixed, 1);
    QCOMPARE(result, QString("Line1\r\nLine2\r\nLine3\r\nLine4"));
}

#include "test_qwenpad.moc"

QTEST_MAIN(TestQwenpad)
