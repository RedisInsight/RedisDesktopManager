#include <QTest>
#include <QApplication>

#include <easylogging++.h>
INITIALIZE_EASYLOGGINGPP

//tests
#include <iostream>
#include <qredisclient/redisclient.h>
#include "testcases/app/test_connectionsmanager.h"
#include "testcases/app/test_configmanager.h"
#include "testcases/app/test_treeoperations.h"
#include "testcases/app/test_abstractkey.h"
#include "testcases/app/test_keymodels.h"
#include "testcases/connections-tree/test_serveritem.h"
#include "testcases/connections-tree/test_databaseitem.h"
#include "testcases/connections-tree/test_model.h"
#include "testcases/console/test_console.h"
#include "testcases/console/test_consolemodel.h"
#include "testcases/value-editor/test_viewmodel.h"

int main(int argc, char *argv[])
{
	QApplication app( argc, argv );   

    initRedisClient();

    int allTestsResult = 0
            // connections-tree module
            + QTest::qExec(new TestServerItem, argc, argv)
            + QTest::qExec(new TestDatabaseItem, argc, argv)
            + QTest::qExec(new TestModel, argc, argv)

            // console module
            + QTest::qExec(new TestConsole, argc, argv)

            // app
            + QTest::qExec(new TestConnectionsManager, argc, argv)
            + QTest::qExec(new TestConfigManager, argc, argv)
            + QTest::qExec(new TestConsoleOperations, argc, argv)
            + QTest::qExec(new TestTreeOperations, argc, argv)
            + QTest::qExec(new TestKeyModels, argc, argv)
            + QTest::qExec(new TestAbstractKey, argc, argv)            

            // value-editor module            
            + QTest::qExec(new TestViewModel, argc, argv)
            ;

    if (allTestsResult == 0)
        qDebug() << "[Tests PASS]";
    else
        qDebug() << "[Tests FAIL]";

    return (allTestsResult != 0 )? 1 : 0;
}

