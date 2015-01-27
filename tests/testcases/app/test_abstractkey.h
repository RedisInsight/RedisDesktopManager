#pragma once

#include <QObject>

class TestAbstractKey : public QObject
{
    Q_OBJECT

private slots:
    void testValueToEscapedString();
    void testEscapedStringToValue();
//void testValueToBinary();

};
