#pragma once
#include "basetestcase.h"
#include "app/widgets/consoletabs.h"
#include "value-editor/viewmodel.h"

class TestConnectionsManager : public BaseTestCase
{
	Q_OBJECT
	
public:
    TestConnectionsManager();

private slots:
	void loadConnectionsConfigFromFile();
	
	void saveConnectionsConfigToFile();
    void saveConnectionsConfigToFile_data();
	
private:
    ConsoleTabs m_tabsWidget;
    QSharedPointer<ValueEditor::ViewModel> m_viewModel;	
};
