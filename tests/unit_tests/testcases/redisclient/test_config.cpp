#include "test_config.h"
#include "modules/redisclient/connectionconfig.h"
#include "modules/redisclient/connection.h"

#include <QDebug>
#include <QTest>
#include <QJsonObject>

using namespace RedisClient;

void TestConfig::testGetParam()
{
    //given
    QString host = "fake_host";
    QString name = "fake_name";
    int port = 1111;
    ConnectionConfig config(host, name, port);

    //when
    QString actualHost = config.param<QString>("host");
    QString actualName = config.param<QString>("name");
    int actualPort = config.param<int>("port");

    //then
    QCOMPARE(actualHost, host);
    QCOMPARE(actualName, name);
    QCOMPARE(actualPort, port);
}

void TestConfig::testOwner()
{
    //given
    ConnectionConfig config;
    QSharedPointer<Connection> testObj(new Connection(config));

    //when
    config.setOwner(testObj.toWeakRef());


    //then
    QCOMPARE(config.getOwner().toStrongRef(), testObj);
}

void TestConfig::testSerialization()
{
    //given
    QJsonObject test {
        {"host", "fake"},
        {"name", "fake"},
        {"port", 1111},
        {"timeout_connect", 60000},
        {"timeout_execute", 60000},
    };

    //when
    ConnectionConfig config = ConnectionConfig::fromJsonObject(test);
    QJsonObject actualResult = config.toJsonObject();

    //then
    QCOMPARE(config.name(), QString("fake"));
    QCOMPARE(config.host(), QString("fake"));
    QCOMPARE(config.port(), 1111);
    QCOMPARE(config.executeTimeout(), 60000);
    QCOMPARE(config.connectionTimeout(), 60000);
    QCOMPARE(actualResult.contains("auth"), false);
    QCOMPARE(actualResult.contains("namespaceSeparator"), false);
    QCOMPARE(actualResult.size(), test.size());
}
