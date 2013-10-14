#include "..\..\include\redis\ConsoleConnectionWrapper.h"

#include "RedisConnectionAbstract.h"
#include "RedisConnection.h"
#include "RedisConnectionOverSsh.h"
#include "consoleTab.h"

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
	QString printableResult;

	if (result.isNull()) 
	{
		printableResult = "NULL";
	} else if (result.type() == QVariant::String || result.type() == QVariant::Int) {
		printableResult = result.toString();
	} else if (result.type() == QVariant::StringList) {
		printableResult = result.toStringList().join("\r\n");
	}
	
	consoleView.output(printableResult);
}
