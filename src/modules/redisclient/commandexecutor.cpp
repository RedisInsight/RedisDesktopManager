#include "commandexecutor.h"
#include "connection.h"
#include "command.h"

RedisClient::Response RedisClient::CommandExecutor::execute(QSharedPointer<Connection> connection, RedisClient::Command &cmd)
{
    return execute(connection.data(), cmd);
}

RedisClient::Response RedisClient::CommandExecutor::execute(Connection* connection, Command& cmd)
{
    if (!connection->waitConnectedState(connection->config.param<int>("timeout_execute")))
        throw Exception("Cannot execute command. Connection not established.");

    Executor syncObject(cmd);

    try {
        connection->runCommand(cmd);
    } catch (RedisClient::Connection::Exception& e) {
        throw Exception("Cannot execute command." + QString(e.what()));
    }

    return syncObject.waitForResult(connection->config.param<int>("timeout_execute"));
}

RedisClient::Executor::Executor(RedisClient::Command &cmd)
{
    cmd.setCallBack(this, [this](Response r) {
        m_result = r;
        m_loop.exit();
    });

    m_timeoutTimer.setSingleShot(true);
    connect(&m_timeoutTimer, SIGNAL(timeout()), &m_loop, SLOT(quit()));
}

RedisClient::Response RedisClient::Executor::waitForResult(unsigned int timeoutInMs)
{
    m_timeoutTimer.start(timeoutInMs);
    m_loop.exec();
    return m_result;
}
