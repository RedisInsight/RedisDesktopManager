#include "test_databaseitem.h"
#include "connections-tree/items/databaseitem.h"
#include "connections-tree/items/serveritem.h"
#include "connections-tree/model.h"
#include "mocks/itemoperationsmock.h"

#include <QMenu>
#include <QSignalSpy>
#include <QTest>
#include <QtCore>

using namespace ConnectionsTree;

TestDatabaseItem::TestDatabaseItem(QObject* parent) : QObject(parent) {}

void TestDatabaseItem::testLoadKeys() {
  // given
  ItemOperationsMock* operations = new ItemOperationsMock();
  QSharedPointer<Operations> operations_(dynamic_cast<Operations*>(operations));
  operations->databases.insert(0, 55);

  for (int i = 1; i < 100000; i++) {
    operations->keys.append(QString("test-%1-key").arg(i).toUtf8());
  }
  operations->keys.append("test-2-key");
  operations->keys.append("test-2-key:subkey");
  operations->keys.append("test-2-key:namespace:subkey2");
  Model dummyModel;
  QSharedPointer<ServerItem> parentItem(
      new ServerItem("test", operations_, dummyModel));
  parentItem->setWeakPointer(parentItem.toWeakRef());

  // when
  parentItem->handleEvent("click");
  qDebug() << parentItem->childCount();
  QSharedPointer<TreeItem> item = parentItem->child(0);

  QSignalSpy spy(&dummyModel, &Model::itemChildsLoaded);
  item->handleEvent("click");

  // then
  QCOMPARE(spy.count(), 1);
  QCOMPARE(item->childCount(), (unsigned int)100002);
  QCOMPARE(item->getDisplayName(), QString("db0  (55)"));
  QCOMPARE(item->getAllChilds().isEmpty(), false);
  QCOMPARE(item->isEnabled(), true);
  QCOMPARE(item->isLocked(), false);
}

// FIXME: add test for reload instead
// void TestDatabaseItem::testUnloadKeys()
//{
//    //given
//    ItemOperationsMock* operations = new ItemOperationsMock();
//    Model model;
//    DatabaseItem item(0, 300,
//    QSharedPointer<Operations>(dynamic_cast<Operations*>(operations)),
//                      QWeakPointer<ConnectionsTree::TreeItem>(), model);

//    //when
//    item.unload();

//    //then
//    QCOMPARE(item.childCount(), (unsigned int)0);
//    QCOMPARE(item.isLocked(), false);
//}
