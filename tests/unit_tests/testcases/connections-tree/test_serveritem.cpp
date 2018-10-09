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
    operations->databases.insert(0, 55);
    Model dummyModel;
    ServerItem item {"test", QSharedPointer<Operations>(dynamic_cast<Operations*>(operations)), dummyModel};
    QSignalSpy spy(&dummyModel, SIGNAL(itemChildsLoaded(QWeakPointer<TreeItem>)));

    //when
    item.handleEvent("click");

    //then
    QCOMPARE(spy.count(), 1);
    QCOMPARE(item.childCount(), static_cast<uint>(1));
    QCOMPARE(item.isLocked(), false);
    QCOMPARE(item.isDatabaseListLoaded(), true);    
}

void TestServerItem::testLoad_invalid()
{
    //given
    ItemOperationsMock* operations = new ItemOperationsMock(false);
    Model dummyModel;
    ServerItem item {"test", QSharedPointer<Operations>(dynamic_cast<Operations*>(operations)), dummyModel};    

    //when
    item.handleEvent("click");

    //then    
    QCOMPARE(item.childCount(), static_cast<uint>(0));
    QCOMPARE(item.isLocked(), false);
    QCOMPARE(item.isDatabaseListLoaded(), false);    
}

void TestServerItem::testUnload()
{
    //given
    ItemOperationsMock* operations = new ItemOperationsMock();
    operations->databases.insert(0, 55);
    Model dummyModel;
    ServerItem item("test", (QSharedPointer<Operations>(dynamic_cast<Operations*>(operations))), dummyModel);

    //when
    item.handleEvent("unload");

    //then
    QCOMPARE(item.childCount(), static_cast<uint>(0));
    QCOMPARE(item.isLocked(), false);
    QCOMPARE(item.isDatabaseListLoaded(), false);
}

void TestServerItem::testReload()
{
    //given
    ItemOperationsMock* operations = new ItemOperationsMock();
    operations->databases.insert(0, 55);
    Model dummyModel;
    ServerItem item{"test", (QSharedPointer<Operations>(dynamic_cast<Operations*>(operations))), dummyModel};
    QSignalSpy spy(&dummyModel, SIGNAL(itemChildsLoaded(QWeakPointer<TreeItem>)));

    //when
    item.handleEvent("reload");

    //then
    QCOMPARE(spy.count(), 1);
}

void TestServerItem::testBasicMethods()
{
    //given
    ItemOperationsMock* operations = new ItemOperationsMock();
    Model dummyModel;

    //when
    ServerItem item("test", (QSharedPointer<Operations>(dynamic_cast<Operations*>(operations))), dummyModel);

    //then
    QCOMPARE(item.getDisplayName(), QString("test"));    
    QCOMPARE(item.parent() == nullptr, true);
    QCOMPARE(item.isEnabled(), true);
    QCOMPARE(item.isLocked(), false);
    QCOMPARE(item.child(0).isNull(), true);
    QCOMPARE(item.getAllChilds().isEmpty(), true);
    QCOMPARE(item.row(), 0);
}

