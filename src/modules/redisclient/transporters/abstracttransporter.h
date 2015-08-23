#pragma once
#include <QObject>
#include <functional>
#include "redisclient/connection.h"
#include "redisclient/command.h"
#include "redisclient/response.h"

namespace RedisClient {

class Response;

class AbstractTransporter : public QObject
{
    Q_OBJECT
public:
    AbstractTransporter(Connection * c); //TODO: replace raw pointer by WeakPtr
    virtual ~AbstractTransporter();

signals:
    void errorOccurred(const QString&);
    void logEvent(const QString&);
    void connected();
    void commandAdded();

public slots:
    virtual void init() = 0;
    virtual void disconnect() = 0;
    virtual void addCommand(Command);
    virtual void cancelCommands(QObject *);

protected slots:
    void executionTimeout();

protected:
    virtual void runCommand(const Command &cmd) = 0;
    virtual void sendResponse();
    virtual void processCommandQueue();

protected:
    bool m_isInitialized;
    bool m_isCommandRunning;
    Connection * m_connection;
    Command m_runningCommand;
    Response m_response;
    QByteArray m_readingBuffer;
    QQueue<Command> m_commands;
    QSharedPointer<QTimer> m_executionTimer;
};

class ResponseEmitter : public QObject {
    Q_OBJECT
public:
    ResponseEmitter(const Response& r)
        : m_response(r){}

    void sendResponse(QObject* owner, std::function<void(Response)> callback)
    {
        QObject::connect(this, &ResponseEmitter::response, owner, callback, Qt::AutoConnection);

        emit response(m_response);
    }

signals:
    void response(Response);
private:
    const Response& m_response;
};
}
