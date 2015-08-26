#include "sshtransporter.h"

#define MAX_BUFFER_SIZE 536800 //response part limit

RedisClient::SshTransporter::SshTransporter(RedisClient::Connection *c)
    :
      RedisClient::AbstractTransporter(c),
      socket(nullptr),
      m_isHostKeyAlreadyAdded(false),
      m_lastConnectionOk(false)
{

}

void RedisClient::SshTransporter::init()
{
    if (!m_sshClient.isNull()) {
        return;
    }

    m_executionTimer = QSharedPointer<QTimer>(new QTimer);
    m_executionTimer->setSingleShot(true);
    connect(m_executionTimer.data(), SIGNAL(timeout()), this, SLOT(executionTimeout()));    

    m_sshClient = QSharedPointer<QxtSshClient>(new QxtSshClient);
    connect(m_sshClient.data(), &QxtSshClient::error, this, &RedisClient::SshTransporter::OnSshConnectionError);
    connect(m_sshClient.data(), &QxtSshClient::connected, this, &RedisClient::SshTransporter::OnSshConnected);

    m_syncLoop = QSharedPointer<QEventLoop>(new QEventLoop);
    m_syncTimer = QSharedPointer<QTimer>(new QTimer);
    m_syncTimer->setSingleShot(true);

    connect(m_syncTimer.data(), SIGNAL(timeout()), m_syncLoop.data(), SLOT(quit()));

    connectToHost();
}

void RedisClient::SshTransporter::disconnect()
{    
    if (m_sshClient.isNull())
        return;

    if (socket)
        QObject::disconnect(socket, 0, 0, 0);

    QObject::disconnect(m_sshClient.data(), 0, 0, 0);

    m_sshClient->resetState();
}

bool RedisClient::SshTransporter::connectToHost()
{
    ConnectionConfig config = m_connection->config;

    if (config.isSshPasswordUsed())
        m_sshClient->setPassphrase(config.param<QString>("ssh_password"));

    QString privateKey = config.getSshPrivateKey();

    if (!privateKey.isEmpty()) {
        m_sshClient->setKeyFiles("", privateKey);
    }

    //connect to ssh server
    m_lastConnectionOk = false;
    m_syncTimer->start(config.connectionTimeout());
    m_sshClient->connectToHost(config.param<QString>("ssh_user"),
                             config.param<QString>("ssh_host"),
                             config.param<int>("ssh_port"));
    m_syncLoop->exec();

    if (!m_lastConnectionOk) {
        if (!m_syncTimer->isActive())
            emit errorOccurred("SSH connection timeout, check connection settings");
        return false;
    }

    //connect to redis
    socket = m_sshClient->openTcpSocket(config.host(), config.port());

    if (!socket) {
        emit errorOccurred("SSH connection established, but socket failed");
        return false;
    }

    connect(socket, &QxtSshTcpSocket::readyRead, this, &RedisClient::SshTransporter::OnSocketReadyRead);
    connect(socket, SIGNAL(destroyed()), this, SLOT(OnSshSocketDestroyed()));

    m_lastConnectionOk = false;
    m_syncTimer->start(config.connectionTimeout());
    m_syncLoop->exec();

    if (!m_lastConnectionOk) {
        emit errorOccurred(QString("SSH connection established, but redis connection failed"));
        return false;
    }

    emit connected();
    emit logEvent(QString("%1 > connected").arg(m_connection->config.name()));

    return true;
}

void RedisClient::SshTransporter::runCommand(const Command &command)
{
    emit logEvent(QString("%1 > [runCommand] %2")
                  .arg(m_connection->config.name())
                  .arg(command.getRawString()));

    if (!socket){
        emit errorOccurred("SSH: TCP socket not connected!");
        return;
    }

    if (command.hasDbIndex()) {

        bool isDbSelected = m_connection->selectDb(command.getDbIndex());

        if (!isDbSelected) {
            emit errorOccurred("Can not select db for running command");
            return sendResponse();
        }
    }

    m_response.clear();
    m_isCommandRunning = true;
    m_runningCommand = command;
    m_executionTimer->start(m_connection->config.executeTimeout());

    // Send command
    QByteArray byteArray = command.getByteRepresentation();
    const char* cString = byteArray.constData();
    socket->write(cString, byteArray.size());
}

void RedisClient::SshTransporter::OnSshConnectionError(QxtSshClient::Error error)
{
    if (!m_isHostKeyAlreadyAdded && QxtSshClient::HostKeyUnknownError == error) {
        QxtSshKey hostKey = m_sshClient->hostKey();
        m_sshClient->addKnownHost(m_connection->config.param<QString>("ssh_host"), hostKey);
        m_sshClient->resetState();
        m_sshClient->connectToHost(m_connection->config.param<QString>("ssh_user"),
                                 m_connection->config.param<QString>("ssh_host"),
                                 m_connection->config.param<int>("ssh_port"));

        m_isHostKeyAlreadyAdded = true;
        return;
    }

    if (m_syncLoop->isRunning()) {
        m_syncLoop->exit();
    }

    emit errorOccurred(QString("SSH Connection error: %1").arg(getErrorString(error)));
}

void RedisClient::SshTransporter::OnSshConnected()
{
    m_lastConnectionOk = true;

    if (m_syncLoop->isRunning()) {
        m_syncLoop->exit();
    }
}

void RedisClient::SshTransporter::OnSocketReadyRead()
{
    //on first emit
    if (!m_lastConnectionOk) {
        m_lastConnectionOk = true;
    }

    if (m_syncLoop->isRunning()) {
        m_syncLoop->exit();
    }

    if (!m_isCommandRunning || !socket) {
        return;
    }

    m_executionTimer->stop();
    m_readingBuffer = socket->read(MAX_BUFFER_SIZE);
    m_response.appendToSource(m_readingBuffer);

    if (m_response.isValid()) {
        return sendResponse();
    } else {        
        m_executionTimer->start(m_connection->config.executeTimeout()); //restart execution timer
    }
}

void RedisClient::SshTransporter::OnSshConnectionClose()
{
    if (m_syncLoop->isRunning())
        m_syncLoop->exit();

    emit logEvent("SSH connection closed");
}

void RedisClient::SshTransporter::OnSshSocketDestroyed()
{
    socket = nullptr;
    emit logEvent("SSH socket detroyed");
}

void RedisClient::SshTransporter::reconnect()
{
    emit logEvent("Reconnect to host");
    if (socket) socket->close();
    m_sshClient->resetState();
    connectToHost();
}

QString RedisClient::SshTransporter::getErrorString(QxtSshClient::Error error)
{
    switch (error) {
        case QxtSshClient::AuthenticationError: return "Authentication Error";
        case QxtSshClient::HostKeyUnknownError: return "Host Key Unknown Error";
        case QxtSshClient::HostKeyInvalidError: return "HostKey Invalid";
        case QxtSshClient::HostKeyMismatchError: return "HostKey Mismatch";
        case QxtSshClient::ConnectionRefusedError: return "Connection Refused";
        case QxtSshClient::UnexpectedShutdownError: return "Unexpected Shutdown";
        case QxtSshClient::HostNotFoundError: return "Host Not Found";
        case QxtSshClient::SocketError: return "Socket Error";
        default:
        case QxtSshClient::UnknownError: return "Unknown Error";
    }
}
