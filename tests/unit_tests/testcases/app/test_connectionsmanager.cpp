#include <QtTest/QtTest>
#include "models/connectionsmanager.h"
#include "test_connectionsmanager.h"
#include "app/widgets/consoletabs.h"

namespace ValueEditor {
    class ViewModel;
}

void TestConnectionsManager::LoadConnectionsConfigFromFile()
{
	//given    
    QSKIP("FIXME");
	// xml fixture test-config.xml
    QString configTestFile = "connections.xml";
    ConsoleTabs tabsWidget;

	//when loads connections
    ConnectionsManager testManager(configTestFile, tabsWidget, QSharedPointer<ValueEditor::ViewModel>());

	//then
    QCOMPARE(testManager.size(), 1);

}
