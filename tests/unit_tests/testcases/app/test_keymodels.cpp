#include "test_keymodels.h"

#include "app/models/key-models/hashkey.h"
#include "app/models/key-models/listkey.h"
#include "app/models/key-models/setkey.h"
#include "app/models/key-models/sortedsetkey.h"
#include "app/models/key-models/stringkey.h"

void TestKeyModels::testKeyFactory()
{
    //given
    QFETCH(QString, typeReply);
    QFETCH(QString, ttlReply);
    QStringList replies = QStringList() << typeReply << ttlReply;
    auto dummyConnection = getRealConnectionWithDummyTransporter(replies);

    //when
    QSharedPointer<ValueEditor::Model> actualResult = getKeyModel(dummyConnection);

    //then
    QFETCH(QString, typeValid);
    QFETCH(int, ttlValid);
    QCOMPARE(actualResult.isNull(), false);
    QCOMPARE(actualResult->getType(), typeValid);
    QCOMPARE(actualResult->getTTL(), ttlValid);

}

void TestKeyModels::testKeyFactory_data()
{
       QTest::addColumn<QString>("typeReply");
       QTest::addColumn<QString>("ttlReply");
       QTest::addColumn<QString>("typeValid");
       QTest::addColumn<int>("ttlValid");

       QTest::newRow("Valid string model w/o TTL")
               << "+string\r\n"
               << ":-1\r\n"
               << "string"
               << -1;

       QTest::newRow("Valid string model w TTL")
               << "+string\r\n"
               << ":100\r\n"
               << "string"
               << 100;

       QTest::newRow("Valid list model w/o TTL")
               << "+list\r\n"
               << ":-1\r\n"
               << "list"
               << -1;

       QTest::newRow("Valid set model w/o TTL")
               << "+set\r\n"
               << ":-1\r\n"
               << "set"
               << -1;

       QTest::newRow("Valid sorted set model w/o TTL")
               << "+zset\r\n"
               << ":-1\r\n"
               << "zset"
               << -1;

       QTest::newRow("Valid hash model w/o TTL")
               << "+hash\r\n"
               << ":-1\r\n"
               << "hash"
               << -1;
}

void TestKeyModels::testKeyFactoryAddKey()
{
    //given
    QFETCH(QStringList, testReplies);
    QFETCH(QString, keyType);
    QFETCH(QVariantMap, row);
    auto connection = getRealConnectionWithDummyTransporter(testReplies);

    KeyFactory factory;
    factory.addKey(connection, "testKey", 0, keyType, row);
}

void TestKeyModels::testKeyFactoryAddKey_data()
{
    QTest::addColumn<QStringList>("testReplies");
    QTest::addColumn<QString>("keyType");
    QTest::addColumn<QVariantMap>("row");

    QVariantMap strRow {{"value", "test"}, {"key": "fake"}};

    QTest::newRow("Valid string model w/o TTL")
            << (QStringList() << "+OK\r\n")
            << "string"
            << strRow;
}

void TestKeyModels::testValueLoading()
{
    //given
    QFETCH(QStringList, testReplies);
    auto dummyConnection = getRealConnectionWithDummyTransporter(testReplies);

    QFETCH(int, testRow);
    QFETCH(int, testRole);
    QFETCH(unsigned long, validRowCount);
    QFETCH(bool, validIsMultiRow);

    //when
    QSharedPointer<ValueEditor::Model> keyModel = getKeyModel(dummyConnection);
    QVERIFY(keyModel.isNull() == false);
    QVERIFY(keyModel->rowsCount() == validRowCount);
    QVERIFY(keyModel->isMultiRow() == validIsMultiRow);

    bool callbackCalled = false;

    keyModel->loadRows(0, keyModel->rowsCount(), [&callbackCalled](const QString&) { callbackCalled = true; });
    wait(500);
    QVERIFY(callbackCalled);
    QVERIFY(keyModel->isRowLoaded(testRow));

    QVariant actualResult = keyModel->getData(testRow, testRole);
    keyModel->clearRowCache();

    //then
    QFETCH(QString, validData);
    QFETCH(QStringList, validColumns);
    QCOMPARE(actualResult.toString(), validData);
    QCOMPARE(keyModel->getColumnNames(), validColumns);
    QVERIFY(keyModel->getRoles().size() != 0);
    QVERIFY(keyModel->isRowLoaded(0) == false);

}

void TestKeyModels::testValueLoading_data()
{
       QTest::addColumn<QStringList>("testReplies");
       QTest::addColumn<int>("testRow");
       QTest::addColumn<int>("testRole");
       QTest::addColumn<unsigned long>("validRowCount");
       QTest::addColumn<bool>("validIsMultiRow");
       QTest::addColumn<QString>("validData");
       QTest::addColumn<QStringList>("validColumns");

       QTest::newRow("Valid string model")
               << (QStringList() << "+string\r\n" << ":-1\r\n" << "$17\r\n__nice_test_data!\r\n")
               << 0
               << Qt::UserRole + 1
               << (unsigned long)1
               << false
               << "__nice_test_data!"
               << QStringList();

       QTest::newRow("Valid list model")
               << (QStringList() << "+list\r\n" << ":-1\r\n" << ":2\r\n" << "*2\r\n$3\r\nfoo\r\n$3\r\nbar\r\n")
               << 1
               << Qt::UserRole + 1
               << (unsigned long)2
               << true
               << "bar"
               << (QStringList() << "row" << "value");

       QTest::newRow("Valid set model")
               << (QStringList() << "+set\r\n" << ":-1\r\n" << ":2\r\n" << "*2\r\n$3\r\nfoo\r\n$3\r\nbar\r\n")
               << 1
               << Qt::UserRole + 1
               << (unsigned long)2
               << true
               << "bar"
               << (QStringList() << "row" << "value");

       QTest::newRow("Valid zset model")
               << (QStringList() << "+zset\r\n" << ":-1\r\n" << ":2\r\n" << "*4\r\n$3\r\nfoo\r\n$1\r\n1\r\n$3\r\nbar\r\n$1\r\n1\r\n")
               << 1
               << Qt::UserRole + 1
               << (unsigned long)2
               << true
               << "bar"
               << (QStringList() << "row" << "value" << "score");

       QTest::newRow("Valid hash model")
               << (QStringList() << "+hash\r\n" << ":-1\r\n" << ":2\r\n" << "*4\r\n$3\r\nfoo\r\n$1\r\n1\r\n$3\r\nbar\r\n$1\r\n1\r\n")
               << 1
               << Qt::UserRole + 1
               << (unsigned long)2
               << true
               << "bar"
               << (QStringList() << "row" << "key" << "value");
}

