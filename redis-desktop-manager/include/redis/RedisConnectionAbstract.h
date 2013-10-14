#pragma once

#include "RedisConnectionConfig.h"
#include "RedisException.h"
#include <QtCore>
#include <QMap>
#include <QList>

class TestRedisConnection;

class RedisConnectionAbstract
{
	friend class TestRedisConnection;

public:

	RedisConnectionConfig config;	

	RedisConnectionAbstract(const RedisConnectionConfig & c) : config(c), connected(false)  {};

	virtual ~RedisConnectionAbstract() {};

	virtual bool connect() = 0;

	virtual QString getLastError() = 0;

	virtual QVariant execute(QString) = 0;

	virtual bool isConnected();
	
	typedef QMap <QString, int> RedisDatabases;
	RedisDatabases getDatabases();

	void selectDb(int dbIndex);

	QStringList getKeys(QString pattern = "*");

protected:
	bool connected;

};

