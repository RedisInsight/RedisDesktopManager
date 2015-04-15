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
    operations.getDatabaseKeys(99, [&callbackCalled](const ConnectionsTree::Operations::RawKeysList &result) {
        //then - part 2
        callbackCalled = true;
    });

    //then - part 1
    wait(100);
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
