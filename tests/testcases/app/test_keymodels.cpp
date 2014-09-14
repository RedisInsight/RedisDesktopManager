#include "test_keymodels.h"
#include "models/key-models/keyfactory.h"

void TestKeyModels::testKeyFactory()
{
    //given
    QStringList replies = QStringList()
            << "+string\r\n" // type cmd
            << ":-1\r\n" //ttl
            ;
    auto dummyConnection = getReadyDummyConnection(replies);
    KeyFactory factory(dummyConnection);

    //when
    QSharedPointer<ValueEditor::Model> actualResult;
    factory.loadKey("testkey", 0, [&actualResult](QSharedPointer<ValueEditor::Model> model) {
        actualResult = model;
    });

    wait(500);

    //then
    QCOMPARE(actualResult->getType(), QString("string"));
    QCOMPARE(actualResult->getTTL(), -1);

}
