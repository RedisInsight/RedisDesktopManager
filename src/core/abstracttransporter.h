#ifndef ABSTRACTTRANSPORTER_H
#define ABSTRACTTRANSPORTER_H

#include <QObject>
#include "connection.h"
#include "command.h"
#include "response.h"

namespace RedisClient {

class AbstractTransporter : public QObject
{
    Q_OBJECT
public:
    AbstractTransporter(Connection * c);
    virtual ~AbstractTransporter();

signals:
    void errorOccurred(const QString&);
    void logEvent(const QString&);
    void connected();
    void commandAdded();

public slots:
    virtual void init() = 0;
    virtual void disconnect() = 0;
    virtual void addCommand(const Command&);
    virtual void cancelCommands(QObject *);

protected:
    bool m_isInitialized;
    bool m_isCommandRunning;
    Connection * m_connection;
    Command runningCommand;
    Response m_response;
    QByteArray readingBuffer;
    QQueue<Command> commands;
    QSharedPointer<QTimer> executionTimer;

    virtual void runCommand(const Command &cmd) = 0;
    virtual void sendResponse();
    virtual void processCommandQueue();
    void sendProgressValue();

protected slots:
    void executionTimeout();
};
}
#endif // ABSTRACTTRANSPORTER_H
