#include <QTest>
#include <QApplication>

//tests
#include <iostream>
#include "testcases/app/test_connectionsmanager.h"
#include "testcases/app/test_treeoperations.h"
#include "testcases/app/test_abstractkey.h"
#include "testcases/app/test_keymodels.h"
#include "testcases/redisclient/test_command.h"
#include "testcases/redisclient/test_response.h"
#include "testcases/redisclient/test_connection.h"
#include "testcases/connections-tree/test_serveritem.h"
#include "testcases/connections-tree/test_databaseitem.h"
#include "testcases/console/test_console.h"
#include "testcases/value-editor/test_view.h"
#include "redisclient/redisclient.h"

int main(int argc, char *argv[])
{
	QApplication app( argc, argv );   

    initRedisClient();

	int allTestsResult = 
            // connections-tree module
            QTest::qExec(new TestServerItem, argc, argv)
            + QTest::qExec(new TestDatabaseItem, argc, argv)

            // redisclient module
            + QTest::qExec(new TestCommand, argc, argv)
            + QTest::qExec(new TestResponse, argc, argv)
            + QTest::qExec(new TestConnection, argc, argv)

            // console module
            + QTest::qExec(new TestConsole, argc, argv)

            // app
            + QTest::qExec(new TestConnectionsManager, argc, argv)
            + QTest::qExec(new TestTreeOperations, argc, argv)
            + QTest::qExec(new TestKeyModels, argc, argv)
            + QTest::qExec(new TestAbstractKey, argc, argv)

            // value-editor module
            + QTest::qExec(new TestView, argc, argv)
            ;

    if (allTestsResult == 0)
        qDebug() << "[Tests PASS]";
    else
        qDebug() << "[Tests FAIL]";

    return (allTestsResult != 0 )? 1 : 0;
}

