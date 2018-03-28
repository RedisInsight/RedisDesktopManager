#pragma once
#include "basetestcase.h"
#include "value-editor/tabsmodel.h"

class TestConnectionsManager : public BaseTestCase
{
	Q_OBJECT
	
public:
    TestConnectionsManager();

private slots:
	void loadConnectionsConfigFromFile();
	
	void saveConnectionsConfigToFile();
    void saveConnectionsConfigToFile_data();
};
