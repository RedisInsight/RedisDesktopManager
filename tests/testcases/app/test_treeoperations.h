#pragma once

#include <QObject>

class TestTreeOperations : public QObject
{
    Q_OBJECT
public:
    explicit TestTreeOperations(QObject *parent = 0);
private slots:
    void testCreation();
};

