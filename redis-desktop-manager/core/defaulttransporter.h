#ifndef DEFAULTTRANSPORTER_H
#define DEFAULTTRANSPORTER_H

#include "abstracttransporter.h"
#include <QTcpSocket>

namespace RedisClient {

class DefaultTransporter : public AbstractTransporter
{
    Q_OBJECT
public:
    DefaultTransporter(Connection * c);
public slots:
    void init();
    void disconnect();
protected:
    QSharedPointer<QTcpSocket> socket;
    bool connectToHost();
    void runCommand(const Command &cmd);
private slots:
    void readyRead();
    void error(QAbstractSocket::SocketError error);
};

}
#endif // DEFAULTTRANSPORTER_H
