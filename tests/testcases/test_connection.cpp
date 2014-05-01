#include <QTest>
#include "connection.h"
#include "commandexecutor.h"
#include "test_connection.h"
#include "abstractprotocol.h"

void TestConnection::connectToHostAndRunCommand()
{
    using namespace RedisClient;

    //given
    RedisConnectionConfig config("127.0.0.1");
    Connection connection(config, true);
    Command cmd("ping");

    //when
    //sync execution
    Response actualResult = CommandExecutor::execute(&connection, cmd);

    //then
    QCOMPARE(actualResult.toString(), QString("+PONG\r\n"));
}

void TestConnection::selectDatabase()
{
    using namespace RedisClient;

    //given
    RedisConnectionConfig config("127.0.0.1");
    Connection connection(config, true);
    QFETCH(int, dbIndex);
    QFETCH(bool, validResult);

    //when
    bool actualResult = connection.operations()->selectDb(dbIndex);

    //then
    QCOMPARE(actualResult, validResult);
}

void TestConnection::selectDatabase_data()
{
    QTest::addColumn<int>("dbIndex");
    QTest::addColumn<bool>("validResult");

    QTest::newRow("Valid db index") << 1 << true;
    QTest::newRow("InValid db index") << 10000 << false;

}

void TestConnection::connectWithAuth()
{
    using namespace RedisClient;

    //given
    RedisConnectionConfig config("127.0.0.1");
    Connection connection(config, true);

    //when
    Command cmd("config set requirepass test");
    CommandExecutor::execute(&connection, cmd);
    connection.disconnect();
    config.auth = "test";
    connection.config = config;
    bool actualResult = connection.connect();
    Command testResultCmd("ping");
    Response actualCommandResult = CommandExecutor::execute(&connection, testResultCmd);

    //then
    QCOMPARE(actualResult, true);
    QCOMPARE(actualCommandResult.toString(), QString("+PONG\r\n"));

}
