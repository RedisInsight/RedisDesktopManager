#include "test_keymodels.h"

#include "app/models/key-models/hashkey.h"
#include "app/models/key-models/listkey.h"
#include "app/models/key-models/setkey.h"
#include "app/models/key-models/sortedsetkey.h"
#include "app/models/key-models/stringkey.h"

void TestKeyModels::testKeyFactory() {
  // given
  QFETCH(QStringList, validReplies);
  auto dummyConnection = getRealConnectionWithDummyTransporter(validReplies);

  // when
  QSharedPointer<ValueEditor::Model> actualResult =
      getKeyModel(dummyConnection);

  // then
  QFETCH(QString, typeValid);
  QFETCH(int, ttlValid);
  QCOMPARE(actualResult.isNull(), false);
  QCOMPARE(actualResult->type(), typeValid);
  QCOMPARE(actualResult->getTTL(), ttlValid);
}

void TestKeyModels::testKeyFactory_data() {
  QTest::addColumn<QStringList>("validReplies");
  QTest::addColumn<QString>("typeValid");
  QTest::addColumn<int>("ttlValid");

  QTest::newRow("Valid string model w/o TTL") << (QStringList() << "+string\r\n"
                                                                << ":-1\r\n")
                                              << "string" << -1;

  QTest::newRow("Valid string model w TTL") << (QStringList() << "+string\r\n"
                                                              << ":100\r\n")
                                            << "string" << 100;

  QTest::newRow("Valid list model w/o TTL") << (QStringList() << "+list\r\n"
                                                              << ":-1\r\n"
                                                              << ":1\r\n")
                                            << "list" << -1;

  QTest::newRow("Valid set model w/o TTL") << (QStringList() << "+set\r\n"
                                                             << ":-1\r\n"
                                                             << ":1\r\n")
                                           << "set" << -1;

  QTest::newRow("Valid sorted set model w/o TTL")
      << (QStringList() << "+zset\r\n"
                        << ":-1\r\n"
                        << ":1\r\n")
      << "zset" << -1;

  QTest::newRow("Valid hash model w/o TTL") << (QStringList() << "+hash\r\n"
                                                              << ":-1\r\n"
                                                              << ":1\r\n")
                                            << "hash" << -1;
}

void TestKeyModels::testKeyFactoryAddKey() {
  // given
  QFETCH(QStringList, testReplies);
  QFETCH(QString, keyType);
  QFETCH(QVariantMap, row);
  auto connection = getRealConnectionWithDummyTransporter(testReplies);
  KeyFactory factory;
  NewKeyRequest r(connection, -1, []() {});

  // when
  r.setKeyName("testKey");
  r.setKeyType(keyType);
  r.setValue(row);
  factory.submitNewKeyRequest(r);
  wait(100);

  // then
  verifyExecutedCommandsCount(connection,
                              testReplies.size() + 2);  // 2 = ping + info
}

void TestKeyModels::testKeyFactoryAddKey_data() {
  QTest::addColumn<QStringList>("testReplies");
  QTest::addColumn<QString>("keyType");
  QTest::addColumn<QVariantMap>("row");

  QVariantMap singleRow{{"value", "test"}};
  QTest::newRow("string") << (QStringList() << "+OK\r\n") << "string"
                          << singleRow;
  QTest::newRow("list") << (QStringList() << "+OK\r\n") << "list" << singleRow;
  QTest::newRow("set") << (QStringList() << "+OK\r\n") << "set" << singleRow;

  QVariantMap hashRow{{"value", "test"}, {"key", "test-key"}};
  QTest::newRow("hash") << (QStringList() << ":1\r\n") << "hash" << hashRow;

  QVariantMap zsetRow{{"value", "test"}, {"score", 5.0}};
  QTest::newRow("zset") << (QStringList() << "+OK\r\n") << "zset" << zsetRow;
}

