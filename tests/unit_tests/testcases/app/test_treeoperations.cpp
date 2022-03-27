#include "test_treeoperations.h"
#include <qredisclient/connection.h>
#include <fakeit.hpp>

#include "app/events.h"
#include "connections-tree/items/databaseitem.h"
#include "connections-tree/model.h"
#include "models/connectionconf.h"
#include "models/treeoperations.h"

using namespace fakeit;
using namespace ConnectionsTree;

void TestTreeOperations::testCreation() {
  // given
  auto events = QSharedPointer<Events>(new Events());
  auto config = getDummyConfig();

  // when
  TreeOperations operations(config, events);

  // then
  // all ok
  Q_UNUSED(operations);
}

void TestTreeOperations::testGetDatabases() {
  // given
  auto events = QSharedPointer<Events>(new Events());
  QString infoResp = getBulkStringReply(
      "# CPU\n"
      "used_cpu_sys:17.89\n"
      "used_cpu_user:24.70\n"
      "used_cpu_sys_children:0.06\n"
      "used_cpu_user_children:0.33\n\n"
      "# Keyspace\n"
      "db0:keys=3495,expires=0,avg_ttl=0\n"
      "db9:keys=1,expires=0,avg_ttl=0\n");

  QStringList expectedResponses{infoResp,  infoResp,  "+OK\r\n",
                                "+OK\r\n", "+OK\r\n", "-ERROR\r\n"};
  auto connection = getFakeConnection();
  connection->setFakeResponses(expectedResponses);
  connection->setClone(connection);

  // Fake callback
  RedisClient::DatabaseList result;
  Mock<TreeItem> fake;
  TreeItem& owner = fake.get();
  auto fakeOwner = QSharedPointer<TreeItem>(&owner, fakeDeleter<TreeItem>);

  auto callback = QSharedPointer<Operations::GetDatabasesCallback>(
      new Operations::GetDatabasesCallback(
          fakeOwner,
          [&result](Operations::DbMapping r, const QString&) { result = r; }));

  // when
  qDebug() << "testGetDatabases - start execution";
  TreeOperations operations(getDummyConfig(), events);
  operations.setConnection(connection);

  operations.getDatabases(callback);

  // then
  wait(100);
  connection.clear();
  QCOMPARE(result.size(), 13);
}

void TestTreeOperations::testLoadNamespaceItems() {
  // given
  QFETCH(uint, runCommandCalled);
  QFETCH(uint, retrieveCollectionCalled);
  QFETCH(QList<QVariant>, expectedScanResponses);
  QFETCH(QStringList, expectedResponses);

  // Setup dummy connection with on/off lua loading
  auto connection = getFakeConnection(expectedScanResponses, expectedResponses);
  ServerConfig conf;
  connection->setConnectionConfig(conf);

  auto events = QSharedPointer<Events>(new Events());
  QSharedPointer<TreeOperations> operations(
      new TreeOperations(getDummyConfig(), events));
  operations->setConnection(connection);

  // Fake callback
  RedisClient::Connection::RawKeysList result;
  Mock<TreeItem> fake;
  TreeItem& owner = fake.get();
  auto fakeOwner = QSharedPointer<TreeItem>(&owner, fakeDeleter<TreeItem>);

  auto callback = QSharedPointer<Operations::LoadNamespaceItemsCallback>(
      new Operations::LoadNamespaceItemsCallback(
          fakeOwner,
          [&result](const RedisClient::Connection::RawKeysList& r,
                  const QString&) { result = r; }));

  // when
  operations->loadNamespaceItems(
              0, QString("*"), callback);

  // then - part 1
  wait(5);
  QCOMPARE(result.size(), 2);
  QCOMPARE(connection->runCommandCalled, runCommandCalled);
  QCOMPARE(connection->retrieveCollectionCalled, retrieveCollectionCalled);
}

void TestTreeOperations::testLoadNamespaceItems_data() {
  QTest::addColumn<uint>("runCommandCalled");
  QTest::addColumn<uint>("retrieveCollectionCalled");
  QTest::addColumn<QList<QVariant> >("expectedScanResponses");
  QTest::addColumn<QStringList>("expectedResponses");
  QTest::newRow("SCAN execution")
      << 1u << 1u
      << (QList<QVariant>()
          << QVariant(QVariantList() << QString("test") << QString("test2")))
      << (QStringList() << "+OK\r\n");
}

void TestTreeOperations::testFlushDb() {
  // given
  auto events = QSharedPointer<Events>(new Events());
  auto connection = getFakeConnection(QList<QVariant>() << QVariant(),
                                      QStringList() << "+OK\r\n");

  // Mock callback
  bool callbackCalledWithError = false;
  Mock<TreeItem> fake;
  TreeItem& owner = fake.get();
  auto fakeOwner = QSharedPointer<TreeItem>(&owner, fakeDeleter<TreeItem>);

  auto callback = QSharedPointer<Operations::FlushDbCallback>(
      new Operations::FlushDbCallback(
          fakeOwner, [&callbackCalledWithError](const QString& e) {
            callbackCalledWithError = !e.isEmpty();
          }));

  // when
  TreeOperations operations(getDummyConfig(), events);
  operations.setConnection(connection);
  operations.flushDb(0, callback);

  // then - part 1
  wait(5);
  QCOMPARE(callbackCalledWithError, false);
  QCOMPARE(connection->runCommandCalled, 1u);
  QCOMPARE(connection->executedCommands[0].getPartAsString(0),
           QString("FLUSHDB"));
}

void TestTreeOperations::testFlushDbCommandError() {
  // given
  auto events = QSharedPointer<Events>(new Events());
  auto connection = getFakeConnection();
  connection->returnErrorOnCmdRun = true;

  // Fake callback
  bool callbackCalledWithError = false;
  Mock<TreeItem> fake;
  TreeItem& owner = fake.get();
  auto fakeOwner = QSharedPointer<TreeItem>(&owner, fakeDeleter<TreeItem>);

  auto callback = QSharedPointer<Operations::FlushDbCallback>(
      new Operations::FlushDbCallback(
          fakeOwner, [&callbackCalledWithError](const QString& e) {
            callbackCalledWithError = !e.isEmpty();
          }));

  // when
  TreeOperations operations(getDummyConfig(), events);
  operations.setConnection(connection);
  operations.flushDb(0, callback);

  // then - part 1
  wait(5);
  QCOMPARE(callbackCalledWithError, true);
}
