#include "connection.h"
#include "command.h"
#include "scancommand.h"
#include "transporters/defaulttransporter.h"
#include "transporters/sshtransporter.h"
#include "commandexecutor.h"
#include "scanresponse.h"

RedisClient::Connection::Connection(const ConnectionConfig &c, bool autoConnect)
    : config(c), m_connected(false), m_dbNumber(-1)
{            
    m_timeoutTimer.setSingleShot(true);
    QObject::connect(&m_timeoutTimer, SIGNAL(timeout()), &m_loop, SLOT(quit()));

    if (autoConnect)
        connect();
}

RedisClient::Connection::~Connection()
{
    if (isTransporterRunning())
        disconnect();
}

bool RedisClient::Connection::connect() // todo: add block/unblock parameter
{
    if (isConnected())
        return true;    

    if (isTransporterRunning())
        return false;

    if (config.isValid() == false)
        throw Exception("Invalid config detected");

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
    timeoutTimer.start(config.param<int>("timeout_connect"));
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
    if (isTransporterRunning()) {
        m_transporterThread->quit();
        m_transporterThread->wait();        
        m_transporter.clear();
    }

    m_connected = false;
}

void RedisClient::Connection::runCommand(const Command &cmd)
{
    if (!cmd.isValid())
        throw Exception("Command is not valid");

    if (!isTransporterRunning() || !m_connected)
        throw Exception("Try run command in not connected state");

    m_addLock.lock();

    QObject::connect(cmd.getOwner(), SIGNAL(destroyed(QObject *)),
            m_transporter.data(), SLOT(cancelCommands(QObject *)));

    emit addCommandToWorker(cmd);
}

void RedisClient::Connection::retrieveCollection(QSharedPointer<RedisClient::ScanCommand> cmd,
                                                 std::function<void (QVariant)> callback)
{
    if (getServerVersion() < 2.8)
        throw Exception("Scan commands not supported by redis-server.");

    if (!cmd->isValidScanCommand())
        throw Exception("Invalid command");    

    // workaround
    Command selectCmd(QStringList() << "select" << QString::number(cmd->getDbIndex()));
    try {
        CommandExecutor::execute(this, selectCmd);
    } catch (const RedisClient::CommandExecutor::Exception& e) {
        throw Exception("Connection error: " + QString(e.what()));
    }

    processScanCommand(cmd, callback);
}

bool RedisClient::Connection::waitConnectedState(unsigned int timeoutInMs)
{
    if (isConnected())
        return true;

    if (!isTransporterRunning())
        return false;

    m_timeoutTimer.start(timeoutInMs);
    m_loop.exec();

    return isConnected();
}

RedisClient::ConnectionConfig RedisClient::Connection::getConfig() const
{
    return config;
}

void RedisClient::Connection::setConnectionConfig(const RedisClient::ConnectionConfig &c)
{
    config = c;    
}

double RedisClient::Connection::getServerVersion()
{
    return m_serverInfo.version;
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

bool RedisClient::Connection::isTransporterRunning()
{
    return m_transporter.isNull() == false
            && m_transporterThread.isNull() == false
            && m_transporterThread->isRunning();
}

void RedisClient::Connection::processScanCommand(QSharedPointer<ScanCommand> cmd,
                                                 std::function<void(QVariant)> callback,
                                                 QSharedPointer<QVariantList> result)
{
    if (result.isNull())
        result = QSharedPointer<QVariantList>(new QVariantList());

    cmd->setCallBack(this, [this, cmd, result, callback](RedisClient::Response r){

        if (!ScanResponse::isValidScanResponse(r)) {
            callback(QVariant(*result));
            return;
        }

        RedisClient::ScanResponse* scanResp = (RedisClient::ScanResponse*)(&r);

        if (!scanResp) {
            callback(QVariant("-Error occured on cast ScanResponse from Response."));
            return;
        }

        result->append(scanResp->getCollection());

        if (scanResp->getCursor() <= 0) {            
            callback(QVariant(*result));
            return;
        }

        cmd->setCursor(scanResp->getCursor());

        processScanCommand(cmd, callback, result);
    });

    runCommand(*cmd);
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
    emit log("AUTH");
    // todo: check is socket succesufully connected before run this method
    m_connected = true;    

    try {
        if (config.useAuth()) {
            Command authCmd(QStringList() << "auth" << config.auth());
            authCmd.markAsHiPriorityCommand();
            CommandExecutor::execute(this, authCmd);
        }

        Command testCommand("ping", nullptr, m_dbNumber);
        testCommand.markAsHiPriorityCommand();
        Response testResult = CommandExecutor::execute(this, testCommand);

        if (testResult.toString() == "+PONG\r\n") {
            Command infoCommand("INFO");
            infoCommand.markAsHiPriorityCommand();
            Response infoResult = CommandExecutor::execute(this, infoCommand);
            m_serverInfo = ServerInfo::fromString(infoResult.getValue().toString());

            setConnectedState();
            emit log("AUTH OK");
            emit authOk();
        } else {
            emit error("AUTH ERROR");
            emit authError("Redis server require password or password invalid");
            m_connected = false;
        }
    } catch (const RedisClient::CommandExecutor::Exception& e) {
        emit error("Connection error on AUTH");
        emit authError("Connection error on AUTH");
        m_connected = false;
    }
}

bool RedisClient::Connection::selectDb(int index)
{
    if (m_dbNumber == index)
        return true;

    QStringList commandParts;
    commandParts << "select" << QString::number(index);
    Command cmd(commandParts);
    Response result = CommandExecutor::execute(this, cmd);

    if (result.isOkMessage()) {
        m_dbNumber = index;
        return true;
    } else {
        return false;
    }
}

void RedisClient::Connection::setTransporter(QSharedPointer<RedisClient::AbstractTransporter> transporter)
{
    if (transporter.isNull())
        return;

    m_transporter = transporter;
}

RedisClient::ServerInfo RedisClient::ServerInfo::fromString(const QString &info)
{
    QRegExp versionRegex("redis_version:([0-9]\\.[0-9]+)", Qt::CaseInsensitive, QRegExp::RegExp2);

    int pos = versionRegex.indexIn(info);

    RedisClient::ServerInfo result;
    if (pos == -1) {
        result.version = 0.0;
    } else {
        result.version = versionRegex.cap(1).toDouble();
    }

    return result;
}
