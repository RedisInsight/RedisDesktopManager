#pragma once
#include "abstracttransporter.h"
#include "redisclient/ssh/qxtsshtcpsocket.h"
#include "redisclient/ssh/qxtsshclient.h"

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
    void reconnect();

private:
    QString getErrorString(QxtSshClient::Error);

private:
    QxtSshTcpSocket * socket; // owner of this object is sshClient
    QSharedPointer<QxtSshClient> m_sshClient;
    QSharedPointer<QEventLoop> m_syncLoop;
    QSharedPointer<QTimer> m_syncTimer;

    bool m_isHostKeyAlreadyAdded;
    bool m_lastConnectionOk;
};
}
