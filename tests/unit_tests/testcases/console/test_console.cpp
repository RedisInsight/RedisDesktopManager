#include "test_console.h"
#include "console/hex_utils.h"
#include <QTest>

TestConsole::TestConsole(QObject *parent) :
    QObject(parent)
{
}

void TestConsole::testHexUtils()
{
    // given
    QByteArray test("123");
    test.append('\0');
    test.append("\u2605");
    test.append("456");

    // when
    QString actualResult = binaryStringToEscapedString(test);
    QByteArray actualResult2 = escapedStringToBinaryString(actualResult);

    // then
    QCOMPARE(actualResult, QString("123\\x00\u2605456"));
    QCOMPARE(actualResult2, test);
}
