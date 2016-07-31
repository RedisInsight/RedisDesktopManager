#include "test_treeoperations.h"
#include <qredisclient/connection.h>
#include "models/treeoperations.h"
#include "models/connectionsmanager.h"
#include "app/widgets/consoletabs.h"

void TestTreeOperations::testCreation()
{
    //given
    ConsoleTabs tabsWidget;
    ConnectionsManager manager(QString(), tabsWidget);
    auto connection = getRealConnectionWithDummyTransporter();

    //when
    TreeOperations operations(connection, tabsWidget, manager);

    //then
    //all ok
    Q_UNUSED(operations);
}

void TestTreeOperations::testGetDatabases()
{
    //given
    ConsoleTabs tabsWidget;
    ConnectionsManager manager(QString(), tabsWidget);

    QStringList expectedResponses{
        getBulkStringReply(
            "# CPU\n"
            "used_cpu_sys:17.89\n"
            "used_cpu_user:24.70\n"
            "used_cpu_sys_children:0.06\n"
            "used_cpu_user_children:0.33\n\n"
            "# Keyspace\n"
            "db0:keys=3495,expires=0,avg_ttl=0\n"
            "db999:keys=1,expires=0,avg_ttl=0\n"
        ),
        "+OK\r\n", "+OK\r\n", "+OK\r\n", "-ERROR\r\n"
    };
    auto connection = getFakeConnection();
    connection->setFakeResponses(expectedResponses);
    bool callbackCalled = false;
    RedisClient::DatabaseList result;

    //when
    qDebug() << "testGetDatabases - start execution";        
    TreeOperations operations(connection, tabsWidget, manager);
    operations.getDatabases(
    [&callbackCalled, &result](const RedisClient::DatabaseList& r) {
        callbackCalled = true;
        result = r;
    });

    //then
    wait(5);
    QCOMPARE(callbackCalled, true);
    QCOMPARE(connection->runCommandCalled, 4u);
    QCOMPARE(result.size(), 1003);
}

void TestTreeOperations::testGetDatabaseKeys()
{    
    //given
    QFETCH(double, redisServerVersion);
    QFETCH(uint, runCommandCalled);
    QFETCH(uint, retrieveCollectionCalled);
    ConsoleTabs tabsWidget;
    ConnectionsManager manager(QString(), tabsWidget);
    auto connection = getFakeConnection(QList<QVariant>() << QVariant(),
                                        QStringList() << "",
                                        redisServerVersion);

    //when
    bool callbackCalled = false;
    TreeOperations operations(connection, tabsWidget, manager);
    operations.getDatabaseKeys(99, [&callbackCalled](
                               const RedisClient::Connection::RawKeysList&,
                               const QString&)
    {
        //then - part 2
        callbackCalled = true;
    });

    //then - part 1
    wait(5);
    QCOMPARE(callbackCalled, true);
    QCOMPARE(connection->runCommandCalled, runCommandCalled);
    QCOMPARE(connection->getServerVersionCalled, 1u);
    QCOMPARE(connection->retrieveCollectionCalled, retrieveCollectionCalled);

}

void TestTreeOperations::testGetDatabaseKeys_data()
{
    QTest::addColumn<double>("redisServerVersion");
    QTest::addColumn<uint>("runCommandCalled");
    QTest::addColumn<uint>("retrieveCollectionCalled");
    QTest::newRow("Legacy redis <= 2.6") << 2.6 << 1u << 0u;
    QTest::newRow("New redis >= 2.8") << 2.8 << 0u << 1u;
}

void TestTreeOperations::testFlushDb()
{
    //given
    ConsoleTabs tabsWidget;
    ConnectionsManager manager(QString(), tabsWidget);
    auto connection = getFakeConnection(QList<QVariant>() << QVariant(),
                                        QStringList() << "+OK");

    //when
    bool callbackCalled = false;
    TreeOperations operations(connection, tabsWidget, manager);
    operations.flushDb(0, [&callbackCalled](const QString&)
    {
        //then - part 2
        callbackCalled = true;
    });

    //then - part 1
    wait(5);
    QCOMPARE(callbackCalled, true);
    QCOMPARE(connection->runCommandCalled, 1u);
    QCOMPARE(connection->executedCommands[0].getPartAsString(0), QString("FLUSHDB"));
}

void TestTreeOperations::testFlushDbCommandError()
{
    //given
    ConsoleTabs tabsWidget;
    ConnectionsManager manager(QString(), tabsWidget);
    auto connection = getFakeConnection();
    connection->returnErrorOnCmdRun = true;

    //when
    bool callbackCalledWithError = false;
    TreeOperations operations(connection, tabsWidget, manager);
    operations.flushDb(0, [&callbackCalledWithError](const QString& e)
    {
        //then - part 2
        callbackCalledWithError = !e.isEmpty();
    });

    //then - part 1
    wait(5);
    QCOMPARE(callbackCalledWithError, true);
}
