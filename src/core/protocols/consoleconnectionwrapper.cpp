#include "consoleconnectionwrapper.h"
#include "core/connection.h"
#include "core/response.h"
#include "core/command.h"
#include "core/commandexecutor.h"

ConsoleConnectionWrapper::ConsoleConnectionWrapper(RedisClient::Connection* connection)
    : m_connection(connection)
{
}

void ConsoleConnectionWrapper::init()
{
    if (!m_connection->connect())
    {
        emit addOutput("Connection error. Check network connection", QConsole::Error);
        return;
    }

    emit addOutput("Connected.\n", QConsole::Complete);
    emit changePrompt(QString("%1:0>").arg(m_connection->config.name), true);
}

QString ConsoleConnectionWrapper::getConsoleName()
{
    return m_connection->config.name;
}

void ConsoleConnectionWrapper::executeCommand(const QString & cmd)
{
    if (cmd == "segfault") { //crash
        delete reinterpret_cast<QString*>(0xFEE1DEAD);
        return;
    }

    using namespace RedisClient;

    Command command(cmd);
    Response result = CommandExecutor::execute(m_connection, command);

    if (command.isSelectCommand())
    {        
        emit changePrompt(
            QString("%1:%2>")
                .arg(m_connection->config.name)
                .arg(command.getSplitedRepresentattion().at(1)),
                false
            );
    }
    QVariant value = result.getValue();
    emit addOutput(RedisClient::Response::valueToHumanReadString(value), QConsole::Complete);
}
