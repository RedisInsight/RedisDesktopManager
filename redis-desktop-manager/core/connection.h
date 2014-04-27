#ifndef CONNECTION_H
#define CONNECTION_H

#include <QObject>
#include <QQueue>
#include <QMap>
#include "config.h"
#include "Command.h"
#include "Response.h"
#include "abstractprotocol.h"
#include "RedisConnectionConfig.h"

namespace RedisClient {

typedef RedisConnectionConfig Config;

class Connection : public QObject
{
    Q_OBJECT
public:
    Connection(const Config & c, bool autoConnect = false);
    ~Connection();

    bool connect();
    bool isConnected();
    bool disconnect();
    void runCommand(const Command &cmd);
    AbstractProtocol * operations();

signals:
    void responseResived(const QVariant &, QObject *);
    void operationProgress(int percents, QObject *);
    void errorOccurred(QString);
    void log(QString);

public slots:
    void addCommand(const Command&);
    void getDatabases();

protected:
    bool connected;
    QTimer * executionTimer;
    Response resp;
    QByteArray readingBuffer;
    bool commandRunning;
    Command runningCommand;
    QQueue<Command> commands;
    QSharedPointer<AbstractProtocol> protocol;
    QSharedPointer<AbstractTransporter> transporter;

    virtual void init();
    virtual void sendResponse();
    void processCommandQueue();

protected slots:
    void executionTimeout();
    void cancelCommands(QObject *);
};
}
#endif // CONNECTION_H
