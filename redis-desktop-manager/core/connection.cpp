#include "connection.h"
#include "defaulttransporter.h"
#include "defaultprotocol.h"
#include "sshtransporter.h"

RedisClient::Connection::Connection(const Config &c, bool autoConnect)
    : config(c)
{        
    if (c.useSshTunnel()) {
        //transporter = new QSharedPointer<AbstractTransporter>(new SshTransporter(this));
    } else {
       transporter = QSharedPointer<AbstractTransporter>(new DefaultTransporter(this));
    }

    protocol = QSharedPointer<AbstractProtocol>(new DefaultProtocol(this));

    if (autoConnect)
        connect();
}

RedisClient::Connection::~Connection()
{
    if (m_isTransporterInitialized)
        disconnect();
}

void RedisClient::Connection::connect()
{
    if (m_isTransporterInitialized)
        return;

    // Create & run transporter
    transporterThread = QSharedPointer<QThread>(new QThread);
    transporter->moveToThread(transporterThread.data());
    QObject::connect(transporterThread.data(), SIGNAL(started()), transporter.data(), SLOT(init()));
    QObject::connect(transporterThread.data(), SIGNAL(finished()), transporter.data(), SLOT(disconnect()));
    transporterThread->start();

    protocol->auth();
}

void RedisClient::Connection::disconnect()
{
    if (m_isTransporterInitialized && transporterThread->isRunning()) {
        transporterThread->quit();
        transporterThread->wait();
        m_isTransporterInitialized = false;
    }
}

void RedisClient::Connection::runCommand(const Command &cmd)
{
    if (!m_isTransporterInitialized)
        throw ConnectionExeption("Try run command in not connected state");

    emit addCommandToWorker(cmd);
}

