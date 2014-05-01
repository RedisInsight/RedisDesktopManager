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

bool RedisClient::Connection::isConnected()
{
    return m_connected;
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
    if (!cmd.isValid())
        throw ConnectionExeption("Command is not valid");

    if (!m_isTransporterInitialized)
        throw ConnectionExeption("Try run command in not connected state");

    emit addCommandToWorker(cmd);
}

RedisClient::AbstractProtocol *RedisClient::Connection::operations()
{
    return protocol.data();
}

void RedisClient::Connection::connectionReady()
{
    // todo: create signal in operations::auth() method and connect to this signal
    m_connected = true;
    // todo: do another ready staff
}

