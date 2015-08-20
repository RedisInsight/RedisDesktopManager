#include "test_command.h"
#include "redisclient/command.h"
#include "redisclient/scancommand.h"

#include <QDebug>
#include <QTest>

void TestCommand::prepareCommand()
{
	//given 
    RedisClient::Command cmd({QString("EXISTS"), "testkey:test"});

	//when
    QByteArray actualResult = cmd.getByteRepresentation();

	//then
    QCOMPARE(actualResult, QByteArray("*2\r\n$6\r\nEXISTS\r\n$12\r\ntestkey:test\r\n"));
}

void TestCommand::parseCommandString()
{
    //given
    QFETCH(QString, data);

    //when
    QString actualResult = RedisClient::Command::splitCommandString(data).join("::");

    //then
    QFETCH(QString, validResult);
    QCOMPARE(actualResult, validResult);
}

void TestCommand::parseCommandString_data()
{
    QTest::addColumn<QString>("data");
    QTest::addColumn<QString>("validResult");
    QTest::newRow("Valid one delimited") << "test \"123\"" << "test::123";
    QTest::newRow("Valid one delimited") << "test '123'" << "test::123";
    QTest::newRow("Valid one raw") << "test 123" << "test::123";
    QTest::newRow("Valid two delimited") << "test \"123\" \"234\"" << "test::123::234";
    QTest::newRow("Valid mixed") << "test \"123\" 234" << "test::123::234";
    QTest::newRow("Valid delimited & escaped") << "test \"10\" \"car\\" "\"s\"" << "test::10::car\"s";
}

void TestCommand::isSelectCommand()
{
    //given
    RedisClient::Command cmd(QStringList{"SELECT","0"});

    //when
    bool actualResult = cmd.isSelectCommand();

    //then
    QCOMPARE(actualResult, true);
}

void TestCommand::scanCommandSetCursor()
{
    //given
    QFETCH(QStringList, rawCommandString);
    QFETCH(int, cursor);
    QFETCH(int, index);
    RedisClient::ScanCommand cmd(rawCommandString);

    //when
    cmd.setCursor(cursor);
    QString actualResult = cmd.getPartAsString(index);

    //then
    QCOMPARE(actualResult, QString::number(cursor));
}

void TestCommand::scanCommandSetCursor_data()
{
    QTest::addColumn<QStringList>("rawCommandString");
    QTest::addColumn<int>("cursor");
    QTest::addColumn<int>("index");
    QTest::newRow("Valid scan") << QStringList{"scan", "0"} << 1 << 1;
    QTest::newRow("Valid sscan") << QStringList{"sscan", "set", "0"} << 1 << 2;
    QTest::newRow("Valid hscan") << QStringList{"hscan", "set", "0"} << 1 << 2;
    QTest::newRow("Valid zscan") << QStringList{"zscan", "set", "0"} << 1 << 2;
}

void TestCommand::scanCommandIsValid()
{
    //given
    QFETCH(QStringList, rawCommandString);
    QFETCH(bool, expected);
    RedisClient::ScanCommand cmd(rawCommandString);

    bool actualResult = cmd.isValidScanCommand();

    QCOMPARE(actualResult, expected);
}

void TestCommand::scanCommandIsValid_data()
{
    QTest::addColumn<QStringList>("rawCommandString");
    QTest::addColumn<bool>("expected");

    QTest::newRow("Valid scan") << QStringList{"scan", "0"} << true;
    QTest::newRow("Invalid scan") << QStringList{"set", "0"} << false;
    QTest::newRow("Valid value scan") << QStringList{"sscan", "set", "0"} << true;
    QTest::newRow("Invalid value scan") << QStringList{"set", "test", "0"} << false;
}

