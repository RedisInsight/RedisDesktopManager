#include "test_valueformatters.h"

#include <QtCore>
#include <QTest>

#include "PHPSerializeFormatter.h"

void TestValueFormatters::PHPSerializeFormatterTest()
{
	//given 
	PHPSerializeFormatter formatter;
	QFETCH(QString, testRawValue);
	QFETCH(QString, validResult);	

	//when
	formatter.setRawValue(testRawValue);
	QString actualResult = formatter.getFormatted();

	//then
	QCOMPARE(actualResult, validResult);

}

void TestValueFormatters::PHPSerializeFormatterTest_data()
{
	QTest::addColumn<QString>("testRawValue");
	QTest::addColumn<QString>("validResult");

	QTest::newRow("Null")			<< "N;"								 << "NULL";	
	QTest::newRow("Bool")			<< "b:0;"							 << "false";	
	QTest::newRow("Integer")		<< "i:123;"							 << "123";	
	QTest::newRow("Double")			<< "d:123.1;"						 << "123.1";	
	QTest::newRow("String")			<< "s:3:\"123\";"					 << "\"123\"";	
	QTest::newRow("Array")			<< "a:2:{i:0;s:3:\"123\";i:1;i:123;}"<< "array(\n   [0] => \"123\",\n   [1] => 123\n)";	
	QTest::newRow("Empty object")	<< "O:4:\"demo\":0:{}"				 << "Object:demo";

	QTest::newRow("Array with sub arrays")	
		<< "a:1:{i:0;a:1:{i:0;a:1:{i:0;s:4:\"test\";}}}"				 
		<< "array(\n   [0] => array(\n      [0] => array(\n         [0] => \"test\"\n      )\n   )\n)";

	QTest::newRow("Array with sub array and string as key and object as value")
		<< "a:1:{i:0;a:1:{s:4:\"test\";O:16:\"MySuperClass_123\":2:{s:3:\"str\";s:46:\"awesome test "
		   "string with numbers 13 and !#!#!#\";s:4:\"str2\";N;}}}"
		<<  "array(\n   [0] => array(\n      [\"test\"] => Object:MySuperClass_123\n   )\n)";

	QTest::newRow("Array with NULL and 2 same objects")
		<< "a:4:{i:0;N;i:1;N;i:2;O:16:\"MySuperClass_123\":1:{s:3:\"str\";N;}i:3;r:4;}"
		<< "TBD";
}