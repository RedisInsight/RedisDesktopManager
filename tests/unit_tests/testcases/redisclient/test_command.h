#pragma once

#include <QObject>
#include <QtCore>

class TestCommand : public QObject
{
	Q_OBJECT

private slots:
	void prepareCommand();
    void parseCommandString();
    void parseCommandString_data();
    void isSelectCommand();
};

