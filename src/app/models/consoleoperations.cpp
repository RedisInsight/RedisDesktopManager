#include "consoleoperations.h"
#include "modules/redisclient/redisclient.h"

ConsoleModel::ConsoleModel(QSharedPointer<RedisClient::Connection> connection)
    : m_connection(connection)
{
}

void ConsoleModel::init()
{
    if (!m_connection->connect())
    {
        emit addOutput("Connection error. Check network connection", QConsole::Error);
        return;
    }

    emit addOutput("Connected.\n", QConsole::Complete);
    emit changePrompt(QString("%1:0>").arg(m_connection->config.name), true);
}

QString ConsoleModel::getConsoleName()
{
    return m_connection->config.name;
}

void ConsoleModel::executeCommand(const QString & cmd)
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
