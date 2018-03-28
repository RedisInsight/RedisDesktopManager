#include "test_viewmodel.h"
#include <QSignalSpy>
#include <modeltest.h>
#include "modules/value-editor/tabsmodel.h"
#include "modules/connections-tree/items/keyitem.h"
#include "modules/connections-tree/model.h"
#include "mocks/fakeKeyFactory.h"
#include "testcases/connections-tree/mocks/itemoperationsmock.h"

void TestViewModel::testAddKey()
{
    // Given
    using namespace ValueEditor;

    QSharedPointer<FakeKeyFactory> keyFactory(new FakeKeyFactory());
    TabsModel model(keyFactory.dynamicCast<AbstractKeyFactory>());
    QString keyName("fake");
    QString keyType("string");
    QVariantMap row{{"value","fake"}};
    auto connection = getFakeConnection();
    bool callbackCalled = false;

    // When
    // Case 1: Empty NewKeyRequest - addKey() on keyfactory is not called
    model.addKey(keyName, keyType, row, QJSValue());
    QCOMPARE((uint)0, keyFactory->addKeyCalled);

    // Case 2: New Request with valid connection - addKey() called
    QSignalSpy spy(&model, SIGNAL(newKeyDialog(QString, QString)));
    model.openNewKeyDialog(connection.dynamicCast<RedisClient::Connection>(),
                           [&callbackCalled](){ callbackCalled = true; }, 0, "fake_key_prefix");
    QCOMPARE(spy.count(), 1);    
    model.addKey(keyName, keyType, row, QJSValue());
    QVERIFY(callbackCalled);
    QCOMPARE((uint)1, keyFactory->addKeyCalled);

    // Case 3: New Request with invalid connection - addKey() not called
    callbackCalled = false;
    model.openNewKeyDialog(connection.dynamicCast<RedisClient::Connection>(),
                           [&callbackCalled](){ callbackCalled = true; }, 0, "fake_key_prefix");
    connection.clear();
    model.addKey(keyName, keyType, row, QJSValue());
    QVERIFY(callbackCalled == false);
    QCOMPARE((uint)1, keyFactory->addKeyCalled);
}

void TestViewModel::testOpenTab()
{
    // Given
    auto connection = getFakeConnection();
    // ViewModel
    using namespace ValueEditor;
    QSharedPointer<FakeKeyFactory> keyFactory(new FakeKeyFactory());
    TabsModel model(keyFactory.dynamicCast<AbstractKeyFactory>());
    // KeyItem
    ItemOperationsMock* operations = new ItemOperationsMock();
    ConnectionsTree::Model m;
    QSharedPointer<ConnectionsTree::Operations> operations_(
                dynamic_cast<ConnectionsTree::Operations*>(operations));
    QSharedPointer<ConnectionsTree::KeyItem> key(new ConnectionsTree::KeyItem(
                "fake:full:path", 0, operations_,
                QWeakPointer<ConnectionsTree::TreeItem>(), m
    ));


    // When - Case 1 open first key, Welcome tab sould be closed
    model.openTab(connection.dynamicCast<RedisClient::Connection>(),
                  *key.data(), false);

    // Then
    QCOMPARE(keyFactory->loadKeyCalled, 1u);    
    QCOMPARE(model.rowCount(), 1);
    QVERIFY(model.getValue(0) != nullptr);

    // When - Case 2 open second key in the same tab, first tab  should be replaced
    QSignalSpy spy2(&model, SIGNAL(replaceTab(int)));
    model.openTab(connection.dynamicCast<RedisClient::Connection>(),
                  *key.data(), false);

    // Then
    QCOMPARE(keyFactory->loadKeyCalled, 2u);    
    QCOMPARE(spy2.count(), 1);
    QCOMPARE(model.rowCount(), 1);
    QVERIFY(model.getValue(0) != nullptr);
}


void TestViewModel::testAbstractModelMethods()
{
    // Given
    using namespace ValueEditor;

    QSharedPointer<AbstractKeyFactory> keyFactory(new FakeKeyFactory());
    TabsModel model(keyFactory);

    // When
    ModelTest test(&model);

    // Then
    // No assertions
}
