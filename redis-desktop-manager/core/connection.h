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
public:
    Connection(const Config & c, bool autoConnect = false);    
    ~Connection();

    Config config;

    void connect();
    bool isConnected();
    void disconnect();
    void runCommand(const Command &cmd);
    AbstractProtocol * operations();

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
