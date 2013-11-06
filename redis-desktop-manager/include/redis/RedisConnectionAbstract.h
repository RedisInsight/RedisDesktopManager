#pragma once

#include "RedisConnectionConfig.h"
#include "RedisException.h"
#include "Response.h"
#include <QtCore>
#include <QMap>
#include <QList>

class TestRedisConnection;

class RedisConnectionAbstract : public QObject
{
	Q_OBJECT

	friend class TestRedisConnection;
public:

	RedisConnectionConfig config;	

	RedisConnectionAbstract(const RedisConnectionConfig & c); 		
	virtual ~RedisConnectionAbstract() {};

	/** Interface for all connection classes **/

	virtual bool connect() = 0;
	virtual QString getLastError() = 0;
	virtual QVariant execute(QString) = 0;
	virtual bool isConnected();
	virtual void runCommand(const QString &cmd) = 0;
	virtual Response getLastResponse();	
	virtual void sendResponse();

	/** 
	 * Get list of databases with keys counters 
	 **/
	typedef QMap <QString, int> RedisDatabases;
	RedisDatabases getDatabases();

	/** 
	 * Select db 
	 * see  http://redis.io/commands/select 
	 */
	void selectDb(int dbIndex);

	/** 
	 * Get keys list from db
	 * see http://redis.io/commands/keys
	 **/	
	QStringList getKeys(QString pattern = "*");	

signals:
	void responseResived(Response &);

protected:
	bool connected;
	QTimer executionTimer;
	Response resp;
	bool commandRunning;
	QString runningCommand;

protected slots:
	void executionTimeout();
};

