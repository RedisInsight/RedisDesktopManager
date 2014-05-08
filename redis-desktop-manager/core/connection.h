#ifndef CONNECTION_H
#define CONNECTION_H

#include <QObject>
#include <QQueue>
#include <QMap>
#include <stdexcept>
#include "config.h"
#include "Command.h"
#include "Response.h"
#include "RedisConnectionConfig.h"

namespace RedisClient {

typedef RedisConnectionConfig Config;
class AbstractProtocol;
class AbstractTransporter;

class Connection : public QObject
{
    Q_OBJECT
    friend class AbstractProtocol;
public:
    Connection(const Config & c, bool autoConnect = false);    
    ~Connection();

    Config config;

    bool connect();
    bool isConnected();
    void disconnect();
    void runCommand(const Command &cmd);
    AbstractProtocol * operations();
    bool waitConnectedState(unsigned int);

signals:    
    void addCommandToWorker(const Command&);
    void operationProgress(int percents, QObject *);
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
    QTimer m_timeoutTimer;

    void setConnectedState();

protected slots:
    void connectionReady();
};

class ConnectionExeption : public std::runtime_error
{
public:
    ConnectionExeption(const QString &err)
        : std::runtime_error(err.toStdString())
    {
    }
};

}
#endif // CONNECTION_H
