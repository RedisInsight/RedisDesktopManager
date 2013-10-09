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
	
	enum ResponseType 
	{
		Status, Error, Integer, Bulk, MultiBulk, Unknown			
	};

	/** @see http://redis.io/topics/protocol for more info **/	
	QString prepareCommand(QString command);

	QVariant parseResponse(QString response);
	QStringList parseMultiBulk(QString response);

	ResponseType getResponseType(QString);	
	ResponseType getResponseType(const QChar);

	QString getStringResponse(QString response);

	bool isFullResponseRecieved(QString r);
	bool isIntReplyValid(QString r);
	bool isBulkReplyValid(QString r);
	bool isMultiBulkReplyValid(QString r);	
	bool waitForData(int ms);

	int getSizeOfBulkReply(QString mb);		
};

