#pragma once

#include <QHash>
#include <QObject>
#include <QWaitCondition>
#include <functional>
#include <QVariantList>
#include <QSharedPointer>
#include "connectionconfig.h"
#include "exception.h"
#include "command.h"
#include "scancommand.h"

namespace RedisClient {

class AbstractTransporter;

struct ServerInfo
{
    double version;

    static ServerInfo fromString(const QString& info);
};

class Connection : public QObject
{
    Q_OBJECT
    ADD_EXCEPTION    
public:
    Connection(const ConnectionConfig & c, bool autoConnect = false);
    virtual ~Connection();

    ConnectionConfig config;

    bool connect();
    bool isConnected();
    void disconnect();

    virtual void runCommand(const Command &cmd);
    virtual void retrieveCollection(QSharedPointer<ScanCommand> cmd,
                                    std::function<void(QVariant)> callback);

    bool waitConnectedState(unsigned int);
    ConnectionConfig getConfig() const;
    void setConnectionConfig(const ConnectionConfig &);

    virtual double getServerVersion();

    /**
     * Select db
     * see  http://redis.io/commands/select
     */
    bool selectDb(int index);

    void setTransporter(QSharedPointer<AbstractTransporter>);
    QSharedPointer<AbstractTransporter> getTransporter() const;

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
    ServerInfo m_serverInfo;

    void setConnectedState();
    void createTransporter();
    bool isTransporterRunning();

    void processScanCommand(QSharedPointer<ScanCommand> cmd,
                            std::function<void(QVariant)> callback,
                            QSharedPointer<QVariantList> result=QSharedPointer<QVariantList>());

protected slots:
    void connectionReady();
    void commandAddedToTransporter();
    void auth();
};
}
