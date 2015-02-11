#include "test_command.h"
#include "redisclient/command.h"
#include "redisclient/scancommand.h"

#include <QDebug>
#include <QTest>

void TestCommand::prepareCommand()
{
	//given 
    RedisClient::Command cmd("EXISTS testkey:test");

	//when
    QByteArray actualResult = cmd.getByteRepresentation();

	//then
    QCOMPARE(actualResult, QByteArray("*2\r\n$6\r\nEXISTS\r\n$12\r\ntestkey:test\r\n"));
}

void TestCommand::parseCommandString()
{
    //given
    QFETCH(QString, data);
    RedisClient::Command cmd(data);

    //when
    QString actualResult = cmd.getSplitedRepresentattion().join("::");

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
    RedisClient::Command cmd("SELECT 0");

    //when
    bool actualResult = cmd.isSelectCommand();

    //then
    QCOMPARE(actualResult, true);
}

void TestCommand::scanCommandSetCursor()
{
    //given
    QFETCH(QString, rawCommandString);
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
    QTest::addColumn<QString>("rawCommandString");
    QTest::addColumn<int>("cursor");
    QTest::addColumn<int>("index");
    QTest::newRow("Valid scan") << "scan 0" << 1 << 1;
    QTest::newRow("Valid sscan") << "sscan set 0" << 1 << 2;
    QTest::newRow("Valid hscan") << "hscan set 0" << 1 << 2;
    QTest::newRow("Valid zscan") << "zscan set 0" << 1 << 2;
}

void TestCommand::scanCommandIsValid()
{
    //given
    QFETCH(QString, rawCommandString);
    QFETCH(bool, expected);
    RedisClient::Command cmd(rawCommandString);

    bool actualResult = RedisClient::ScanCommand::isValidScanCommand(cmd);

    QCOMPARE(actualResult, expected);
}

void TestCommand::scanCommandIsValid_data()
{
    QTest::addColumn<QString>("rawCommandString");
    QTest::addColumn<bool>("expected");

    QTest::newRow("Valid scan") << "scan 0" << true;
    QTest::newRow("Invalid scan") << "set 0" << false;
    QTest::newRow("Valid value scan") << "sscan set 0" << true;
    QTest::newRow("Invalid value scan") << "set test 0" << false;
}

