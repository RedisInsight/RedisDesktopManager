#include "commandexecutor.h"
#include "connection.h"
#include "Command.h"

Response RedisClient::CommandExecutor::execute(RedisClient::Connection *connection, Command &cmd)
{
    if (!connection->waitConnectedState(connection->config.executeTimeout))
        throw ConnectionExeption("Can not execute command. Connection not established.");

    Executor syncObject(cmd);

    connection->runCommand(cmd);
    return syncObject.waitForResult(connection->config.executeTimeout);
}

RedisClient::Executor::Executor(Command &cmd)
{
    cmd.setOwner(this);
    cmd.setCallBackName("responseReceiver");
    m_timeoutTimer.setSingleShot(true);
    connect(&m_timeoutTimer, SIGNAL(timeout()), &m_loop, SLOT(quit()));
}

Response RedisClient::Executor::waitForResult(unsigned int timeoutInMs)
{
    m_timeoutTimer.start(timeoutInMs);
    m_loop.exec();
    return m_result;
}

void RedisClient::Executor::responseReceiver(Response r)
{
    m_result = r;
    m_loop.exit();
}
