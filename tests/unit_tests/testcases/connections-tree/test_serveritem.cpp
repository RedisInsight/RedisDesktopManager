#include "test_serveritem.h"
#include "mocks/itemoperationsmock.h"
#include "connections-tree/items/serveritem.h"
#include "connections-tree/model.h"

#include <QtCore>
#include <QTest>
#include <QSignalSpy>
#include <QMenu>

using namespace ConnectionsTree;

TestServerItem::TestServerItem(QObject *parent) :
    QObject(parent)
{
}

void TestServerItem::testLoad()
{    
    //given
    ItemOperationsMock* operations = new ItemOperationsMock();
    operations->databases["test-db"] = 55;
    Model dummyModel;
    ServerItem item {"test", QSharedPointer<Operations>(dynamic_cast<Operations*>(operations)), dummyModel};
    QSignalSpy spy(&item, SIGNAL(databaseListLoaded()));

    //when
    item.load();

    //then
    QCOMPARE(spy.count(), 1);
    QCOMPARE(item.childCount(), static_cast<uint>(1));
    QCOMPARE(item.isLocked(), false);
    QCOMPARE(item.isDatabaseListLoaded(), true);
}

void TestServerItem::testUnload()
{
    //given
    ItemOperationsMock* operations = new ItemOperationsMock();
    operations->databases["test-db"] = 55;
    Model dummyModel;
    ServerItem item("test", (QSharedPointer<Operations>(dynamic_cast<Operations*>(operations))), dummyModel);

    //when
    item.unload();

    //then
    QCOMPARE(item.childCount(), static_cast<uint>(0));
    QCOMPARE(item.isLocked(), false);
    QCOMPARE(item.isDatabaseListLoaded(), false);
}

void TestServerItem::testReload()
{
    //given
    ItemOperationsMock* operations = new ItemOperationsMock();
    operations->databases["test-db"] = 55;
    Model dummyModel;
    ServerItem item{"test", (QSharedPointer<Operations>(dynamic_cast<Operations*>(operations))), dummyModel};
    QSignalSpy spy(&item, SIGNAL(databaseListLoaded()));

    //when
    item.reload();

    //then
    QCOMPARE(spy.count(), 1);
}

void TestServerItem::testContextMenu()
{
    //given
    ItemOperationsMock* operations = new ItemOperationsMock();
    Model dummyModel;
    ServerItem item("test", (QSharedPointer<Operations>(dynamic_cast<Operations*>(operations))), dummyModel);
    DummyParentView view;

    //when
    QSharedPointer<QMenu> actualResult = item.getContextMenu(view);

    //then
    QCOMPARE(actualResult->isEmpty(), false);
}

