#include "test_databaseitem.h"

#include <QMenu>
#include <QSignalSpy>
#include <QTest>
#include <QtCore>

#include <respbasetestcase.h>
#include "connections-tree/items/databaseitem.h"
#include "connections-tree/items/serveritem.h"
#include "connections-tree/model.h"
#include "mocks.h"

using namespace ConnectionsTree;

TestDatabaseItem::TestDatabaseItem(QObject* parent) : QObject(parent) {}

void TestDatabaseItem::testLoadKeys() {
  // given

  QList<QByteArray> keys;
  for (int i = 1; i < 100000; i++) {
    keys.append(QString("test-%1-key").arg(i).toUtf8());
  }
  keys.append("test-2-key");
  keys.append("test-2-key:subkey");
  keys.append("test-2-key:namespace:subkey2");

  auto operations = getOperationsWithDbAndKeys({{0, 55}}, QString(), keys);

  Operations& mock = operations.get();
  auto ptr = QSharedPointer<Operations>(&mock, fakeDeleter<Operations>);

  Model dummyModel;
  QSharedPointer<ServerItem> parentItem(new ServerItem(ptr, dummyModel));
  parentItem->setWeakPointer(parentItem.toWeakRef());

  // when
  parentItem->handleEvent("click");
  QTest::qWait(150);
  qDebug() << parentItem->childCount();
  QSharedPointer<TreeItem> item = parentItem->child(0);

  item->handleEvent("click");

  QTest::qWait(150);

  // then
  // TODO: check mock calls
  // TODO: verify "load more" item is last
  // TODO: verify namespaces are rendered correctly
  QCOMPARE(item->childCount(), (unsigned int)1002);
  QCOMPARE(item->getDisplayName(), QString("db0  (55)"));
  QCOMPARE(item->getAllChilds().isEmpty(), false);
  QCOMPARE(item->isEnabled(), true);
  QCOMPARE(item->isLocked(), false);
}
