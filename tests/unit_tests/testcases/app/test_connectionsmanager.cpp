#include <QtTest/QtTest>
#include "models/connectionsmanager.h"
#include "test_connectionsmanager.h"
#include "app/widgets/consoletabs.h"
#include "value-editor/viewmodel.h"

void TestConnectionsManager::LoadConnectionsConfigFromFile()
{
	//given        
	// xml fixture test-config.xml
    QString configTestFile = "./unit_tests/testcases/app/connections.xml";
    ConsoleTabs tabsWidget;
    QSharedPointer<ValueEditor::ViewModel> viewModel(
                new ValueEditor::ViewModel(QSharedPointer<ValueEditor::AbstractKeyFactory>(nullptr)));

	//when loads connections
    ConnectionsManager testManager(configTestFile, tabsWidget, viewModel);

	//then
    QCOMPARE(testManager.size(), 1);

}
