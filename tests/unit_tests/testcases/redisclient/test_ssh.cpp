#include <chrono>
#include <thread>
#include <QTest>
#include "test_ssh.h"
#include "redisclient/connection.h"
#include "redisclient/commandexecutor.h"
#include "redisclient/command.h"


using namespace RedisClient;

void TestSsh::init()
{
    qRegisterMetaType<RedisClient::Command>("Command");
    qRegisterMetaType<RedisClient::Response>("RedisClient::Response");

    config = ConnectionConfig("127.0.0.1", "test", 7000);
    config.setParam("auth", "test");
    config.setParam("timeout_execute", 10000);
    config.setParam("timeout_connect", 10000);
}

#ifdef SSH_TESTS
void TestSsh::connectWithSshTunnelPass()
{
    //given
    config.setSshTunnelSettings("192.168.56.222", "vagrant", "vagrant", 22, "");
    Connection connection(config, false);

    //when
    bool actualResult = connection.connect();

    //then
    QCOMPARE(connection.isConnected(), true);
    QCOMPARE(actualResult, true);
}

void TestSsh::connectAndCheckTimeout()
{
    //given
    config.setSshTunnelSettings("192.168.56.222", "vagrant", "vagrant", 22, "");
    Connection connection(config, false);
    Command cmd("ping");

    //when
    bool actualResult = connection.connect();
    wait(15 * 60 * 1000);
    Response actualCmdResult = CommandExecutor::execute(&connection, cmd);
    QCOMPARE(actualCmdResult.toString(), QString("+PONG\r\n"));
    wait(60 * 1000);
    actualCmdResult = CommandExecutor::execute(&connection, cmd);
    QCOMPARE(actualCmdResult.toString(), QString("+PONG\r\n"));
    wait(60 * 1000);

    //then
    QCOMPARE(connection.isConnected(), true);
    QCOMPARE(actualResult, true);
}

void TestSsh::connectWithSshTunnelKey()
{
    //given
    Connection connection(config, false);

    //when
    bool actualResult = connection.connect();

    //then
    QCOMPARE(connection.isConnected(), true);
    QCOMPARE(actualResult, true);
}
#endif
