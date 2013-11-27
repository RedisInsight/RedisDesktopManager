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
		void executeCommand(const QString &);

	signals:
		void changePrompt(const QString &);
		void addOutput(const QString &);

private:
	RedisConnectionAbstract * connection;
	RedisConnectionConfig config;
	bool connectionValid;	
};

