#include "test_redisconnection.h"
#include "redisconnection.h"
#include "RedisConnectionConfig.h"
#include <QtTest/QtTest>

void TestRedisConnection::parseResponse()
{
	//given 
	RedisConnectionConfig conf("");
	RedisConnection conn(conf);
	QFETCH(QString, testResponse);
	QFETCH(QVariant, validResult);

	//when
	QVariant actualResult = conn.parseResponse(testResponse);

	//then
	QCOMPARE(actualResult, validResult);
}

void TestRedisConnection::parseResponse_data()
{
	QTest::addColumn<QString>("testResponse");
	QTest::addColumn<QVariant>("validResult");

	QTest::newRow("Status")		<< "+OK\r\n"							<< QVariant(QString("OK"));	
	QTest::newRow("Error")		<< "-ERR unknown command 'foobar'\r\n"	<< QVariant(QString("ERR unknown command 'foobar'"));	
	QTest::newRow("Integer")	<< ":99998\r\n"							<< QVariant(99998);	
	QTest::newRow("Bulk")		<< "$6\r\nfoobar\r\n"					<< QVariant("foobar");
	QTest::newRow("Null Bulk")	<< "$-1\r\n"							<< QVariant();
	QTest::newRow("Multi Bulk")	<< "*3\r\n:1\r\n:2\r\n$6\r\nfoobar\r\n"	<< QVariant(QStringList() << "1" << "2" << "foobar");
}


void TestRedisConnection::prepareCommand()
{
	//given 
	RedisConnectionConfig conf("");
	RedisConnection conn(conf);	

	//when
	QVariant actualResult = conn.prepareCommand("EXISTS testkey:test");

	//then
	QCOMPARE(actualResult, QVariant("*2\r\n$6\r\nEXISTS\r\n$12\r\ntestkey:test\r\n"));
}


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


void TestRedisConnection::isIntResponseValid()
{
	//given
	RedisConnectionConfig conf("");
	RedisConnection conn(conf);	

	//when
	bool actualOnValid = conn.isIntReplyValid(":10000\r\n");
	bool actualOnInvalid = conn.isIntReplyValid(":99\n");
	bool actualOnInvalid2 = conn.isIntReplyValid(":");
	bool actualOnInvalid3 = conn.isIntReplyValid("");

	//then
	QCOMPARE(actualOnValid, true);
	QCOMPARE(actualOnInvalid, false);
	QCOMPARE(actualOnInvalid2, false);
	QCOMPARE(actualOnInvalid3, false);
}

void TestRedisConnection::isBulkReplyValid()
{
	//given
	RedisConnectionConfig conf("");
	RedisConnection conn(conf);	

	//when
	bool actualOnValid = conn.isBulkReplyValid("$6\r\nfoobar\r\n");
	bool actualOnValid2 = conn.isBulkReplyValid("$-1\r\n");
	bool actualOnValid3 = conn.isBulkReplyValid("$12\r\n# Keyspace\r\n\r\n");
	bool actualOnInvalid = conn.isBulkReplyValid("$1\r\n");
	bool actualOnInvalid2 = conn.isBulkReplyValid("$5\r\n\r\n");
	bool actualOnInvalid3 = conn.isBulkReplyValid("$5\r\nhell\r\n");
	bool actualOnInvalid4 = conn.isBulkReplyValid("$5\r\n");

	//then
	QCOMPARE(actualOnValid, true);
	QCOMPARE(actualOnValid2, true);
	QCOMPARE(actualOnValid3, true);
	QCOMPARE(actualOnInvalid, false);
	QCOMPARE(actualOnInvalid2, false);
	QCOMPARE(actualOnInvalid3, false);
	QCOMPARE(actualOnInvalid4, false);
}

void TestRedisConnection::isMultiBulkReplyValid()
{
	//given
	RedisConnectionConfig conf("");
	RedisConnection conn(conf);	

	//when
	bool actualOnValid = conn.isMultiBulkReplyValid("*5\r\n:1\r\n:2\r\n:3\r\n:4\r\n$6\r\nfoobar\r\n");
	bool actualOnValid2 = conn.isMultiBulkReplyValid("*4\r\n$3\r\nfoo\r\n$3\r\nbar\r\n$5\r\nHello\r\n$5\r\nWorld\r\n");
	bool actualOnInvalid = conn.isMultiBulkReplyValid("");
	bool actualOnInvalid2 = conn.isMultiBulkReplyValid("*5\r\n");
	bool actualOnInvalid3 = conn.isMultiBulkReplyValid("*5\r\n:1\r\n");
	bool actualOnInvalid4 = conn.isMultiBulkReplyValid("*2\r\n:1\r\n$6\r\nHello\r\n");

	//then
	QCOMPARE(actualOnValid, true);
	QCOMPARE(actualOnValid2, true);
	QCOMPARE(actualOnInvalid, false);
	QCOMPARE(actualOnInvalid2, false);
	QCOMPARE(actualOnInvalid3, false);
	QCOMPARE(actualOnInvalid4, false);
}


