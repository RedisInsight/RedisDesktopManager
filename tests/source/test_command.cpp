#include "test_command.h"
#include "Command.h"

#include <QTest>

void TestCommand::prepareCommand()
{
	//given 
	//Command class

	//when
	QVariant actualResult = Command::getFormatted("EXISTS testkey:test");

	//then
	QCOMPARE(actualResult, QVariant("*2\r\n$6\r\nEXISTS\r\n$12\r\ntestkey:test\r\n"));
}



