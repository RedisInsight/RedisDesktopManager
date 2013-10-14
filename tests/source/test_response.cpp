#include "test_response.h"
#include "Response.h"

#include <QtCore>
#include <QTest>

void TestResponse::getValue()
{
	//given 
	Response test;
	QFETCH(QString, testResponse);
	QFETCH(QVariant, validResult);

	test.setSource(testResponse);

	//when
	QVariant actualResult = test.getValue();

	//then
	QCOMPARE(actualResult, validResult);
}

void TestResponse::getValue_data()
{
	QTest::addColumn<QString>("testResponse");
	QTest::addColumn<QVariant>("validResult");

	QTest::newRow("Status")		<< "+OK\r\n"							<< QVariant(QString("OK"));	
	QTest::newRow("Error")		<< "-ERR unknown command 'foobar'\r\n"	<< QVariant(QString("ERR unknown command 'foobar'"));	
	QTest::newRow("Integer")	<< ":99998\r\n"							<< QVariant(99998);	
	QTest::newRow("Bulk")		<< "$6\r\nfoobar\r\n"					<< QVariant("foobar");
	QTest::newRow("Null Bulk")	<< "$-1\r\n"							<< QVariant();
	QTest::newRow("Multi Bulk")	<< "*3\r\n:1\r\n:2\r\n$6\r\nfoobar\r\n"	<< QVariant(QStringList() << "1" << "2" << "foobar");
	QTest::newRow("Multi Bulk with empty item")	
		<< "*6\r\n$6\r\napp_id\r\n$1\r\n0\r\n$7\r\nkeyword\r\n$0\r\n\r\n$3\r\nurl\r\n$5\r\nnourl\r\n"
		<< QVariant(QStringList() << "app_id" << "0" << "keyword" << "" << "url" << "nourl");
	QTest::newRow("Multi Bulk with \\r\\n in item")	
		<< "*6\r\n$6\r\napp_id\r\n$1\r\n0\r\n$7\r\nkeyword\r\n$0\r\n\r\n$3\r\nurl\r\n$5\r\nn\r\nrl\r\n"
		<< QVariant(QStringList() << "app_id" << "0" << "keyword" << "" << "url" << "n\r\nrl");
}


void TestResponse::isValid()
{
	//given	
	QFETCH(QString, testResponse);
	QFETCH(bool, validResult);

	Response test(testResponse);	

	//when
	bool actualOnValid = test.isValid();

	//then
	QCOMPARE(actualOnValid, validResult);
}


void TestResponse::isValid_data()
{
	QTest::addColumn<QString>("testResponse");
	QTest::addColumn<bool>("validResult");

	//test int
	QTest::newRow("Int valid")		<< ":10000\r\n" << true;
	QTest::newRow("Int invalid")	<< ":99\n"		<< false;
	QTest::newRow("Int invalid")	<< ":"			<< false;
	QTest::newRow("Int invalid")	<< ""			<< false;

	//test bulk
	QTest::newRow("Bulk valid")		<< "$6\r\nfoobar\r\n"			<< true;
	QTest::newRow("Bulk valid")		<< "$-1\r\n"					<< true;
	QTest::newRow("Bulk valid")		<< "$12\r\n# Keyspace\r\n\r\n"  << true;	
	QTest::newRow("Bulk invalid")	<< "$1\r\n"						<< false;
	QTest::newRow("Bulk invalid")	<< "$5\r\n\r\n"					<< false;
	QTest::newRow("Bulk invalid")	<< "$5\r\nhell\r\n"				<< false;
	QTest::newRow("Bulk invalid")	<< "$5\r\n"						<< false;

	//test multi bulk
	QTest::newRow("Multi Bulk valid")
		<< "*5\r\n:1\r\n:2\r\n:3\r\n:4\r\n$6\r\nfoobar\r\n"	<< true;
	QTest::newRow("Multi Bulk valid")
		<< "*4\r\n$3\r\nfoo\r\n$3\r\nbar\r\n$5\r\nHello\r\n$5\r\nWorld\r\n"	<< true;

	QTest::newRow("Multi Bulk invalid") << "*5\r\n"						<< false;
	QTest::newRow("Multi Bulk invalid") << "*5\r\n:1\r\n"				<< false;
	QTest::newRow("Multi Bulk invalid") << "*2\r\n:1\r\n$6\r\nHello\r\n"<< false;

}