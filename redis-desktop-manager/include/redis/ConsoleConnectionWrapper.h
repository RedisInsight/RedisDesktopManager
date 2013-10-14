#pragma once

#include <QObject>

class RedisConnectionConfig;
class RedisConnectionAbstract;
class consoleTab;

class ConsoleConnectionWrapper : public QObject
{
	Q_OBJECT

public:
	ConsoleConnectionWrapper(RedisConnectionConfig &, consoleTab &);	

	public slots:
		void executeCommand(QString);

private:
	RedisConnectionAbstract * connection;
	consoleTab & consoleView;
	bool connectionValid;	
};

