#include <QtTest/QtTest>
#include <QFile>
#include <QDir>
#include <QModelIndex>
#include "models/connectionsmanager.h"
#include "test_connectionsmanager.h"

TestConnectionsManager::TestConnectionsManager()    
{}

void TestConnectionsManager::loadConnectionsConfigFromFile()
{
	//given        
	// xml fixture
    QString configTestFile = "./unit_tests/testcases/app/connections.json";

	//when loads connections
    ConnectionsManager testManager(configTestFile);

	//then
    QCOMPARE(testManager.size(), 1);
}

void TestConnectionsManager::saveConnectionsConfigToFile_data()
{
    QTest::addColumn<QString>("connectionName");

    QTest::newRow("simple") << "test";
    QTest::newRow("unicode") << "❤❤❤༆";
}

void TestConnectionsManager::saveConnectionsConfigToFile()
{
    //given
    QFETCH(QString, connectionName);
    QString configTestFile = QString("%1/test_rdm.json").arg(QDir::tempPath());
    QFile::remove(configTestFile);
    RedisClient::ConnectionConfig connectionConfig = getDummyConfig(connectionName);
    ConnectionsManager testManager(configTestFile);

	//when
	// add new connection and save
    testManager.addNewConnection(connectionConfig, true);
    // load everything from scratch
    ConnectionsManager testManagerNew(configTestFile);
    QModelIndex testIndex = testManagerNew.index(0, 0, QModelIndex());
    QString actualConnectionName = testManagerNew.data(testIndex, ConnectionsTree::Model::itemName).toString();

    //then
    qDebug() << "Actual name: " << actualConnectionName;  
    QCOMPARE(QFile::exists(configTestFile), true);
    QCOMPARE(actualConnectionName, connectionName);    
}
