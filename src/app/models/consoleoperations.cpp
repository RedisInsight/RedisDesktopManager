#include "consoleoperations.h"
#include "app/models/connectionconf.h"
#include <qredisclient/redisclient.h>

ConsoleModel::ConsoleModel(QSharedPointer<RedisClient::Connection> connection)
    : m_connection(connection), m_current_db(0)
{
}

void ConsoleModel::init()
{
    try {
        if (!m_connection->connect())
        {
            emit addOutput("Connection error. Check network connection", QConsole::Error);
            return;
        }
    } catch (RedisClient::Connection::Exception&) {
        emit addOutput("Invalid Connection. Check connection settings.", QConsole::Error);
        return;
    }

    emit addOutput("Connected.\n", QConsole::Complete);
    emit changePrompt(QString("%1:0>").arg(m_connection->getConfig().name()), true);
}

QString ConsoleModel::getConsoleName()
{
    return m_connection->getConfig().name();
}

void ConsoleModel::executeCommand(const QString & cmd)
{
    if (cmd == "segfault") { //crash
        delete reinterpret_cast<QString*>(0xFEE1DEAD);
        return;
    }

    using namespace RedisClient;

    Command command(Command::splitCommandString(cmd), m_current_db);
    Response result;

    try {
        result = m_connection->commandSync(command);
    } catch (Connection::Exception& e) {
        emit addOutput(QString("Connection error:") + QString(e.what()), QConsole::Error);
        return;
    }

    if (command.isSelectCommand())
    {        
        m_current_db = command.getPartAsString(1).toInt();
        emit changePrompt(
            QString("%1:%2>")
                .arg(m_connection->getConfig().name())
                .arg(command.getPartAsString(1)),
                false
            );
    }
    QVariant value = result.getValue();
    emit addOutput(RedisClient::Response::valueToHumanReadString(value), QConsole::Complete);
}
