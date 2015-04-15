#pragma once
#include <QObject>
#include <QtCore>
#include "basetestcase.h"
#include "redisclient/connectionconfig.h"


class TestSsh : public BaseTestCase
{
    Q_OBJECT

private slots:
    void init();

#ifdef SSH_TESTS
    void connectWithSshTunnelPass();
    void connectWithSshTunnelKey();
    void connectAndCheckTimeout();
#endif
private:
    RedisClient::ConnectionConfig config;
};

