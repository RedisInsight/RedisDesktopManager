#ifndef REDISCONNECTION_H
#define REDISCONNECTION_H

#include <QtNetwork>
#include <QTcpSocket>
#include "RedisConnectionAbstract.h"

class Test_RedisConnection;

class RedisConnection : public RedisConnectionAbstract
{	
    friend class Test_RedisConnection;
public:
	RedisConnection(const RedisConnectionConfig &);

	~RedisConnection();

	bool connect();
	
	QString getLastError();

	QVariant execute(QString);

private:		
	QTcpSocket * socket;	
};

#endif // REDISCONNECTION_H
