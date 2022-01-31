#pragma once
#include "respbasetestcase.h"
#include "value-editor/tabsmodel.h"

class TestConnectionsManager : public RESPBaseTestCase
{
	Q_OBJECT
	
public:
    TestConnectionsManager();

private slots:
	void loadConnectionsConfigFromFile();
	
	void saveConnectionsConfigToFile();
    void saveConnectionsConfigToFile_data();
};
