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
		<< "a:1:{i:0;a:1:{i:0;a:1:{i:0;s:4:\"test\";}}}"				 << "TBD";
}