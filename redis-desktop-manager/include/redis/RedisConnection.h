#ifndef REDISCONNECTION_H
#define REDISCONNECTION_H

#include <QtNetwork>
#include <QTcpSocket>
#include "RedisConnectionAbstract.h"
#include "Response.h"
#include <QQueue>

class Test_RedisConnection;
class Command;

class RedisConnection : public RedisConnectionAbstract
{	
	Q_OBJECT

    friend class Test_RedisConnection;
public:
	RedisConnection(const RedisConnectionConfig &);

	~RedisConnection();

	bool connect();
	
	QString getLastError();

	QVariant execute(QString);	

	void runCommand(const Command &cmd);

protected:
	void init();

private:
	QTcpSocket * socket;	
	QByteArray readingBuffer;	

private slots:
	void readyRead();
	void error(QAbstractSocket::SocketError error);	

};

#endif // REDISCONNECTION_H
