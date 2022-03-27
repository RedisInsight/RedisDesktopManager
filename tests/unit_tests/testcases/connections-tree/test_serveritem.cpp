#include "test_serveritem.h"

#include <QMenu>
#include <QSignalSpy>
#include <QTest>
#include <QtCore>

#include "respbasetestcase.h"
#include "connections-tree/items/serveritem.h"
#include "connections-tree/model.h"
#include "mocks.h"

using namespace ConnectionsTree;
using namespace fakeit;

TestServerItem::TestServerItem(QObject* parent) : QObject(parent) {}

void TestServerItem::testLoad() {
  // given
  QMap<int, int> databases = {{0, 55}};
  Mock<Operations> operations =
      getOperationsWithGetDatabases(databases, QString());
  Operations& mock = operations.get();
  auto ptr = QSharedPointer<Operations>(&mock, fakeDeleter<Operations>);

  QFETCH(QString, action);

  Model dummyModel;
  ServerItem item{ptr, dummyModel};

  // when
  item.handleEvent(action);

  QTest::qWait(50);

  // then
  QCOMPARE(item.childCount(), static_cast<uint>(1));
  QCOMPARE(item.child(0)->getDisplayName(), QString("db0  (55)"));
  QCOMPARE(item.isLocked(), false);
  QCOMPARE(item.isDatabaseListLoaded(), true);
}

void TestServerItem::testLoad_data() {
  QTest::addColumn<QString>("action");
  QTest::newRow("load") << "click";
  QTest::newRow("reload") << "reload";
}

void TestServerItem::testBasicMethods() {
  // given
  Mock<Operations> operations;
  When(Method(operations, connectionName)).Return("test");
  Operations& mock = operations.get();

  auto ptr = QSharedPointer<Operations>(&mock, fakeDeleter<Operations>);

  Model dummyModel;

  // when
  ServerItem item(ptr, dummyModel);

  // then
  QCOMPARE(item.getDisplayName(), QString("test"));
  QCOMPARE(item.parent() == nullptr, true);
  QCOMPARE(item.isEnabled(), true);
  QCOMPARE(item.isLocked(), false);
  QCOMPARE(item.child(0).isNull(), true);
  QCOMPARE(item.getAllChilds().isEmpty(), true);
  QCOMPARE(item.row(), 0);
}

void TestServerItem::testLoad_invalid() {
  // given
  Mock<Operations> operations =
      getOperationsWithGetDatabases({}, QString("fake connection error"));
  Operations& mock = operations.get();
  auto ptr = QSharedPointer<Operations>(&mock, fakeDeleter<Operations>);

  Model dummyModel;
  ServerItem item{ptr, dummyModel};

  // when
  item.handleEvent("click");
  QTest::qWait(50);

  // then
  // TODO: check mock calls
  QCOMPARE(item.childCount(), static_cast<uint>(0));
  QCOMPARE(item.isLocked(), false);
  QCOMPARE(item.isDatabaseListLoaded(), false);
}

void TestServerItem::testUnload() {
  // given
  QMap<int, int> databases = {{0, 55}};
  Mock<Operations> operations =
      getOperationsWithGetDatabases(databases, QString());
  Operations& mock = operations.get();
  auto ptr = QSharedPointer<Operations>(&mock, fakeDeleter<Operations>);

  Model dummyModel;
  ServerItem item{ptr, dummyModel};

  // when
  item.handleEvent("click");
  QTest::qWait(50);
  item.handleEvent("unload");

  // then
  // TODO: check mock calls
  QCOMPARE(item.childCount(), static_cast<uint>(0));
  QCOMPARE(item.isLocked(), false);
  QCOMPARE(item.isDatabaseListLoaded(), false);
}