void TestKeyModels::testValueLoading() {
  // given
  QFETCH(QStringList, testReplies);
  auto dummyConnection = getRealConnectionWithDummyTransporter(testReplies);

  QFETCH(int, testRow);
  QFETCH(int, testRole);
  QFETCH(unsigned long, validRowCount);
  QFETCH(bool, validIsMultiRow);

  // when
  QSharedPointer<ValueEditor::Model> keyModel = getKeyModel(dummyConnection);
  QVERIFY(keyModel.isNull() == false);
  QVERIFY(keyModel->isMultiRow() == validIsMultiRow);

  bool callbackCalled = false;

  keyModel->loadRowsCount([keyModel, &callbackCalled, validRowCount](QString) {
    QVERIFY(keyModel->rowsCount() == validRowCount);

    keyModel->loadRows(0, keyModel->rowsCount(),
                       [&callbackCalled](const QString&, unsigned long) {
                         callbackCalled = true;
                       });
  });

  wait(500);
  QVERIFY(callbackCalled);
  QVERIFY(keyModel->isRowLoaded(testRow));

  QVariant actualResult = keyModel->getData(testRow, testRole);
  keyModel->clearRowCache();

  // then
  QFETCH(QString, validData);
  QFETCH(QStringList, validColumns);
  QCOMPARE(actualResult.toString(), validData);
  QCOMPARE(keyModel->getColumnNames(), validColumns);
  QVERIFY(keyModel->getRoles().size() != 0);
  QVERIFY(keyModel->isRowLoaded(0) == false);
}

void TestKeyModels::testValueLoading_data() {
  QTest::addColumn<QStringList>("testReplies");
  QTest::addColumn<int>("testRow");
  QTest::addColumn<int>("testRole");
  QTest::addColumn<unsigned long>("validRowCount");
  QTest::addColumn<bool>("validIsMultiRow");
  QTest::addColumn<QString>("validData");
  QTest::addColumn<QStringList>("validColumns");

  QTest::newRow("Valid string model")
      << (QStringList() << "+string\r\n"
                        << ":-1\r\n"
                        << "$17\r\n__nice_test_data!\r\n")
      << 0 << Qt::UserRole + 1 << (unsigned long)1 << false
      << "__nice_test_data!" << QStringList();

  QTest::newRow("Valid list model")
      << (QStringList() << "+list\r\n"
                        << ":-1\r\n"
                        << ":2\r\n"
                        << "*2\r\n$3\r\nfoo\r\n$3\r\nbar\r\n")
      << 1 << Qt::UserRole + 2 << (unsigned long)2 << true << "bar"
      << (QStringList() << "row"
                        << "value");

  QTest::newRow("Valid set model")
      << (QStringList() << "+set\r\n"
                        << ":-1\r\n"
                        << ":2\r\n"
                        << "*2\r\n$1\r\n0\r\n*2\r\n$3\r\nfoo\r\n$3\r\nbar\r\n"
                        << ":1\r\n"
                        << ":1\r\n"
                        << ":1\r\n"
                        << ":1\r\n")
      << 1 << Qt::UserRole + 2 << (unsigned long)2 << true << "bar"
      << (QStringList() << "row"
                        << "value");

  QTest::newRow("Valid zset model")
      << (QStringList()
          << "+zset\r\n"
          << ":-1\r\n"
          << ":2\r\n"
          << "*4\r\n$3\r\nfoo\r\n$1\r\n1\r\n$3\r\nbar\r\n$1\r\n1\r\n")
      << 1 << Qt::UserRole + 2 << (unsigned long)2 << true << "bar"
      << (QStringList() << "row"
                        << "value"
                        << "score");

  QTest::newRow("Valid hash model")
      << (QStringList() << "+hash\r\n"
                        << ":-1\r\n"
                        << ":2\r\n"
                        << "*2\r\n$1\r\n0\r\n*4\r\n$3\r\nfoo\r\n$1\r\n1\r\n$"
                           "3\r\nfoo\r\n$3\r\nbar\r\n")
      << 1 << Qt::UserRole + 3 << (unsigned long)2 << true << "bar"
      << (QStringList() << "row"
                        << "key"
                        << "value");
}

