#include "test_redisconnection.h"
#include "RedisConnection.h"
#include "RedisConnectionConfig.h"
#include "Response.h"
#include "Command.h"
#include <QtTest/QtTest>

void TestRedisConnection::connect()
{
	//given 
	RedisConnectionConfig conf("127.0.0.1");
	RedisConnection conn(conf);	

	//when
	conn.connect();

	//then
	QCOMPARE(conn.connected, true);
}

void TestRedisConnection::execute()
{
	//given 
	RedisConnectionConfig conf("127.0.0.1");
	RedisConnection conn(conf);	

	//when
	conn.connect();
	QVariant actualResult = conn.execute("PING");

	//then
	QCOMPARE(actualResult, QVariant("PONG"));
}

void TestRedisConnection::runCommand()
{
	//given
	RedisConnectionConfig conf("127.0.0.1");
	RedisConnection conn(conf);	
	QEventLoop loop;
	QTimer timer;
	Command cmd(QString("ECHO TEST"), nullptr);

	//when
	conn.connect();

	timer.setSingleShot(true);
	QObject::connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
	QObject::connect(&conn, SIGNAL(responseResived(Response &)), &loop, SLOT(quit()));

	timer.start(5000);
	conn.runCommand(cmd);
	loop.exec();

	QVariant actualResult = conn.getLastResponse().getValue();

	//then
	QCOMPARE(actualResult, QVariant("TEST"));
}




