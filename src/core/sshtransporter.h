#ifndef SSHTRANSPORTER_H
#define SSHTRANSPORTER_H

#include "abstracttransporter.h"
#include "ssh/qxtsshtcpsocket.h"
#include "ssh/qxtsshclient.h"

namespace RedisClient {

class SshTransporter : public AbstractTransporter
{
public:
    SshTransporter(Connection *);
public slots:
    void init();
    void disconnect();
protected:
    bool connectToHost();
    void runCommand(const Command &cmd);
protected slots:
    void OnSshConnectionError(QxtSshClient::Error);
    void OnSshConnected();
    void OnSocketReadyRead();
    void OnSshConnectionClose();
private:
    QxtSshTcpSocket * socket; // owner of this object is sshClient
    QSharedPointer<QxtSshClient> sshClient;
    QSharedPointer<QEventLoop> syncLoop;
    QSharedPointer<QTimer> syncTimer;

    bool isHostKeyAlreadyAdded;
    bool m_lastConnectionOk;

    QString getErrorString(QxtSshClient::Error);
};

}

#endif // SSHTRANSPORTER_H
