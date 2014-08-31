#include "test_console.h"
#include "console/consoletab.h"
#include "mocks/consoleoperations.h"
#include <QTest>

TestConsole::TestConsole(QObject *parent) :
    QObject(parent)
{
}

void TestConsole::testWidget()
{
    //given
    QSharedPointer<ConsoleOperationsMock> operations(new ConsoleOperationsMock());

    //when
    Console::ConsoleTab tab(operations);

    //then
    QCOMPARE(tab.getTitle(), operations->getConsoleName());
}
