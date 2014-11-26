#include "test_keymodels.h"


void TestKeyModels::testKeyFactory()
{
    //given
    QFETCH(QString, typeReply);
    QFETCH(QString, ttlReply);
    QStringList replies = QStringList() << typeReply << ttlReply;
    auto dummyConnection = getReadyDummyConnection(replies);    

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

void TestKeyModels::testValueLoading()
{
    //given
    QFETCH(QStringList, testReplies);
    auto dummyConnection = getReadyDummyConnection(testReplies);

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

    keyModel->loadRows(0, keyModel->rowsCount(), [&callbackCalled]() { callbackCalled = true; });
    wait(100);
    QVERIFY(callbackCalled);
    QVERIFY(keyModel->isRowLoaded(testRow));

    QString actualResult = keyModel->getData(testRow, testRole);

    //then
    QFETCH(QString, validData);
    QCOMPARE(actualResult, validData);

}

void TestKeyModels::testValueLoading_data()
{
       QTest::addColumn<QStringList>("testReplies");
       QTest::addColumn<int>("testRow");
       QTest::addColumn<int>("testRole");
       QTest::addColumn<unsigned long>("validRowCount");
       QTest::addColumn<bool>("validIsMultiRow");
       QTest::addColumn<QString>("validData");

       QTest::newRow("Valid string model")
               << (QStringList() << "+string\r\n" << ":-1\r\n" << "$17\r\n__nice_test_data!\r\n")
               << 0
               << Qt::UserRole + 1
               << (unsigned long)1
               << false
               << "__nice_test_data!";

       QTest::newRow("Valid list model")
               << (QStringList() << "+list\r\n" << ":-1\r\n" << ":2\r\n" << "*2\r\n$3\r\nfoo\r\n$3\r\nbar\r\n")
               << 1
               << Qt::UserRole + 1
               << (unsigned long)2
               << true
               << "bar";

       QTest::newRow("Valid set model")
               << (QStringList() << "+set\r\n" << ":-1\r\n" << ":2\r\n" << "*2\r\n$3\r\nfoo\r\n$3\r\nbar\r\n")
               << 1
               << Qt::UserRole + 1
               << (unsigned long)2
               << true
               << "bar";

       QTest::newRow("Valid zset model")
               << (QStringList() << "+zset\r\n" << ":-1\r\n" << ":2\r\n" << "*4\r\n$3\r\nfoo\r\n$1\r\n1\r\n$3\r\nbar\r\n$1\r\n1\r\n")
               << 1
               << Qt::UserRole + 1
               << (unsigned long)2
               << true
               << "bar";

       QTest::newRow("Valid hash model")
               << (QStringList() << "+hash\r\n" << ":-1\r\n" << ":2\r\n" << "*4\r\n$3\r\nfoo\r\n$1\r\n1\r\n$3\r\nbar\r\n$1\r\n1\r\n")
               << 1
               << Qt::UserRole + 1
               << (unsigned long)2
               << true
               << "bar";
}

QSharedPointer<ValueEditor::Model> TestKeyModels::getKeyModel(QSharedPointer<RedisClient::Connection> connection)
{
    QSharedPointer<ValueEditor::Model> actualResult;
    KeyFactory factory;
    factory.loadKey(connection, "testKey", 0, [&actualResult](QSharedPointer<ValueEditor::Model> model) {
        actualResult = model;
    });

    wait(500);

    return actualResult;
}
