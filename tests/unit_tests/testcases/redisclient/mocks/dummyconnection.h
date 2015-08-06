#pragma once
#include <functional>
#include <QVariant>
#include "redisclient/connection.h"
#include "redisclient/scancommand.h"

class DummyConnection : public RedisClient::Connection
{
public:
    DummyConnection(double version=2.6, bool raise_error=false)
        : RedisClient::Connection(RedisClient::ConnectionConfig(), false),
          runCommandCalled(0), retrieveCollectionCalled(0),
          getServerVersionCalled(0), m_version(version), 
          m_raiseExceptionOnConnect(raise_error)
    {
        if (!m_raiseExceptionOnConnect)
            m_connected = true;
    }

    bool connect()
    {
        if (!m_raiseExceptionOnConnect)
            m_connected = true;
        return m_connected;            
    }

    double getServerVersion() override
    {
        getServerVersionCalled++;
        return m_version;
    }

    void retrieveCollection(QSharedPointer<RedisClient::ScanCommand>,
                            std::function<void(QVariant)> callback) override
    {
        QVariant resp;

        if (fakeScanCollections.size()) {
           resp = fakeScanCollections.first();
           fakeScanCollections.removeFirst();
        }

        retrieveCollectionCalled++;
        callback(resp);
    }

    void runCommand(const RedisClient::Command &cmd) override
    {
        RedisClient::Response resp;

        if (fakeResponses.size()) {
           resp = fakeResponses.first();
           fakeResponses.removeFirst();
        } else {
           qDebug() << "Unexpected command: "<< cmd.getRawString();
        }

        auto callback = cmd.getCallBack();
        callback(resp);

        runCommandCalled++;        
    }

    uint runCommandCalled;
    uint retrieveCollectionCalled;
    uint getServerVersionCalled;

    QList<QVariant> fakeScanCollections;
    QList<RedisClient::Response> fakeResponses;

    void setFakeResponses(const QStringList& respList)
    {
        for (QString response : respList) {
            RedisClient::Response r(response.toLatin1());
            fakeResponses.push_back(r);
        }
    }

private:
    double m_version;
    bool m_raiseExceptionOnConnect;
};

