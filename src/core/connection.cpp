#include "connection.h"
#include "defaulttransporter.h"
#include "defaultprotocol.h"
#include "sshtransporter.h"
#include "connectionexception.h"

RedisClient::Connection::Connection(const ConnectionConfig &c, bool autoConnect)
    : config(c), m_isTransporterInitialized(false), m_connected(false), m_dbNumber(-1)
{        
    protocol = QSharedPointer<AbstractProtocol>(new DefaultProtocol(this));

    m_timeoutTimer.setSingleShot(true);
    QObject::connect(&m_timeoutTimer, SIGNAL(timeout()), &m_loop, SLOT(quit()));

    if (autoConnect)
        connect();
}

RedisClient::Connection::~Connection()
{
    if (m_isTransporterInitialized)
        disconnect();
}

bool RedisClient::Connection::connect() // todo: add block/unblock parameter
{
    if (isConnected())
        return true;

    if (m_isTransporterInitialized)
        return false;

    //todo : implement unix socket transporter
    if (config.useSshTunnel()) {
       transporter = QSharedPointer<AbstractTransporter>(new SshTransporter(this));
    } else {
       transporter = QSharedPointer<AbstractTransporter>(new DefaultTransporter(this));
    }

    // Create & run transporter
    transporterThread = QSharedPointer<QThread>(new QThread);
    transporter->moveToThread(transporterThread.data());
    QObject::connect(transporterThread.data(), SIGNAL(started()), transporter.data(), SLOT(init()));
    QObject::connect(transporterThread.data(), SIGNAL(finished()), transporter.data(), SLOT(disconnect()));
    QObject::connect(transporter.data(), SIGNAL(connected()), protocol.data(), SLOT(auth()));
    QObject::connect(transporter.data(), SIGNAL(commandAdded()), this, SLOT(commandAddedToTransporter()));

    //wait for data
    QEventLoop loop;
    QTimer timeoutTimer;

    //configure sync objects
    timeoutTimer.setSingleShot(true);
    QObject::connect(&timeoutTimer, SIGNAL(timeout()), &loop, SLOT(quit()));
    QObject::connect(transporter.data(), SIGNAL(errorOccurred(const QString&)), &loop, SLOT(quit()));
    QObject::connect(protocol.data(), SIGNAL(errorOccurred(const QString&)), &loop, SLOT(quit()));
    QObject::connect(protocol.data(), SIGNAL(authOk()), &loop, SLOT(quit()));

    transporterThread->start();
    m_isTransporterInitialized = true;
    timeoutTimer.start(config.connectionTimeout);
    loop.exec();

    if (!m_connected)
        disconnect();

    return m_connected;
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

    m_connected = false;
}

void RedisClient::Connection::runCommand(const Command &cmd)
{
    if (!cmd.isValid())
        throw ConnectionExeption("Command is not valid");

    if (!m_isTransporterInitialized || !m_connected)
        throw ConnectionExeption("Try run command in not connected state");

    m_addLock.lock();

    emit addCommandToWorker(cmd);
}

QSharedPointer<RedisClient::AbstractProtocol> RedisClient::Connection::operations()
{
    return protocol;
}

bool RedisClient::Connection::waitConnectedState(unsigned int timeoutInMs)
{
    if (isConnected())
        return true;

    if (!m_isTransporterInitialized)
        return false;

    m_timeoutTimer.start(timeoutInMs);
    m_loop.exec();

    return isConnected();
}

RedisClient::ConnectionConfig RedisClient::Connection::getConfig() const
{
    return config;
}

void RedisClient::Connection::setConnectionConfig(RedisClient::ConnectionConfig &c)
{
    config = c;
}

void RedisClient::Connection::setConnectedState()
{
    m_connected = true;

    if (m_loop.isRunning())
        m_loop.exit();

    emit connected();
}

void RedisClient::Connection::connectionReady()
{
    // todo: create signal in operations::auth() method and connect to this signal
    m_connected = true;
    // todo: do another ready staff
}

void RedisClient::Connection::commandAddedToTransporter()
{
    m_addLock.unlock();
    qDebug() << "Command added";
}

