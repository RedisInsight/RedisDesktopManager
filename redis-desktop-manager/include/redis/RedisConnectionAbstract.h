#pragma once

#include <QObject>
#include <QQueue>
#include <QMap>

#include "RedisConnectionConfig.h"
#include "RedisException.h"
#include "Response.h"
#include "Command.h"

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

	virtual void runCommand(const Command &cmd) = 0;

	virtual Response getLastResponse();	

	/** 
	 * Get list of databases with keys counters 
	 * @emit databesesLoaded
	 **/
	typedef QMap <QString, int> RedisDatabases;

	/** 
	 * Select db 
	 * see  http://redis.io/commands/select 
	 */
	void selectDb(int dbIndex);

	/** 
	 * Get keys list from db
	 * see http://redis.io/commands/keys
	 *  @emit keysLoaded
	 **/	
	void getKeys(QString pattern = "*");	

	static RedisConnectionAbstract * createConnection(const RedisConnectionConfig & c);

public slots:
	void init();
	void addCommand(const Command&);
	void getDatabases();

signals:
	void responseResived(QVariant &, QObject *);
	void databesesLoaded(RedisConnectionAbstract::RedisDatabases&);
	void keysLoaded(QStringList&);

protected:
	bool connected;
	QTimer executionTimer;
	Response resp;
	bool commandRunning;
	bool keysLoadingRunning;
	Command runningCommand;
	QQueue<Command> commands;

	virtual void sendResponse();
	void processCommandQueue();

protected slots:
	void executionTimeout();
};

