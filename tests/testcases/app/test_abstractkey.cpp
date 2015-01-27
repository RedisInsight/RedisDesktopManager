#include <QtCore>
#include <QTest>

#include "test_abstractkey.h"
#include "app/models/key-models/abstractkey.h"

void TestAbstractKey::testValueToEscapedString()
{
    // given
    QByteArray test("123");
    test.append('\0');
    test.append("\u2605");
    test.append("456");

    // when
    QVariant actualResult = valueToEscapedString(test);

    // then
    QCOMPARE(actualResult.toString(), QString("123\\x00\u2605456"));
}

void TestAbstractKey::testEscapedStringToValue()
{
    // given
    QString test("123\\x00\u2605456");

    // when
    QByteArray actualResult = escapedStringToValue(test);

    // then
    QCOMPARE(actualResult.size(), 10);
}
