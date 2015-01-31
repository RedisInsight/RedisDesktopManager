#pragma once

#include <QObject>
#include <QStringList>
#include <QTest>
#include <QEventLoop>
#include <QTimer>
#include "redisclient/connection.h"
#include "redisclient/connectionconfig.h"
#include "testcases/redisclient/mocks/dummyTransporter.h"

class BaseTestCase : public QObject
{
    Q_OBJECT

protected:

    QSharedPointer<RedisClient::Connection> getReadyDummyConnection(const QStringList& expectedResponses = QStringList())
    {
        RedisClient::ConnectionConfig dummyConf("127.0.0.1");
        dummyConf.auth = "";
        dummyConf.name = "test";
        dummyConf.executeTimeout = 2000;
        dummyConf.connectionTimeout = 2000;

        QSharedPointer<RedisClient::Connection> connection( new RedisClient::Connection(dummyConf, false));

        QSharedPointer<DummyTransporter> transporter(new DummyTransporter(connection.data()));
        transporter->setFakeResponses(expectedResponses);

        connection->setTransporter(transporter.dynamicCast<RedisClient::AbstractTransporter>());
        connection->connect();

        return connection;
    }

    void wait(int ms)
    {
        //wait for data
        QEventLoop loop;
        QTimer timeoutTimer;

        timeoutTimer.setSingleShot(true);
        QObject::connect(&timeoutTimer, SIGNAL(timeout()), &loop, SLOT(quit()));

        timeoutTimer.start(ms);
        loop.exec();
    }

};
