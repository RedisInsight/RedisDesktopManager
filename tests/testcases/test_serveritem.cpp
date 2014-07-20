#include "test_serveritem.h"
#include "itemoperationsmock.h"
#include "items/serveritem.h"

#include <QtCore>
#include <QTest>

TestServerItem::TestServerItem(QObject *parent) :
    QObject(parent)
{
}

void TestServerItem::testLoad()
{
    using namespace ConnectionsTree;

    //given
    ItemOperationsMock* operations = new ItemOperationsMock();
    operations->databases["test-db"] = 55;
    ServerItem item((QSharedPointer<Operations>(dynamic_cast<Operations*>(operations))));

    //when
    item.load();

    //then
    QCOMPARE(item.childCount(), static_cast<uint>(1));
    QCOMPARE(item.isLocked(), false);
}

