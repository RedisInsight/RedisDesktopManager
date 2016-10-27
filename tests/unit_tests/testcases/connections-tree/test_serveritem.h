#pragma once
#include <QObject>


class TestServerItem : public QObject
{
    Q_OBJECT
public:
    explicit TestServerItem(QObject *parent = 0);

private slots:
    void testLoad();
    void testLoad_invalid();

    void testUnload();
    void testReload();    
    void testBasicMethods();
};
