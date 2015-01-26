#include <QTest>
#include "test_connection.h"
#include "redisclient/connection.h"
#include "redisclient/commandexecutor.h"
#include "redisclient/command.h"


using namespace RedisClient;

void TestConnection::init()
{
    qRegisterMetaType<RedisClient::Command>("Command");
    qRegisterMetaType<RedisClient::Response>("RedisClient::Response");

    config = ConnectionConfig();
    config.host = "127.0.0.1";
    config.auth = "test";
    config.name = "test";
    config.port = 6379;
    config.connectionTimeout = 10000;
    config.executeTimeout = 10000;
}

void TestConnection::setSshSettings(ConnectionConfig &c, bool usePass = true)
{
    c.sshHost = "192.168.252.10";
    c.sshPort = 22;
    c.connectionTimeout = 10000;

    if (usePass) {   
        c.sshPassword = "123";
        c.sshUser = "admin";
    } else {
        c.sshUser = "admin";
        c.sshPrivateKeyPath = "D:\\ssh-keys\\private";
        c.connectionTimeout = 300000;
    }
}

#ifdef INTEGRATION_TESTS
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
    bool actualResult = connection.selectDb(dbIndex);

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
    } catch (Connection::Exception&) {
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
    } catch (CommandExecutor::Exception&) {
        hasException = true;
    }

    //then
    QCOMPARE(hasException, true);
}

void TestConnection::runCommandAndDelete()
{
    //given
    Connection connection(config, true);
    Command cmd("ping");
    QObject * owner = new QObject();
    cmd.setCallBack(owner, [](RedisClient::Response r){});

    //when
    connection.runCommand(cmd);
    delete owner;

    //then
    //no errors
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
    QSKIP("This test requires configured ssh server");

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
    QSKIP("This test requires configured ssh server");

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
#endif

void TestConnection::testWithDummyTransporter()
{
    //given            
    // connection with dummy transporter
    QString validResponse("+PONG\r\n");
    QSharedPointer<Connection> connection = getReadyDummyConnection(QStringList() << validResponse);
    Command cmd("ping");

    //when    
    connection->connect();
    Response actualResult = CommandExecutor::execute(connection, cmd);

    //then
    QCOMPARE(connection->isConnected(), true);
    QCOMPARE(actualResult.toString(), validResponse);
}
