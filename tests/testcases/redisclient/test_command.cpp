#include "test_command.h"
#include "redisclient/command.h"

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

