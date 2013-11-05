#include "ConsoleConnectionWrapper.h"

#include "RedisConnectionAbstract.h"
#include "RedisConnection.h"
#include "RedisConnectionOverSsh.h"
#include "consoleTab.h"
#include "Response.h"

ConsoleConnectionWrapper::ConsoleConnectionWrapper(RedisConnectionConfig &config)
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

	connection =  (config.useSshTunnel()) ? (RedisConnectionAbstract *) new RedisConnectionOverSsh(config) 
		: (RedisConnectionAbstract *) new RedisConnection(config);

	if (!connection->connect()) 
	{
		emit addOutput("Invalid config. Can't create connection.");	
		return;
	}

	connectionValid = true;

	emit changePrompt(QString("%1:0>").arg(config.name));
	emit addOutput("Connected.");	
}

void ConsoleConnectionWrapper::executeCommand(QString cmd)
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

	QVariant result = connection->execute(cmd);
	
	QRegExp selectDbRegex("^( )*select( )+(\\d)+");

	bool isSelectCommand = selectDbRegex.indexIn(cmd) > -1;

	if (isSelectCommand) 
	{		
		emit changePrompt(
			QString("%1:%2>")
				.arg(connection->config.name)
				.arg(selectDbRegex.cap(3))
			);
	}

	addOutput(Response::valueToString(result));
}
