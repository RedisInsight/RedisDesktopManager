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

    RedisClient::ConnectionConfig getDummyConfig(QString name="test");
        

    QSharedPointer<RedisClient::Connection> getRealConnectionWithDummyTransporter(
            const QStringList& expectedResponses = QStringList());

    QSharedPointer<DummyConnection> getFakeConnection(const QList<QVariant>& expectedScanResponses = QList<QVariant>(),
                                                      const QStringList& expectedResponses = QStringList(),
                                                      double version=2.6,
                                                      bool raise_error=false);

    void wait(int ms);

    void verifyExecutedCommandsCount(QSharedPointer<RedisClient::Connection> connection, uint valid_result);
    
    QString getBulkStringReply(const QString& s);

};