void TestKeyModels::testKeyModelModifyRows() {
  // given
  QFETCH(QStringList, testReplies);
  QFETCH(QVariantMap, row);
  QFETCH(int, role);
  auto dummyConnection = getRealConnectionWithDummyTransporter(testReplies);

  // when
  QSharedPointer<ValueEditor::Model> keyModel = getKeyModel(dummyConnection);
  QVERIFY(keyModel.isNull() == false);
  keyModel->loadRows(0, 10, [](const QString&, unsigned long) { return; });
  wait(500);
  keyModel->addRow(row, [](QString) {});
  row["value"] = "fakeUpdate";
  keyModel->updateRow(0, row, [](QString) {});
  wait(100);
  QVariant actualResult = keyModel->getData(0, role);
  keyModel->removeRow(0, [](QString) {});

  // then
  QVERIFY(actualResult.type() == QVariant::ByteArray);
  QCOMPARE(actualResult.toString(), QString("fakeUpdate"));
}

void TestKeyModels::testKeyModelModifyRows_data() {
  QTest::addColumn<QStringList>("testReplies");
  QTest::addColumn<QVariantMap>("row");
  QTest::addColumn<int>("role");

  QVariantMap stringRow;
  stringRow["value"] = "test";
  QTest::newRow("Valid string model")
      << (QStringList() << "+string\r\n"
                        << ":-1\r\n"
                        << "$17\r\n__nice_test_data!\r\n"
                        << "+OK\r\n"
                        << "+OK\r\n")
      << stringRow << Qt::UserRole + 1;

  QVariantMap listRow;
  listRow["row"] = 0;
  listRow["value"] = "test";
  QTest::newRow("Valid list model")
      << (QStringList() << "+list\r\n"
                        << ":2\r\n"
                        << "*2\r\n$3\r\nfoo\r\n$3\r\nbar\r\n"
                        << "+OK\r\n"
                        << "*1\r\n$3\r\nfoo\r\n"
                        << "+OK\r\n"
                        << "*1\r\n$10\r\nfakeUpdate\r\n"
                        << "+OK\r\n"
                        << "+OK\r\n")
      << listRow << Qt::UserRole + 2;

  QVariantMap setRow;
  setRow["row"] = 0;
  setRow["value"] = "test";
  QTest::newRow("Valid set model")
      << (QStringList() << "+set\r\n"
                        << ":2\r\n"
                        << "*2\r\n$1\r\n0\r\n*2\r\n$3\r\nfoo\r\n$3\r\nbar\r\n"
                        << ":1\r\n"
                        << "+OK\r\n"
                        << ":1\r\n"
                        << "+OK\r\n")
      << setRow << Qt::UserRole + 2;

  QVariantMap zsetRow;
  zsetRow["row"] = 0;
  zsetRow["value"] = "test";
  zsetRow["score"] = 1.1;
  QTest::newRow("Valid zset model")
      << (QStringList()
          << "+zset\r\n"
          << ":2\r\n"
          << "*4\r\n$3\r\nfoo\r\n$1\r\n1\r\n$3\r\nbar\r\n$1\r\n1\r\n"
          << ":1\r\n"
          << ":1\r\n"
          << ":1\r\n"
          << ":1\r\n")
      << zsetRow << Qt::UserRole + 2;

  QVariantMap hashRow;
  hashRow["row"] = 0;
  hashRow["key"] = "test";
  hashRow["value"] = "test";
  QTest::newRow("Valid hash model")
      << (QStringList() << "+hash\r\n"
                        << ":2\r\n"
                        << "*2\r\n$1\r\n0\r\n*4\r\n$3\r\nfoo\r\n$1\r\n1\r\n$"
                           "3\r\nbar\r\n$1\r\n1\r\n"
                        << ":1\r\n"
                        << ":1\r\n"
                        << ":1\r\n"
                        << ":1\r\n")
      << hashRow << Qt::UserRole + 3;
}

QSharedPointer<ValueEditor::Model> TestKeyModels::getKeyModel(
    QSharedPointer<RedisClient::Connection> connection) {
  QSharedPointer<ValueEditor::Model> actualResult;
  KeyFactory factory;
  factory.loadKey(connection, "testKey", -1,
                  [&actualResult](QSharedPointer<ValueEditor::Model> model,
                                  const QString&) { actualResult = model; });

  wait(100);

  return actualResult;
}
