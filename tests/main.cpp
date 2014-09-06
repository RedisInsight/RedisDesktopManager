#include <QTest>
#include <QApplication>

//tests
#include <iostream>
#include "testcases/app/test_connectionsmanager.h"
#include "testcases/app/test_treeoperations.h"
#include "testcases/redisclient/test_command.h"
#include "testcases/redisclient/test_response.h"
#include "testcases/redisclient/test_connection.h"
#include "testcases/connections-tree/test_serveritem.h"
#include "testcases/connections-tree/test_databaseitem.h"
#include "testcases/console/test_console.h"

int main(int argc, char *argv[])
{
	QApplication app( argc, argv );   

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
            ;

    if (allTestsResult == 0)
        qDebug() << "[Tests PASS]";

    return (allTestsResult != 0 )? 1 : 0;
}

