#pragma once

#include <QObject>
#include "RedisConnectionConfig.h"

class RedisConnectionAbstract;

class ConsoleConnectionWrapper : public QObject
{
	Q_OBJECT

public:
	ConsoleConnectionWrapper(RedisConnectionConfig &);	

	public slots:
		void init();
		void executeCommand(QString);

	signals:
		void changePrompt(QString);
		void addOutput(QString);

private:
	RedisConnectionAbstract * connection;
	RedisConnectionConfig config;
	bool connectionValid;	
};

