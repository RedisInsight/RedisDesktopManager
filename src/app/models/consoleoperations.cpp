#include "consoleoperations.h"
#include "modules/redisclient/redisclient.h"

ConsoleModel::ConsoleModel(QSharedPointer<RedisClient::Connection> connection)
    : m_connection(connection), m_current_db(0)
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
    emit changePrompt(QString("%1:0>").arg(m_connection->config.param<QString>("name")), true);
}

QString ConsoleModel::getConsoleName()
{
    return m_connection->config.param<QString>("name");
}

void ConsoleModel::executeCommand(const QString & cmd)
{
    if (cmd == "segfault") { //crash
        delete reinterpret_cast<QString*>(0xFEE1DEAD);
        return;
    }

    using namespace RedisClient;

    Command command(cmd, nullptr, m_current_db);
    Response result;

    try {
        result = CommandExecutor::execute(m_connection, command);
    } catch (CommandExecutor::Exception& e) {
        emit addOutput(QString("Connection error:") + QString(e.what()), QConsole::Error);
        return;
    }

    if (command.isSelectCommand())
    {        
        m_current_db = command.getPartAsString(1).toInt();
        emit changePrompt(
            QString("%1:%2>")
                .arg(m_connection->config.param<QString>("name"))
                .arg(command.getPartAsString(1)),
                false
            );
    }
    QVariant value = result.getValue();
    emit addOutput(RedisClient::Response::valueToHumanReadString(value), QConsole::Complete);
}
