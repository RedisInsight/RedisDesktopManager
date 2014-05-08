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
    Command cmd("config set requirepass test"); // todo: find way to disable auth in redis
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

void TestConnection::connectWithSshTunnel()
{
    using namespace RedisClient;

    //given
    RedisConnectionConfig config("192.168.252.10"); //todo: move all configuration options to separete file
    config.sshHost = "127.0.0.1";
    config.sshPort = 22;
    config.sshPassword = "123";
    config.sshUser = "admin";
    config.connectionTimeout = 120000;
    Connection connection(config, false);

    //when
    bool actualResult = connection.connect();

    //then
    QCOMPARE(actualResult, true);
}
