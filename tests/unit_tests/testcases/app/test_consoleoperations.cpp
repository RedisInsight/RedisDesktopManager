#include <QtTest/QtTest>
#include <QSignalSpy>
#include "models/consoleoperations.h"
#include "test_consoleoperations.h"


void TestConsoleOperations::init_invalid()
{
    //given
    QSharedPointer<RedisClient::Connection> invalidConnection = getFakeConnection(
        QList<QVariant>(), QStringList(), 2.6, true
    ).dynamicCast<RedisClient::Connection>();
    ConsoleModel testModel(invalidConnection);
    QSignalSpy spy(&testModel, SIGNAL(addOutput(const QString&, QConsole::ResultType)));
    
    //when
    testModel.init();
        
    //then
    QCOMPARE(spy.count(), 1);
    QList<QVariant> arguments = spy.takeFirst();
    QVERIFY(arguments.at(0).toString() == "Invalid Connection. Check connection settings.");
}
