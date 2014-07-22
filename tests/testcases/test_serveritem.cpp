#include "test_serveritem.h"
#include "itemoperationsmock.h"
#include "items/serveritem.h"

#include <QtCore>
#include <QTest>
#include <QSignalSpy>

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
    ServerItem item("test", (QSharedPointer<Operations>(dynamic_cast<Operations*>(operations))));
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
    ServerItem item("test", (QSharedPointer<Operations>(dynamic_cast<Operations*>(operations))));

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
    ServerItem item("test", (QSharedPointer<Operations>(dynamic_cast<Operations*>(operations))));
    QSignalSpy spy(&item, SIGNAL(databaseListLoaded()));

    //when
    item.reload();

    //then
    QCOMPARE(spy.count(), 1);
}

