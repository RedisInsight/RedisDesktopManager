#pragma once

#include "redisclient/command.h"
#include "redisclient/transporters/abstracttransporter.h"

class DummyTransporter : public RedisClient::AbstractTransporter
{
    Q_OBJECT
public:
    DummyTransporter(RedisClient::Connection* c)
        : RedisClient::AbstractTransporter(c),
          initCalls(0),
          disconnectCalls(0),
          addCommandCalls(0),
          cancelCommandsCalls(0)
    {

    }

    int initCalls;
    int disconnectCalls;
    int addCommandCalls;
    int cancelCommandsCalls;

    QList<RedisClient::Command> executedCommands;
    QList<RedisClient::Response> fakeResponses;

public slots:
    virtual void addCommand(const RedisClient::Command& cmd) override
    {
        addCommandCalls++;
        RedisClient::AbstractTransporter::addCommand(cmd);
    }

    void init() override { initCalls++; emit connected(); }
    virtual void disconnect() { disconnectCalls++; }
    virtual void cancelCommands(QObject *) override { cancelCommandsCalls++; }

protected:
    virtual void runCommand(const RedisClient::Command &cmd) override
    {
        executedCommands.push_back(cmd);

        if (fakeResponses.size() > 0) {
            runningCommand = cmd;
            m_response = fakeResponses.first();
            fakeResponses.removeFirst();
            sendResponse();
            return;
        }
    }
};
