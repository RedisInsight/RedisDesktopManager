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

    Response result = CommandExecutor::execute(connection.data(), Command(cmd));
    
    QRegExp selectDbRegex("^( )*select( )+(\\d)+");

    bool isSelectCommand = selectDbRegex.indexIn(cmd) > -1;

    if (isSelectCommand) 
    {        
        emit changePrompt(
            QString("%1:%2>")
                .arg(connection->config.name)
                .arg(selectDbRegex.cap(3)),
                false
            );
    }

    emit addOutput(RedisClient::Response::valueToHumanReadString(result.getValue()));
}
