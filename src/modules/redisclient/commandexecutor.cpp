#include "commandexecutor.h"
#include "connection.h"
#include "command.h"

RedisClient::Response RedisClient::CommandExecutor::execute(QSharedPointer<Connection> connection, RedisClient::Command &cmd)
{
    return execute(connection.data(), cmd);
}

RedisClient::Response RedisClient::CommandExecutor::execute(Connection* connection, Command& cmd)
{
    if (!connection->waitConnectedState(connection->config.executeTimeout))
        throw Exception("Can not execute command. Connection not established.");

    Executor syncObject(cmd);

    connection->runCommand(cmd);
    return syncObject.waitForResult(connection->config.executeTimeout);
}

RedisClient::Executor::Executor(RedisClient::Command &cmd)
{
    cmd.setOwner(this);
    cmd.setCallBackName("responseReceiver");
    m_timeoutTimer.setSingleShot(true);
    connect(&m_timeoutTimer, SIGNAL(timeout()), &m_loop, SLOT(quit()));
}

RedisClient::Response RedisClient::Executor::waitForResult(unsigned int timeoutInMs)
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
