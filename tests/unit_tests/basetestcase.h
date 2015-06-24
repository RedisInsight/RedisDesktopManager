#pragma once

#include <QObject>
#include <QStringList>
#include <QTest>
#include <QEventLoop>
#include <QTimer>
#include "redisclient/connection.h"
#include "redisclient/connectionconfig.h"
#include "testcases/redisclient/mocks/dummyTransporter.h"
#include "testcases/redisclient/mocks/dummyconnection.h"

class BaseTestCase : public QObject
{
    Q_OBJECT

protected:

    RedisClient::ConnectionConfig getDummyConfig(QString name="test")
    {
        RedisClient::ConnectionConfig dummyConf("127.0.0.1", name);
        dummyConf.setParam("auth", "");
        dummyConf.setParam("timeout_execute", 2000);
        dummyConf.setParam("timeout_connect", 2000); 
        
        return dummyConf;   
    }
        

    QSharedPointer<RedisClient::Connection> getReadyDummyConnection(const QStringList& expectedResponses = QStringList())
    {
        RedisClient::ConnectionConfig dummyConf = getDummyConfig();

        QSharedPointer<RedisClient::Connection> connection( new RedisClient::Connection(dummyConf, false));

        QSharedPointer<DummyTransporter> transporter(new DummyTransporter(connection.data()));
        transporter->setFakeResponses(expectedResponses);

        connection->setTransporter(transporter.dynamicCast<RedisClient::AbstractTransporter>());
        connection->connect();

        return connection;
    }

    QSharedPointer<DummyConnection> getFakeConnection(const QList<QVariant>& expectedScanResponses = QList<QVariant>(),
                                                      const QStringList& expectedResponses = QStringList(),
                                                      double version=2.6)
    {
        QSharedPointer<DummyConnection> connection(new DummyConnection(version));
        connection->fakeScanCollections.append(expectedScanResponses);
        connection->setFakeResponses(expectedResponses);

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
