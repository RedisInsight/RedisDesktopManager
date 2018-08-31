#include <QtTest/QtTest>
#include <QSignalSpy>
#include "console/consolemodel.h"
#include "test_consolemodel.h"


void TestConsoleOperations::init_invalid()
{
    //given
    QSharedPointer<RedisClient::Connection> invalidConnection = getFakeConnection(
        QList<QVariant>(), QStringList(), 2.6, true
    ).dynamicCast<RedisClient::Connection>();
    Console::Model testModel(invalidConnection, 0);
    QSignalSpy spy(&testModel, SIGNAL(addOutput(const QString&, QString)));
    
    //when
    testModel.init();
        
    //then
    QCOMPARE(spy.count(), 1);
    QList<QVariant> arguments = spy.takeFirst();
    QVERIFY(arguments.at(0).toString() == "Invalid Connection. Check connection settings.");
}
