#pragma once

#include <QObject>

class TestConsole : public QObject
{
    Q_OBJECT
public:
    explicit TestConsole(QObject *parent = 0);

private slots:    
    void testHexUtils();
};

