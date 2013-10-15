#include "ConsoleConnectionWrapper.h"

#include "RedisConnectionAbstract.h"
#include "RedisConnection.h"
#include "RedisConnectionOverSsh.h"
#include "consoleTab.h"
#include "Response.h"

ConsoleConnectionWrapper::ConsoleConnectionWrapper(RedisConnectionConfig &config, consoleTab &view)
	: consoleView(view) , connectionValid(false)
{
	if (config.isNull()) 
	{
		consoleView.output("Invalid config. Can't create connection.");
		return;
	} 

	connection =  (config.useSshTunnel()) ? (RedisConnectionAbstract *) new RedisConnectionOverSsh(config) 
										  : (RedisConnectionAbstract *) new RedisConnection(config);

	if (!connection->connect()) 
	{
		consoleView.output("Invalid config. Can't create connection.");
		return;
	}

	connectionValid = true;
	consoleView.setPrompt(QString("%1:0>").arg(config.name));
	consoleView.output("Connected.");	
}

void ConsoleConnectionWrapper::executeCommand(QString cmd)
{
	if (!connectionValid) 
	{
		consoleView.output("Invalid config. Can't create connection.");
		return;
	}

	if (!connection->isConnected() && !connection->connect()) 
	{
		consoleView.output("Connection error. Check network connection");
		return;
	}

	QVariant result = connection->execute(cmd);
	
	QRegExp selectDbRegex("^( )*select( )+(\\d)+");

	bool isSelectCommand = selectDbRegex.indexIn(cmd) > -1;

	if (isSelectCommand) 
	{		
		consoleView.setPrompt(
			QString("%1:%2>")
				.arg(connection->config.name)
				.arg(selectDbRegex.cap(3))
			);
	}

	consoleView.output(Response::valueToString(result));
}
