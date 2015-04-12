#include "test_databaseitem.h"
#include "connections-tree/items/databaseitem.h"
#include "mocks/itemoperationsmock.h"

#include <QtCore>
#include <QTest>
#include <QSignalSpy>
#include <QMenu>

using namespace ConnectionsTree;

TestDatabaseItem::TestDatabaseItem(QObject *parent) :
    QObject(parent)
{
}

void TestDatabaseItem::testLoadKeys()
{
    //given
    ItemOperationsMock* operations = new ItemOperationsMock();
    operations->keys.append("test-1-key");
    operations->keys.append("test-2-key");
    operations->keys.append("test-2-key:subkey");
    operations->keys.append("test-2-key:namespace:subkey2");
    DatabaseItem item("test", 0, 300,
                      QSharedPointer<Operations>(dynamic_cast<Operations*>(operations)),
                      nullptr);
    QSignalSpy spy(&item, SIGNAL(keysLoaded(unsigned int)));
    DummyParentView view;

    //when
    bool actualResult = item.onClick(view);

    //then
    QCOMPARE(spy.wait(), true);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(item.childCount(), (unsigned int)3);
    QCOMPARE(actualResult, true);
    QCOMPARE(item.getDisplayName(), QString("test (3/300)"));
    QCOMPARE(item.getIcon().isNull(), false);
    QCOMPARE(item.getAllChilds().isEmpty(), false);
    QCOMPARE(item.isEnabled(), true);
    QCOMPARE(item.isLocked(), false);
}

void TestDatabaseItem::testUnloadKeys()
{
    //given
    ItemOperationsMock* operations = new ItemOperationsMock();
    DatabaseItem item("test", 0, 300,
                      QSharedPointer<Operations>(dynamic_cast<Operations*>(operations)),
                      nullptr);

    //when
    item.unload();

    //then
    QCOMPARE(item.childCount(), (unsigned int)0);
    QCOMPARE(item.isLocked(), false);
}

void TestDatabaseItem::testContextMenu()
{
    //given
    ItemOperationsMock* operations = new ItemOperationsMock();
    DatabaseItem item("test", 0, 300,
                      QSharedPointer<Operations>(dynamic_cast<Operations*>(operations)),
                      nullptr);
    DummyParentView view;

    //when
    QSharedPointer<QMenu> actualResult = item.getContextMenu(view);

    //then
    QCOMPARE(actualResult->isEmpty(), false);
}
