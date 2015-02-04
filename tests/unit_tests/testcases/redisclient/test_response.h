#pragma once

#include <QObject>

class TestResponse: public QObject
{
	Q_OBJECT

private slots:
	//parsing tests
	void getValue();
	void getValue_data();

    // basic tests
    void source();
    void valueToHumanReadString();

	//validation tests
	void isValid();
	void isValid_data();	
};

