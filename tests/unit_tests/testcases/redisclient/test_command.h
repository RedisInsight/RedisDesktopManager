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

    void scanCommandSetCursor();
    void scanCommandSetCursor_data();
    void scanCommandIsValid();
    void scanCommandIsValid_data();
};

