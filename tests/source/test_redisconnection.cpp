#include "test_redisconnection.h"
#include "RedisConnection.h"
#include "RedisConnectionConfig.h"
#include "Response.h"
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

	//when
	conn.connect();

	timer.setSingleShot(true);
	QObject::connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
	QObject::connect(&conn, SIGNAL(responseResived(Response &)), &loop, SLOT(quit()));

	timer.start(5000);
	conn.runCommand("ECHO TEST");
	loop.exec();

	QVariant actualResult = conn.getLastResponse().getValue();

	//then
	QCOMPARE(actualResult, QVariant("TEST"));
}


void TestRedisConnection::getDatabases()
{
	QFAIL("refactor test");

	//given
	RedisConnectionConfig conf("127.0.0.1");
	RedisConnection conn(conf);	

	//when 
	conn.connect();
	//QMap <QString, int> actualServers = conn.getDatabases();

	//then
	//QCOMPARE(actualServers.size(), 100); // default db count for redis 2.6
}




