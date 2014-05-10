#include "sshtransporter.h"
#include "abstractprotocol.h"

#define MAX_BUFFER_SIZE 536800 //response part limit

RedisClient::SshTransporter::SshTransporter(RedisClient::Connection *c)
    : RedisClient::AbstractTransporter(c), isHostKeyAlreadyAdded(false), m_lastConnectionOk(false), socket(nullptr)
{

}

void RedisClient::SshTransporter::init()
{
    if (!sshClient.isNull()) {
        return;
    }

    executionTimer = QSharedPointer<QTimer>(new QTimer);
    executionTimer->setSingleShot(true);
    connect(executionTimer.data(), SIGNAL(timeout()), this, SLOT(executionTimeout()));    

    sshClient = QSharedPointer<QxtSshClient>(new QxtSshClient);
    connect(sshClient.data(), &QxtSshClient::error, this, &RedisClient::SshTransporter::OnSshConnectionError);
    connect(sshClient.data(), &QxtSshClient::connected, this, &RedisClient::SshTransporter::OnSshConnected);

    syncLoop = QSharedPointer<QEventLoop>(new QEventLoop);
    syncTimer = QSharedPointer<QTimer>(new QTimer);
    syncTimer->setSingleShot(true);

    connect(syncTimer.data(), SIGNAL(timeout()), syncLoop.data(), SLOT(quit()));

    connectToHost();
}

void RedisClient::SshTransporter::disconnect()
{    
    if (sshClient.isNull())
        return;

    QObject::disconnect(socket, 0, 0, 0);
    QObject::disconnect(sshClient.data(), 0, 0, 0);

    sshClient->resetState();
}

bool RedisClient::SshTransporter::connectToHost()
{
    ConnectionConfig config = m_connection->config;

    if (config.isSshPasswordUsed())
        sshClient->setPassphrase(config.sshPassword);

    if (!config.sshPrivateKeyPath.isEmpty()) {

        QString privateKey = config.getSshPrivateKey();

        if (!privateKey.isEmpty())
            sshClient->setKeyFiles("", privateKey);
    }

    //connect to ssh server
    m_lastConnectionOk = false;
    syncTimer->start(config.connectionTimeout);
    sshClient->connectToHost(config.sshUser, config.sshHost, config.sshPort);
    syncLoop->exec();

    if (!m_lastConnectionOk) {
        emit errorOccurred("SSH connection timeout, check connection settings");
        return false;
    }

    //connect to redis
    socket = sshClient->openTcpSocket(config.host, config.port);

    if (socket == NULL) {
        emit errorOccurred("SSH connection established, but socket failed");
        return false;
    }

    connect(socket, &QxtSshTcpSocket::readyRead, this, &RedisClient::SshTransporter::OnSocketReadyRead);

    m_lastConnectionOk = false;
    syncTimer->start(config.connectionTimeout);
    syncLoop->exec();

    if (!m_lastConnectionOk) {
        emit errorOccurred(QString("SSH connection established, but redis connection failed"));
        return false;
    }

    emit connected();
    emit logEvent(QString("%1 > connected").arg(m_connection->config.name));

    return true;
}

void RedisClient::SshTransporter::runCommand(const Command &command)
{
    emit logEvent(QString("%1 > [runCommand] %2").arg(m_connection->config.name).arg(command.getRawString()));

    if (command.hasDbIndex()) {

        bool isDbSelected = m_connection->operations()->selectDb(command.getDbIndex());

        if (!isDbSelected) {
            emit errorOccurred("Can not select db for running command");
            return sendResponse();
        }
    }

    m_response.clear();
    m_isCommandRunning = true;
    runningCommand = command;
    executionTimer->start(m_connection->config.executeTimeout);

    // Send command
    QByteArray byteArray = command.getByteRepresentation();
    const char* cString = byteArray.constData();
    socket->write(cString, byteArray.size());
}

void RedisClient::SshTransporter::OnSshConnectionError(QxtSshClient::Error error)
{
    if (!isHostKeyAlreadyAdded && QxtSshClient::HostKeyUnknownError == error) {
        QxtSshKey hostKey = sshClient->hostKey();
        sshClient->addKnownHost(m_connection->config.sshHost, hostKey);
        sshClient->resetState();
        sshClient->connectToHost(m_connection->config.sshUser, m_connection->config.sshHost, m_connection->config.sshPort);
        isHostKeyAlreadyAdded = true;
        return;
    }

    if (syncLoop->isRunning()) {
        syncLoop->exit();
    }

    emit errorOccurred(QString("SSH Connection error: %1").arg(getErrorString(error)));
}

void RedisClient::SshTransporter::OnSshConnected()
{
    m_lastConnectionOk = true;

    if (syncLoop->isRunning()) {
        syncLoop->exit();
    }
}

void RedisClient::SshTransporter::OnSocketReadyRead()
{
    //on first emit
    if (!m_lastConnectionOk) {
        m_lastConnectionOk = true;
    }

    if (syncLoop->isRunning()) {
        syncLoop->exit();
    }

    if (!m_isCommandRunning) {
        return;
    }

    executionTimer->stop();
    readingBuffer = socket->read(MAX_BUFFER_SIZE);
    m_response.appendToSource(readingBuffer);

    if (m_response.isValid()) {
        return sendResponse();
    } else {
        emit operationProgress(m_response.getLoadedItemsCount(), runningCommand.getOwner());
        executionTimer->start(m_connection->config.executeTimeout); //restart execution timer
    }
}

void RedisClient::SshTransporter::OnSshConnectionClose()
{
    if (syncLoop->isRunning())
        syncLoop->exit();
}

QString RedisClient::SshTransporter::getErrorString(QxtSshClient::Error error)
{
    switch (error) {
        case QxtSshClient::AuthenticationError:
            return "Authentication Error";
        case QxtSshClient::HostKeyUnknownError:
            return "Host Key Unknown Error";
        case QxtSshClient::HostKeyInvalidError:
            return "HostKey Invalid";
        case QxtSshClient::HostKeyMismatchError:
            return "HostKey Mismatch";
        case QxtSshClient::ConnectionRefusedError:
            return "Connection Refused";
        case QxtSshClient::UnexpectedShutdownError:
            return "Unexpected Shutdown";
        case QxtSshClient::HostNotFoundError:
            return "Host Not Found";
        case QxtSshClient::SocketError:
            return "Socket Error";
        default:
        case QxtSshClient::UnknownError:
            return "Unknown Error";
    }
}
