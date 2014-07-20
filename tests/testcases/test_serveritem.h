#ifndef TEST_SERVERITEM_H
#define TEST_SERVERITEM_H

#include <QObject>

class TestServerItem : public QObject
{
    Q_OBJECT
public:
    explicit TestServerItem(QObject *parent = 0);

private slots:

    void testLoad();

};

#endif // TEST_SERVERITEM_H
