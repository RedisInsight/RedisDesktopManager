#ifndef TEST_DATABASEITEM_H
#define TEST_DATABASEITEM_H

#include <QObject>

class TestDatabaseItem : public QObject
{
    Q_OBJECT
public:
    explicit TestDatabaseItem(QObject *parent = 0);

private slots:
    void testLoadKeys();

};

#endif // TEST_DATABASEITEM_H
