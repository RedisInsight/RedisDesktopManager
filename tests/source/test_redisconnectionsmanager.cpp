#include "RedisConnectionsManager.h"
#include "test_redisconnectionsmanager.h"
#include <QtTest/QtTest>

void TestRedisConnectionsManager::LoadConnectionsConfigFromFile()
{
	//given
	// xml fixture test-config.xml
	QString configTestFile = "stubs\\connections.xml";

	//when loads connections
	RedisConnectionsManager testManager(configTestFile);	

	//then
	QCOMPARE(testManager.connections.size(), 1);

}