void TestKeyModels::testKeyModelModifyRows()
{
    //given
    QFETCH(QStringList, testReplies);
    QFETCH(QVariantMap, row);
    QFETCH(int, role);
    QFETCH(int, validRowCount);
    auto dummyConnection = getRealConnectionWithDummyTransporter(testReplies);

    //when
    QSharedPointer<ValueEditor::Model> keyModel = getKeyModel(dummyConnection);
    QVERIFY(keyModel.isNull() == false);
    keyModel->loadRows(0, 10, [](const QString&){return;});
    wait(500);
    keyModel->addRow(row);
    row["value"] = "fakeUpdate";
    keyModel->updateRow(0, row);
    QVariant actualResult = keyModel->getData(0, role);
    keyModel->removeRow(0);

    //then
    QVERIFY(actualResult.type() == QVariant::ByteArray);
    QCOMPARE(actualResult.toString(), QString("fakeUpdate"));
    QCOMPARE(keyModel->rowsCount(), (unsigned long)validRowCount);
}

void TestKeyModels::testKeyModelModifyRows_data()
{
    QTest::addColumn<QStringList>("testReplies");
    QTest::addColumn<QVariantMap>("row");
    QTest::addColumn<int>("role");
    QTest::addColumn<int>("validRowCount");

    QVariantMap stringRow;
    stringRow["value"] = "test";
    QTest::newRow("Valid string model")
            << (QStringList()
                << "+string\r\n"
                << ":-1\r\n"
                << "$17\r\n__nice_test_data!\r\n"
                << "+OK\r\n"
                << "+OK\r\n")
            << stringRow
            << Qt::UserRole + 1
            << 0;

    QVariantMap listRow;
    listRow["row"] = 0;
    listRow["value"] = "test";
    QTest::newRow("Valid list model")
            << (QStringList()
                << "+list\r\n"
                << ":-1\r\n"
                << ":2\r\n"
                << "*2\r\n$3\r\nfoo\r\n$3\r\nbar\r\n"
                << "+OK\r\n"
                << "*1\r\n$3\r\nfoo\r\n"
                << "+OK\r\n"
                << "*1\r\n$10\r\nfakeUpdate\r\n"
                << "+OK\r\n"
                << "+OK\r\n")
            << listRow
            << Qt::UserRole + 1
            << 2;

    QVariantMap setRow;
    setRow["row"] = 0;
    setRow["value"] = "test";
    QTest::newRow("Valid set model")
            << (QStringList()
                << "+set\r\n"
                << ":-1\r\n"
                << ":2\r\n"
                << "*2\r\n$3\r\nfoo\r\n$3\r\nbar\r\n"
                << "+OK\r\n")
            << setRow
            << Qt::UserRole + 1
            << 2;

    QVariantMap zsetRow;
    zsetRow["row"] = 0;
    zsetRow["value"] = "test";
    zsetRow["score"] = 1.1;
    QTest::newRow("Valid zset model")
            << (QStringList()
                << "+zset\r\n"
                << ":-1\r\n"
                << ":2\r\n"
                << "*4\r\n$3\r\nfoo\r\n$1\r\n1\r\n$3\r\nbar\r\n$1\r\n1\r\n"
                << ":1\r\n"
                << ":1\r\n"
                << ":1\r\n"
                << ":1\r\n"
                )
            << zsetRow
            << Qt::UserRole + 1
            << 2;

    QVariantMap hashRow;
    hashRow["row"] = 0;
    hashRow["key"] = "test";
    hashRow["value"] = "test";
    QTest::newRow("Valid hash model")
            << (QStringList()
                << "+hash\r\n"
                << ":-1\r\n"
                << ":2\r\n"
                << "*4\r\n$3\r\nfoo\r\n$1\r\n1\r\n$3\r\nbar\r\n$1\r\n1\r\n"
                << ":1\r\n"
                << ":1\r\n"
                << ":1\r\n")
            << hashRow
            << Qt::UserRole + 2
            << 2;
}

QSharedPointer<ValueEditor::Model> TestKeyModels::getKeyModel(QSharedPointer<RedisClient::Connection> connection)
{
    QSharedPointer<ValueEditor::Model> actualResult;
    KeyFactory factory;
    factory.loadKey(connection, "testKey", 0, [&actualResult](QSharedPointer<ValueEditor::Model> model, const QString&) {
        actualResult = model;
    });

    wait(100);

    return actualResult;
}
