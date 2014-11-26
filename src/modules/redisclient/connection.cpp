#include "connection.h"
#include "transporters/defaulttransporter.h"
#include "transporters/sshtransporter.h"
#include "commandexecutor.h"

RedisClient::Connection::Connection(const ConnectionConfig &c, bool autoConnect)
    : config(c), m_isTransporterInitialized(false), m_connected(false), m_dbNumber(-1)
{            
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

    if (m_transporter.isNull())
        createTransporter();

    // Create & run transporter
    m_transporterThread = QSharedPointer<QThread>(new QThread);
    m_transporter->moveToThread(m_transporterThread.data());
    QObject::connect(m_transporterThread.data(), SIGNAL(started()), m_transporter.data(), SLOT(init()));
    QObject::connect(m_transporterThread.data(), SIGNAL(finished()), m_transporter.data(), SLOT(disconnect()));
    QObject::connect(m_transporter.data(), SIGNAL(connected()), this, SLOT(auth()));
    QObject::connect(m_transporter.data(), SIGNAL(commandAdded()), this, SLOT(commandAddedToTransporter()));

    //wait for data
    QEventLoop loop;
    QTimer timeoutTimer;

    //configure sync objects
    timeoutTimer.setSingleShot(true);
    QObject::connect(&timeoutTimer, SIGNAL(timeout()), &loop, SLOT(quit()));
    QObject::connect(m_transporter.data(), SIGNAL(errorOccurred(const QString&)), &loop, SLOT(quit()));
    QObject::connect(this, SIGNAL(authError(const QString&)), &loop, SLOT(quit()));
    QObject::connect(this, SIGNAL(authOk()), &loop, SLOT(quit()));

    m_transporterThread->start();
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
    if (m_isTransporterInitialized && m_transporterThread->isRunning()) {
        m_transporterThread->quit();
        m_transporterThread->wait();
        m_isTransporterInitialized = false;
        m_transporter.clear();
    }

    m_connected = false;
}

void RedisClient::Connection::runCommand(const Command &cmd)
{
    if (!cmd.isValid())
        throw Exception("Command is not valid");

    if (!m_isTransporterInitialized || !m_connected)
        throw Exception("Try run command in not connected state");

    m_addLock.lock();

    emit addCommandToWorker(cmd);

    m_commandWaiter.wait(&m_addLock, 1000);
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

float RedisClient::Connection::getServerVersion()
{
    return 0.0; // TBD
}

void RedisClient::Connection::setConnectedState()
{
    m_connected = true;

    if (m_loop.isRunning())
        m_loop.exit();

    emit connected();
}

void RedisClient::Connection::createTransporter()
{
    //todo : implement unix socket transporter
    if (config.useSshTunnel()) {
       m_transporter = QSharedPointer<AbstractTransporter>(new SshTransporter(this));
    } else {
       m_transporter = QSharedPointer<AbstractTransporter>(new DefaultTransporter(this));
    }
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
}

void RedisClient::Connection::auth()
{
    // todo: check is socket succesufully connected before run this method
    m_connected = true;

    if (config.useAuth()) {
        Command authCmd(QStringList() << "auth" << config.auth);
        CommandExecutor::execute(this, authCmd);
    }

    Command testCommand("ping");
    Response testResult = CommandExecutor::execute(this, testCommand);

    if (testResult.toString() == "+PONG\r\n") {
        setConnectedState();
        emit authOk();
    } else {
        emit authError("Redis server require password or password invalid");
        m_connected = false;
    }
}

bool RedisClient::Connection::selectDb(int index)
{
    if (m_dbNumber == index)
        return true;

    m_dbNumber = index;
    QStringList commandParts;
    commandParts << "select" << QString::number(index);
    Command cmd(commandParts);
    Response result = CommandExecutor::execute(this, cmd);
    return result.isOkMessage();
}

void RedisClient::Connection::setTransporter(QSharedPointer<RedisClient::AbstractTransporter> transporter)
{
    if (transporter.isNull())
        return;

    m_transporter = transporter;
}
