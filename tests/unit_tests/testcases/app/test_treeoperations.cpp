#include "test_treeoperations.h"
#include "models/treeoperations.h"
#include "redisclient/connection.h"
#include "app/widgets/consoletabs.h"

void TestTreeOperations::testCreation()
{
    //given
    ConsoleTabs tabsWidget;
    auto connection = getReadyDummyConnection();

    //when
    TreeOperations operations(connection, tabsWidget);

    //then
    //all ok
    Q_UNUSED(operations);
}

void TestTreeOperations::testGetDatabases()
{
    //given
    ConsoleTabs tabsWidget;
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
        )
    };
    auto connection = getFakeConnection();
    connection->setFakeResponses(expectedResponses);
    bool callbackCalled = false;
    ConnectionsTree::Operations::DatabaseList result;

    //when
    qDebug() << "testGetDatabases - start execution";        
    TreeOperations operations(connection, tabsWidget);
    operations.getDatabases(
    [&callbackCalled, &result](const ConnectionsTree::Operations::DatabaseList& r) {    
        callbackCalled = true;
        result = r;
    });

    //then
    wait(5);
    QCOMPARE(callbackCalled, true);
    QCOMPARE(connection->runCommandCalled, 1u);
    QCOMPARE(result.size(), 1000);              
}

void TestTreeOperations::testGetDatabasesWithSelectScan()
{
    //given
    ConsoleTabs tabsWidget;
    QStringList expectedResponses{
        getBulkStringReply(
            "# CPU\n"
            "used_cpu_sys:17.89\n"           
            "# Keyspace\n"
        ),
        "+OK\r\n", "+OK\r\n", "+OK\r\n", "-ERROR\r\n"
    };
    auto connection = getFakeConnection();
    connection->setFakeResponses(expectedResponses);
    bool callbackCalled = false;
    ConnectionsTree::Operations::DatabaseList result;

    //when
    TreeOperations operations(connection, tabsWidget);
    operations.getDatabases(
    [&callbackCalled, &result](const ConnectionsTree::Operations::DatabaseList& r) {    
        callbackCalled = true;
        result = r;
    });

    //then
    wait(5);
    QCOMPARE(callbackCalled, true);
    QCOMPARE(connection->runCommandCalled, 5u);
    QCOMPARE(result.size(), 3);
}

void TestTreeOperations::testGetDatabaseKeys()
{    
    //given
    QFETCH(double, redisServerVersion);
    QFETCH(uint, runCommandCalled);
    QFETCH(uint, retrieveCollectionCalled);
    ConsoleTabs tabsWidget;
    auto connection = getFakeConnection(QList<QVariant>() << QVariant(),
                                        QStringList() << "",
                                        redisServerVersion);

    //when
    bool callbackCalled = false;
    TreeOperations operations(connection, tabsWidget);
    operations.getDatabaseKeys(99, [&callbackCalled](const ConnectionsTree::Operations::RawKeysList&) {
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
