#include "RedisConnectionsManager.h"
#include "test_redisconnectionsmanager.h"
#include <QtTest/QtTest>

void TestRedisConnectionsManager::LoadConnectionsConfigFromFile()
{
	//given
	// xml fixture test-config.xml
	QString configTestFile = "test-config.xml";

	//when loads connections
	RedisConnectionsManager testManager(configTestFile);	

	//then
	QCOMPARE(testManager.connections.size(), 1);

}

#include "moc_test_redisconnectionsmanager.cpp"