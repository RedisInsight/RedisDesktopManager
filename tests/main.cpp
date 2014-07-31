#include <QTest>
#include <QApplication>

//tests
#include <iostream>
#include "test_connectionsmanager.h"
#include "test_command.h"
#include "test_response.h"
#include "test_connection.h"
#include "test_serveritem.h"
#include "test_databaseitem.h"

int main(int argc, char *argv[])
{
	QApplication app( argc, argv );   

	int allTestsResult = 
            // connections-tree module
            QTest::qExec(new TestServerItem, argc, argv)
            + QTest::qExec(new TestDatabaseItem, argc, argv)

            // core
            + QTest::qExec(new TestCommand, argc, argv)
            + QTest::qExec(new TestResponse, argc, argv)
            + QTest::qExec(new TestConnection, argc, argv)

            // models
            + QTest::qExec(new TestConnectionsManager, argc, argv);

    if (allTestsResult != 0 ) {

		#ifdef WIN32		
		std::cin.get();		
		#endif // WIN32

        return 1;
    }

	return 0;
}

