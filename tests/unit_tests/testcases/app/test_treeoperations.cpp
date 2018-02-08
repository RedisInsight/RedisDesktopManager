#include "test_treeoperations.h"
#include <qredisclient/connection.h>
#include "models/treeoperations.h"
#include "models/connectionsmanager.h"
#include "models/connectionconf.h"
#include "connections-tree/model.h"
#include "connections-tree/items/databaseitem.h"

void TestTreeOperations::testCreation()
{
    //given
    ConnectionsManager manager{QString()};
    auto connection = getRealConnectionWithDummyTransporter();

    //when
    TreeOperations operations(connection, manager);

    //then
    //all ok
    Q_UNUSED(operations);
}

void TestTreeOperations::testGetDatabases()
{
    //given
    ConnectionsManager manager{QString()};
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
    TreeOperations operations(connection, manager);
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

void TestTreeOperations::testLoadNamespaceItems()
{    
    //given
    QFETCH(bool, useLuaLoading);
    QFETCH(uint, runCommandCalled);
    QFETCH(uint, retrieveCollectionCalled);    
    QFETCH(QList<QVariant>, expectedScanResponses);
    QFETCH(QStringList, expectedResponses);

    // Setup dummy connection with on/off lua loading
    auto connection = getFakeConnection(expectedScanResponses, expectedResponses);
    ServerConfig conf;
    conf.setLuaKeysLoading(useLuaLoading);
    connection->setConnectionConfig(conf);

    ConnectionsManager manager{QString()};
    QSharedPointer<TreeOperations> operations(new TreeOperations(connection, manager));

    ConnectionsTree::Model dummyModel;
    QSharedPointer<ConnectionsTree::DatabaseItem> item(
                new ConnectionsTree::DatabaseItem(
                    0, 1, operations, QWeakPointer<ConnectionsTree::TreeItem>(), dummyModel
                )
     );


    //when
    bool callbackCalled = false;

    operations->loadNamespaceItems(
                qSharedPointerDynamicCast<ConnectionsTree::AbstractNamespaceItem>(item),
                QString(), [&callbackCalled](const QString& err)
    {
        //then - part 2
        callbackCalled = true;
        QVERIFY2(err.isEmpty(), qPrintable(err));
    });

    //then - part 1
    wait(5);
    QCOMPARE(callbackCalled, true);
    QCOMPARE(connection->runCommandCalled, runCommandCalled);
    QCOMPARE(connection->retrieveCollectionCalled, retrieveCollectionCalled);
}

void TestTreeOperations::testLoadNamespaceItems_data()
{    
    QTest::addColumn<bool>("useLuaLoading");
    QTest::addColumn<uint>("runCommandCalled");
    QTest::addColumn<uint>("retrieveCollectionCalled");
    QTest::addColumn< QList<QVariant> >("expectedScanResponses");
    QTest::addColumn< QStringList >("expectedResponses");
    QTest::newRow("LUA execution")
            << true << 1u << 0u << (QList<QVariant>() << QVariant()) << (QStringList() << "*2\r\n$2\r\n{}\r\n\$20\r\n{\"test\":1,\"test2\":1}\r\n");
    QTest::newRow("SCAN execution")
            << false << 0u << 1u << (QList<QVariant>() << QVariant(QVariantList() << QString("test") << QString("test2"))) << (QStringList() << "");
}

void TestTreeOperations::testFlushDb()
{
    //given
    ConnectionsManager manager{QString()};
    auto connection = getFakeConnection(QList<QVariant>() << QVariant(),
                                        QStringList() << "+OK");

    //when
    bool callbackCalled = false;
    TreeOperations operations(connection, manager);
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
    ConnectionsManager manager{QString()};
    auto connection = getFakeConnection();
    connection->returnErrorOnCmdRun = true;

    //when
    bool callbackCalledWithError = false;
    TreeOperations operations(connection, manager);
    operations.flushDb(0, [&callbackCalledWithError](const QString& e)
    {
        //then - part 2
        callbackCalledWithError = !e.isEmpty();
    });

    //then - part 1
    wait(5);
    QCOMPARE(callbackCalledWithError, true);
}
