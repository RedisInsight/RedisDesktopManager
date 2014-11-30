#ifndef CONNECTION_H
#define CONNECTION_H

#include <QObject>
#include <QWaitCondition>
#include "connectionconfig.h"
#include "exception.h"
#include "command.h"

namespace RedisClient {

class AbstractTransporter;

class Connection : public QObject
{
    Q_OBJECT
    ADD_EXCEPTION
public:
    Connection(const ConnectionConfig & c, bool autoConnect = false);
    ~Connection();

    ConnectionConfig config;

    bool connect();
    bool isConnected();
    void disconnect();
    void runCommand(const Command &cmd);    
    bool waitConnectedState(unsigned int);
    ConnectionConfig getConfig() const;
    void setConnectionConfig(ConnectionConfig&);

    float getServerVersion();

    /**
     * Select db
     * see  http://redis.io/commands/select
     */
    bool selectDb(int index);

    void setTransporter(QSharedPointer<AbstractTransporter>);

signals:    
    void addCommandToWorker(Command);
    void error(const QString&);
    void log(const QString&);
    void connected();
    void disconnected();
    void authOk();
    void authError(const QString&);

protected:    
    bool m_connected;    
    QSharedPointer<QThread> m_transporterThread;
    QSharedPointer<AbstractTransporter> m_transporter;
    QEventLoop m_loop;
    QEventLoop m_cmdLoop;
    QMutex m_addLock;
    QWaitCondition m_commandWaiter;
    QTimer m_timeoutTimer;
    int m_dbNumber;

    void setConnectedState();
    void createTransporter();
    bool isTransporterRunning();

protected slots:
    void connectionReady();
    void commandAddedToTransporter();
    void auth();
};

}
#endif // CONNECTION_H
