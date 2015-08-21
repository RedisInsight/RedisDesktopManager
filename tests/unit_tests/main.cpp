#include <QTest>
#include <QApplication>

#include <easylogging++.h>
INITIALIZE_EASYLOGGINGPP

//tests
#include <iostream>
#include "testcases/app/test_connectionsmanager.h"
#include "testcases/app/test_configmanager.h"
#include "testcases/app/test_consoleoperations.h"
#include "testcases/app/test_treeoperations.h"
#include "testcases/app/test_abstractkey.h"
#include "testcases/app/test_keymodels.h"
#include "testcases/app/test_dialogs.h"
#include "testcases/redisclient/test_command.h"
#include "testcases/redisclient/test_response.h"
#include "testcases/redisclient/test_connection.h"
#include "testcases/redisclient/test_ssh.h"
#include "testcases/redisclient/test_config.h"
#include "testcases/connections-tree/test_serveritem.h"
#include "testcases/connections-tree/test_databaseitem.h"
#include "testcases/console/test_console.h"
#include "testcases/value-editor/test_valueview.h"
#include "testcases/value-editor/test_compression.h"
#include "redisclient/redisclient.h"

int main(int argc, char *argv[])
{
	QApplication app( argc, argv );   

    initRedisClient();

    int allTestsResult = 0
            // connections-tree module
            + QTest::qExec(new TestServerItem, argc, argv)
            + QTest::qExec(new TestDatabaseItem, argc, argv)

            // redisclient module
            + QTest::qExec(new TestCommand, argc, argv)
            + QTest::qExec(new TestSsh, argc, argv)
            + QTest::qExec(new TestResponse, argc, argv)
            + QTest::qExec(new TestConnection, argc, argv)
            + QTest::qExec(new TestConfig, argc, argv)

            // console module
            + QTest::qExec(new TestConsole, argc, argv)

            // app
            + QTest::qExec(new TestConnectionsManager, argc, argv)
            + QTest::qExec(new TestConfigManager, argc, argv)
            + QTest::qExec(new TestConsoleOperations, argc, argv)
            + QTest::qExec(new TestTreeOperations, argc, argv)
            + QTest::qExec(new TestKeyModels, argc, argv)
            + QTest::qExec(new TestAbstractKey, argc, argv)
            + QTest::qExec(new TestDialogs, argc, argv)

            // value-editor module
            + QTest::qExec(new TestCompression, argc, argv)
            + QTest::qExec(new TestValueView, argc, argv)
            ;

    if (allTestsResult == 0)
        qDebug() << "[Tests PASS]";
    else
        qDebug() << "[Tests FAIL]";

    return (allTestsResult != 0 )? 1 : 0;
}

