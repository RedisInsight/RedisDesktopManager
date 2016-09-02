#pragma once
#include "basetestcase.h"
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
};
