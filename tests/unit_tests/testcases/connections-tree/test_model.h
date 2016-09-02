#pragma once
#include <QObject>

class TestModel : public QObject
{
    Q_OBJECT
private slots:
    void testLoadImplementation();
};

