#include "consolemodel.h"
#include <qredisclient/redisclient.h>

using namespace Console;

Model::Model(QSharedPointer<RedisClient::Connection> connection)
    : TabModel(connection), m_current_db(0)
{    
}

void Model::init()
{
    try {
        if (!m_connection->connect())
        {
            emit addOutput("Connection error. Check network connection", "error");
            return;
        }
    } catch (RedisClient::Connection::Exception&) {
        emit addOutput("Invalid Connection. Check connection settings.", "error");
        return;
    }

    emit addOutput("Connected.\n", "complete");
    emit changePrompt(QString("%1:0>").arg(m_connection->getConfig().name()), true);
}

QString Model::getName() const
{
    return m_connection->getConfig().name();
}

void Model::executeCommand(const QString & cmd)
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
        emit addOutput(QString("Connection error:") + QString(e.what()), "error");
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
    emit addOutput(RedisClient::Response::valueToHumanReadString(value), "complete");
}
