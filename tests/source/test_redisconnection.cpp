#include "test_redisconnection.h"
#include "RedisConnection.h"
#include "RedisConnectionConfig.h"
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

void TestRedisConnection::getDatabases()
{
	//given
	RedisConnectionConfig conf("127.0.0.1");
	RedisConnection conn(conf);	

	//when 
	conn.connect();
	QMap <QString, int> actualServers = conn.getDatabases();

	//then
	QCOMPARE(actualServers.size(), 16); // default db count for redis 2.6
}




