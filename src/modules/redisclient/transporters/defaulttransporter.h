#pragma once

#include "abstracttransporter.h"
#include <QSslSocket>

namespace RedisClient {

class DefaultTransporter : public AbstractTransporter
{
    Q_OBJECT
public:
    DefaultTransporter(Connection * c);
    ~DefaultTransporter();
public slots:
    void init();
    void disconnect();
protected:    
    bool connectToHost();
    void runCommand(const Command &cmd);
private slots:    
    void readyRead();
    void error(QAbstractSocket::SocketError error);
    void stateChanged(QAbstractSocket::SocketState socketState);
    void reconnect();
private:
    QSharedPointer<QSslSocket> socket;
    bool m_errorOccurred;
};

}
