#ifndef COMMANDEXECUTOR_H
#define COMMANDEXECUTOR_H

#include <QObject>
#include "Response.h"

class Command;

namespace RedisClient {

class Connection;

class CommandExecutor
{    
public:
    static Response execute(Connection *, Command& cmd);
};

class Executor : public QObject
{
    Q_OBJECT
    friend class CommandExecutor;
private slots:
    void responseReceiver(Response);    
private:
    Executor(Command& cmd);
    Response waitForResult(unsigned int);
    Response m_result;
    QEventLoop m_loop;
    QTimer m_timeoutTimer;
};

}
#endif // COMMANDEXECUTOR_H
