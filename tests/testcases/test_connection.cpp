#include <QTest>
#include "connection.h"
#include "commandexecutor.h"
#include "test_connection.h"
#include "abstractprotocol.h"
#include "connectionexception.h"

using namespace RedisClient;

void TestConnection::init()
{
    config = ConnectionConfig();
    config.host = "127.0.0.1";
    config.auth = "test";
    config.connectionTimeout = 30000;
}

void TestConnection::setSshSettings(ConnectionConfig &c, bool usePass = true)
{
    c.sshHost = "192.168.252.10";
    c.sshPort = 22;

    if (usePass) {   
        c.sshPassword = "123";
        c.sshUser = "admin";
    } else {
        c.sshUser = "admin";
        c.sshPrivateKeyPath = "D:\\ssh-keys\\private";
        c.connectionTimeout = 300000;
    }
}

void TestConnection::connectToHostAndRunCommand()
{
    //given
    Connection connection(config, true);
    Command cmd("ping");

    //when
    //sync execution
    Response actualResult = CommandExecutor::execute(&connection, cmd);

    //then
    QCOMPARE(connection.isConnected(), true);
    QCOMPARE(actualResult.toString(), QString("+PONG\r\n"));    
}

void TestConnection::selectDatabase()
{
    //given
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

void TestConnection::runEmptyCommand()
{
    //given
    Connection connection(config, true);
    Command cmd;

    //when
    bool hasException = false;
    try {
        connection.runCommand(cmd);
    } catch (ConnectionExeption&) {
        hasException = true;
    }

    //then
    QCOMPARE(hasException, true);
}

void TestConnection::runCommandWithoutConnection()
{
    //given
    Connection connection(config, false);
    Command cmd("PING"); //valid

    //when
    bool hasException = false;
    try {
        CommandExecutor::execute(&connection, cmd);
    } catch (ConnectionExeption&) {
        hasException = true;
    }

    //then
    QCOMPARE(hasException, true);
}

void TestConnection::connectWithAuth()
{   
    //given
    Connection connection(config, true);

    //when
    Command cmd("config set requirepass test");
    CommandExecutor::execute(&connection, cmd);
    connection.disconnect();

    bool actualConnectResult = connection.connect();
    Command testResultCmd("ping");
    Response actualCommandResult = CommandExecutor::execute(&connection, testResultCmd);

    //then
    QCOMPARE(actualConnectResult, true);
    QCOMPARE(actualCommandResult.toString(), QString("+PONG\r\n"));
}

void TestConnection::connectWithSshTunnelPass()
{
    #ifndef WIN32
    QSKIP("This test requires ssh server")
    #endif

    //given
    setSshSettings(config);

    Connection connection(config, false);

    //when
    bool actualResult = connection.connect();

    //then
    QCOMPARE(connection.isConnected(), true);
    QCOMPARE(actualResult, true);
}

void TestConnection::connectWithSshTunnelKey()
{
    #ifndef WIN32
    QSKIP("This test requires ssh server")
    #endif

    //given
    setSshSettings(config, false);

    Connection connection(config, false);

    //when
    bool actualResult = connection.connect();

    //then
    QCOMPARE(connection.isConnected(), true);
    QCOMPARE(actualResult, true);
}

void TestConnection::connectAndDisconnect()
{
    //given
    Connection connection(config, false);

    //when
    bool connectResult = connection.connect();
    connection.disconnect();

    //then
    QCOMPARE(connectResult, true);
    QCOMPARE(connection.isConnected(), false);
}

