#ifndef COMMANDEXECUTOR_H
#define COMMANDEXECUTOR_H

#include <QObject>
#include "response.h"
#include "exception.h"

namespace RedisClient {

class Connection;
class Command;

class CommandExecutor
{    
    ADD_EXCEPTION

public:
    static Response execute(QSharedPointer<Connection>, Command& cmd);
    static Response execute(Connection*, Command& cmd);
};

class Executor : public QObject
{
    Q_OBJECT
    friend class CommandExecutor;
private slots:
    void responseReceiver(RedisClient::Response);
private:
    Executor(Command& cmd);
    Response waitForResult(unsigned int);
    Response m_result;
    QEventLoop m_loop;
    QTimer m_timeoutTimer;
};

}
#endif // COMMANDEXECUTOR_H
