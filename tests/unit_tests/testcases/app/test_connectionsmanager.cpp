#include <QtTest/QtTest>
#include <QFile>
#include <QModelIndex>
#include "models/connectionsmanager.h"
#include "test_connectionsmanager.h"

TestConnectionsManager::TestConnectionsManager()
    : m_viewModel(new ValueEditor::ViewModel(QSharedPointer<ValueEditor::AbstractKeyFactory>(nullptr)))
{}

void TestConnectionsManager::loadConnectionsConfigFromFile()
{
	//given        
	// xml fixture
    QString configTestFile = "./unit_tests/testcases/app/connections.xml";   

	//when loads connections
    ConnectionsManager testManager(configTestFile, m_tabsWidget, m_viewModel);

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
    QString configTestFile = "/tmp/test_rdm.xml";
    QFile::remove(configTestFile);
    RedisClient::ConnectionConfig connectionConfig = getDummyConfig(connectionName);
    ConnectionsManager testManager(configTestFile, m_tabsWidget, m_viewModel);    

	//when
	// add new connection and save
    testManager.addNewConnection(connectionConfig, true);
    // load everything from scratch
    ConnectionsManager testManagerNew(configTestFile, m_tabsWidget, m_viewModel);
    QModelIndex testIndex = testManagerNew.index(0, 0, QModelIndex());
    QString actualConnectionName = testManagerNew.data(testIndex, Qt::DisplayRole).toString();

    //then
    qDebug() << "Actual name: " << actualConnectionName;  
    QCOMPARE(QFile::exists(configTestFile), true);
    QCOMPARE(actualConnectionName, connectionName);    
}
