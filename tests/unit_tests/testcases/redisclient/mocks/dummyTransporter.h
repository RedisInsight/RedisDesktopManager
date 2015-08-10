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

    void setFakeResponses(const QStringList& respList)
    {
        for (QString response : respList) {
            RedisClient::Response r(response.toLatin1());
            fakeResponses.push_back(r);
        }
    }

    QList<RedisClient::Command> executedCommands;
    QList<RedisClient::Response> fakeResponses;

public slots:
    virtual void addCommand(RedisClient::Command cmd) override
    {
        addCommandCalls++;
        RedisClient::AbstractTransporter::addCommand(cmd);
    }

    void init() override
    {
        initCalls++;

        // Init command tested after socket connection
        RedisClient::Response info("+FAKE_SERVER_INFO\r\n");
        fakeResponses.push_front(info);

        RedisClient::Response r("+PONG\r\n");
        fakeResponses.push_front(r);

        qDebug() << "Transporter connect: OK";
        emit connected();
    }
    virtual void disconnect() { disconnectCalls++; }
    virtual void cancelCommands(QObject *) override { cancelCommandsCalls++; }

protected:
    virtual void runCommand(const RedisClient::Command &cmd) override
    {
        executedCommands.push_back(cmd);        
        runningCommand = cmd;

        if (fakeResponses.size() > 0) {            
            m_response = fakeResponses.first();
            fakeResponses.removeFirst();                        
        } else {
            qDebug() << "Unexpected command: "<< cmd.getRawString();
            qDebug() << "Previous commands:";
            for (auto cmd : executedCommands) {
                qDebug() << "\t" << cmd.getRawString();
            }
            m_response = RedisClient::Response();
        }

        sendResponse();
    }
};
