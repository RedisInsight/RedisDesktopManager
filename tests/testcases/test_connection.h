#ifndef TEST_CONNECTION_H
#define TEST_CONNECTION_H

#include <QObject>
#include <QtCore>

class TestConnection : public QObject
{
    Q_OBJECT

private slots:
    void connectToHostAndRunCommand();

    void selectDatabase();
    void selectDatabase_data();

    void connectWithAuth();

    void connectWithSshTunnel();
};

#endif // TEST_CONNECTION_H

