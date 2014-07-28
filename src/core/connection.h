#ifndef CONNECTION_H
#define CONNECTION_H

#include <QObject>
#include "connectionconfig.h"

namespace RedisClient {

class AbstractProtocol;
class AbstractTransporter;
class Command;

class Connection : public QObject
{
    Q_OBJECT
    friend class AbstractProtocol;
public:
    Connection(const ConnectionConfig & c, bool autoConnect = false);
    ~Connection();

    ConnectionConfig config;

    bool connect();
    bool isConnected();
    void disconnect();
    void runCommand(const Command &cmd);
    QSharedPointer<AbstractProtocol> operations();
    bool waitConnectedState(unsigned int);
    ConnectionConfig getConfig() const;
    void setConnectionConfig(ConnectionConfig&);

signals:    
    void addCommandToWorker(const Command&);    
    void error(const QString&);
    void log(const QString&);
    void connected();
    void disconnected();

protected:
    bool m_isTransporterInitialized;
    bool m_connected;
    QSharedPointer<AbstractProtocol> protocol;
    QSharedPointer<QThread> transporterThread;
    QSharedPointer<AbstractTransporter> transporter;    
    QEventLoop m_loop;
    QEventLoop m_cmdLoop;
    QTimer m_timeoutTimer;
    int m_dbNumber;

    void setConnectedState();

protected slots:
    void connectionReady();
    void commandAddedToTransporter();
};

}
#endif // CONNECTION_H
