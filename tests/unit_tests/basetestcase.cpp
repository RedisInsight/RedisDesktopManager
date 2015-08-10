#include "basetestcase.h"

RedisClient::ConnectionConfig BaseTestCase::getDummyConfig(QString name)
{
    RedisClient::ConnectionConfig dummyConf("127.0.0.1", name);
    dummyConf.setParam("auth", "");
    dummyConf.setParam("timeout_execute", 2000);
    dummyConf.setParam("timeout_connect", 2000);

    return dummyConf;
}

QSharedPointer<RedisClient::Connection> BaseTestCase::getRealConnectionWithDummyTransporter(const QStringList &expectedResponses)
{
    RedisClient::ConnectionConfig dummyConf = getDummyConfig();

    QSharedPointer<RedisClient::Connection> connection( new RedisClient::Connection(dummyConf, false));

    QSharedPointer<DummyTransporter> transporter(new DummyTransporter(connection.data()));
    transporter->setFakeResponses(expectedResponses);

    connection->setTransporter(transporter.dynamicCast<RedisClient::AbstractTransporter>());
    connection->connect();

    return connection;
}

QSharedPointer<DummyConnection> BaseTestCase::getFakeConnection(const QList<QVariant> &expectedScanResponses, const QStringList &expectedResponses, double version, bool raise_error)
{
    QSharedPointer<DummyConnection> connection(new DummyConnection(version, raise_error));
    connection->fakeScanCollections.append(expectedScanResponses);
    connection->setFakeResponses(expectedResponses);

    return connection;
}

void BaseTestCase::wait(int ms)
{
    //wait for data
    QEventLoop loop;
    QTimer timeoutTimer;

    timeoutTimer.setSingleShot(true);
    QObject::connect(&timeoutTimer, SIGNAL(timeout()), &loop, SLOT(quit()));

    timeoutTimer.start(ms);
    loop.exec();
}

void BaseTestCase::verifyExecutedCommandsCount(QSharedPointer<RedisClient::Connection> connection, uint valid_result)
{
    auto dummyTransporter = connection->getTransporter().dynamicCast<DummyTransporter>();
    QCOMPARE((uint)dummyTransporter->addCommandCalls, valid_result);
}
