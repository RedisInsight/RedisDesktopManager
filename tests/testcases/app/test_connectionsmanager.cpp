#include "models/connectionsmanager.h"
#include "test_connectionsmanager.h"
#include <QtTest/QtTest>

void TestConnectionsManager::LoadConnectionsConfigFromFile()
{
	//given
	// xml fixture test-config.xml
    QString configTestFile = "connections.xml";

	//when loads connections
    ConnectionsManager testManager(configTestFile);

	//then
    QCOMPARE(testManager.size(), 1);

}
