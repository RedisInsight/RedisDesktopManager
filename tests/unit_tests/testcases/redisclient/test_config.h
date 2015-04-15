#pragma once

#include <QObject>
#include <QtCore>

class TestConfig : public QObject
{
    Q_OBJECT

private slots:
    void testGetParam();
    void testOwner();
    void testSerialization();
};


