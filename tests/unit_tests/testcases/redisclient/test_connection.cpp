#include <chrono>
#include <thread>
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

    config = ConnectionConfig("127.0.0.1", "test", 6379);
    config.setParam("auth", "test");
    config.setParam("timeout_execute", 10000);
    config.setParam("timeout_connect", 10000);
}

void TestConnection::setSshSettings(ConnectionConfig &c, bool usePass = true)
{        
    c.setParam("timeout_connect", 10000);

    c.setSshTunnelSettings("127.0.0.1", "test",
                           (usePass)? "test" : "",
                           2222, (usePass)? "" : "fixme");
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
    bool actualResultFirst = connection.selectDb(dbIndex);
    bool actualResultSecond = connection.selectDb(dbIndex);

    //then
    QCOMPARE(actualResultFirst, validResult);
    QCOMPARE(actualResultSecond, validResult);
}

void TestConnection::selectDatabase_data()
{
    QTest::addColumn<int>("dbIndex");
    QTest::addColumn<bool>("validResult");

    QTest::newRow("Valid db index") << 1 << true;
    QTest::newRow("InValid db index") << 10000 << false;
}

void TestConnection::testScanCommand()
{
    //given
    Connection connection(config, true);
    Command cmd(QStringList() << "SCAN" << "0"); //valid

    //when
    Response result = CommandExecutor::execute(&connection, cmd);
    QVariant value = result.getValue();


    //then
    QCOMPARE(value.isNull(), false);
}

void TestConnection::testRetriveCollection()
{
    //given
    Connection connection(config, true);
    QSharedPointer<ScanCommand> cmd(new ScanCommand("SCAN 0")); //valid
    bool callbackCalled = false;
    QVERIFY(cmd->isValidScanCommand());

    //when
    CommandExecutor::execute(&connection, Command() << "FLUSHDB");
    CommandExecutor::execute(&connection, Command() << "SET" << "test" << "1");
    connection.retrieveCollection(cmd, [&callbackCalled](QVariant result) {
        //then - part 1
        QCOMPARE(result.isNull(), false);
        QCOMPARE(result.toList().size(), 1);
        QCOMPARE(result.canConvert(QMetaType::QVariantList), true);
        callbackCalled = true;
    });
    wait(2000);

    //then - part 2
    QCOMPARE(callbackCalled, true);
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


void TestConnection::connectWithSshTunnelPass() // FIXME
{
    //QSKIP("This test requires configured ssh server");
    //given
    setSshSettings(config, true);

    Connection connection(config, false);

    //when
    bool actualResult = connection.connect();

    //then
    QCOMPARE(connection.isConnected(), true);
    QCOMPARE(actualResult, true);
}

void TestConnection::connectWithSshTunnelKey() // FIXME
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

void TestConnection::testParseServerInfo()
{
    //given
    QString testInfo("# Server\n"
                     "redis_version:2.9.999\n"
                     "redis_git_sha1:3bf72d0d\n"
                     "redis_git_dirty:0\n"
                     "redis_build_id:69b45658ca5a9e2d\n"
                     "redis_mode:standalone\n"
                     "os:Linux 3.13.7-x86_64-linode38 x86_64\n"
                     "arch_bits:32\n"
                     "multiplexing_api:epoll\n"
                     "gcc_version:4.4.1\n"
                     "process_id:14029\n"
                     "run_id:63bccba63aa231ac84b459af7a6ae34cb89caecd\n"
                     "tcp_port:6379\n"
                     "uptime_in_seconds:18354826\n"
                     "uptime_in_days:212\n"
                     "hz:10\n"
                     "lru_clock:14100747\n"
                     "config_file:/etc/redis/6379.conf\n");

    //when
    ServerInfo actualResult = ServerInfo::fromString(testInfo);

    //then
    QCOMPARE(actualResult.version, 2.9);
}

void TestConnection::testConfig()
{
    //given
    Connection connection(config, false);
    ConnectionConfig empty;

    //when
    connection.setConnectionConfig(empty);
    ConnectionConfig actualResult = connection.getConfig();

    //then
    QCOMPARE(actualResult.isNull(), empty.isNull());
}
