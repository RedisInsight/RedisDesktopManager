#include "consoleconnectionwrapper.h"
#include "connection.h"
#include "response.h"
#include "command.h"
#include "commandexecutor.h"

ConsoleConnectionWrapper::ConsoleConnectionWrapper(RedisClient::ConnectionConfig &config)
    : config(config), connectionValid(false)
{
}

void ConsoleConnectionWrapper::init()
{
    if (config.isNull()) 
    {
        emit addOutput("Invalid config. Can't create connection.");        
        return;
    } 

    connection = QSharedPointer<RedisClient::Connection>(new RedisClient::Connection(config, false));

    if (!connection->connect()) 
    {
        emit addOutput("Invalid config. Can't create connection.");    
        return;
    }

    connectionValid = true;

    emit addOutput("Connected.\n");
    emit changePrompt(QString("%1:0>").arg(config.name), true);    
}

void ConsoleConnectionWrapper::executeCommand(const QString & cmd)
{
    if (cmd == "segfault") { //crash
        delete reinterpret_cast<QString*>(0xFEE1DEAD);
        return;
    }

    if (!connectionValid) 
    {
        emit addOutput("Invalid config. Can't create connection.");
        return;
    }

    if (!connection->isConnected() && !connection->connect()) 
    {
        emit addOutput("Connection error. Check network connection");
        return;
    }

    using namespace RedisClient;

    Command command(cmd);
    Response result = CommandExecutor::execute(connection.data(), command);
    
    int dbIndex = 0;

    if (command.isSelectCommand(&dbIndex))
    {        
        emit changePrompt(
            QString("%1:%2>")
                .arg(connection->config.name)
                .arg(dbIndex),
                false
            );
    }
    QVariant value = result.getValue();
    emit addOutput(RedisClient::Response::valueToHumanReadString(value));
}
