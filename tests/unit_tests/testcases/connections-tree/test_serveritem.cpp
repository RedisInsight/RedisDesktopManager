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
    DummyParentView view;

    //when
    bool actualResult = item.onClick(view);

    //then
    QCOMPARE(spy.count(), 1);
    QCOMPARE(item.childCount(), static_cast<uint>(1));
    QCOMPARE(item.isLocked(), false);
    QCOMPARE(item.isDatabaseListLoaded(), true);
    QCOMPARE(actualResult, true);
}

void TestServerItem::testLoad_invalid()
{
    //given
    ItemOperationsMock* operations = new ItemOperationsMock(false);
    Model dummyModel;
    ServerItem item {"test", QSharedPointer<Operations>(dynamic_cast<Operations*>(operations)), dummyModel};
    QSignalSpy spy(&dummyModel, SIGNAL(error(const QString&)));
    DummyParentView view;

    //when
    bool actualResult = item.onClick(view);

    //then
    QCOMPARE(spy.count(), 1);
    QCOMPARE(item.childCount(), static_cast<uint>(0));
    QCOMPARE(item.isLocked(), false);
    QCOMPARE(item.isDatabaseListLoaded(), false);
    QCOMPARE(actualResult, false);
}

void TestServerItem::testUnload()
{
    //given
    ItemOperationsMock* operations = new ItemOperationsMock();
    operations->databases.insert(0, 55);
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
    operations->databases.insert(0, 55);
    Model dummyModel;
    ServerItem item{"test", (QSharedPointer<Operations>(dynamic_cast<Operations*>(operations))), dummyModel};
    QSignalSpy spy(&dummyModel, SIGNAL(itemChildsLoaded(QWeakPointer<TreeItem>)));

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

void TestServerItem::testBasicMethods()
{
    //given
    ItemOperationsMock* operations = new ItemOperationsMock();
    Model dummyModel;

    //when
    ServerItem item("test", (QSharedPointer<Operations>(dynamic_cast<Operations*>(operations))), dummyModel);

    //then
    QCOMPARE(item.getDisplayName(), QString("test"));
    QCOMPARE(item.getIcon().isNull(), false);
    QCOMPARE(item.parent() == nullptr, true);
    QCOMPARE(item.isEnabled(), true);
    QCOMPARE(item.isLocked(), false);
    QCOMPARE(item.child(0).isNull(), true);
    QCOMPARE(item.getAllChilds().isEmpty(), true);
    QCOMPARE(item.row(), 0);
}

