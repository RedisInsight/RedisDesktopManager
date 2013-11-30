#pragma once


#include "qconsole.h"
#include <QThread>

class RedisConnectionConfig;
class ConsoleConnectionWrapper;

class consoleTab: public QConsole
{
	Q_OBJECT

public:
	consoleTab(RedisConnectionConfig&);
	~consoleTab(void);	

public slots:
	void setPrompt(const QString &, bool);	

private:	
	ConsoleConnectionWrapper * connection;
	QThread connectionThread;	
};

