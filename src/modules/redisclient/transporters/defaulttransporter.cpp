#include "defaulttransporter.h"

RedisClient::DefaultTransporter::DefaultTransporter(RedisClient::Connection *c)
    : RedisClient::AbstractTransporter(c), socket(nullptr), m_errorOccurred(false)
{
}

RedisClient::DefaultTransporter::~DefaultTransporter()
{
    if (!socket.isNull())
        socket->abort();
}

void RedisClient::DefaultTransporter::init()
{
    using namespace RedisClient;

    if (!socket.isNull())
        return;    

    m_executionTimer = QSharedPointer<QTimer>(new QTimer);
    m_executionTimer->setSingleShot(true);
    connect(m_executionTimer.data(), SIGNAL(timeout()), this, SLOT(executionTimeout()));

    socket = QSharedPointer<QSslSocket>(new QSslSocket());
    socket->setSocketOption(QAbstractSocket::KeepAliveOption, 1);

    connect(socket.data(), &QSslSocket::readyRead, this, &DefaultTransporter::readyRead);
    connect(socket.data(), &QSslSocket::encrypted, this, &DefaultTransporter::encrypted);
    connect(socket.data(), SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(error(QAbstractSocket::SocketError)));
    connect(socket.data(), SIGNAL(sslErrors(const QList<QSslError> &)), this, SLOT(sslError(const QList<QSslError> &)));

    connectToHost();
}

void RedisClient::DefaultTransporter::disconnect()
{
    if (socket.isNull())
        return;

    //socket->disconnectFromHost();
    socket->abort();
    socket.clear();
}

bool RedisClient::DefaultTransporter::connectToHost()
{
    m_errorOccurred = false;

    RedisClient::ConnectionConfig& conf = m_connection->config;

    bool connectionResult = false;

    if (conf.useSsl()) {
        QList<QSslCertificate> trustedCas = conf.sslCaCertificates(); // Required

        if (trustedCas.empty()) {
            emit errorOccurred("SSL Error: no trusted Cas");
            return false;
        }

        socket->addCaCertificates(trustedCas);

        QString privateKey = conf.sslPrivateKeyPath();
        if (!privateKey.isEmpty()) {
            socket->setPrivateKey(privateKey);
        }

        QString localCert = conf.sslLocalCertPath();
        if (!localCert.isEmpty()) {
            socket->setLocalCertificate(localCert);
        }

        socket->connectToHostEncrypted(conf.host(), conf.port());
        connectionResult = socket->waitForEncrypted(conf.connectionTimeout())
                && socket->waitForConnected(conf.connectionTimeout());

    } else {
        socket->connectToHost(conf.host(), conf.port());
        connectionResult = socket->waitForConnected(conf.connectionTimeout());
    }

    if (connectionResult)
    {
        emit connected();
        emit logEvent(QString("%1 > connected").arg(conf.name()));
        return true;
    }

    if (!m_errorOccurred)
        emit errorOccurred("Connection timeout");

    emit logEvent(QString("%1 > connection failed").arg(conf.name()));
    return false;
}

void RedisClient::DefaultTransporter::runCommand(const Command &command)
{
    if (socket->state() == QAbstractSocket::UnconnectedState) {
        reconnect();
        m_commands.enqueue(command);
        m_isCommandRunning = false;
        return;
    }

    emit logEvent(QString("%1 > [runCommand] %2")
                  .arg(m_connection->config.name())
                  .arg(command.getRawString()));

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
    QByteArray cmd = command.getByteRepresentation();
    socket->write(cmd);
    socket->flush();
}

void RedisClient::DefaultTransporter::readyRead()
{
    if (socket->bytesAvailable() <= 0) {
        return;
    }

    m_executionTimer->stop();
    m_readingBuffer = socket->readAll();
    m_response.appendToSource(m_readingBuffer);

    if (m_response.isValid()) {
        return sendResponse();
    } else {        
        m_executionTimer->start(m_connection->config.executeTimeout()); //restart execution timer
    }
}

void RedisClient::DefaultTransporter::error(QAbstractSocket::SocketError error)
{
    if (error == QAbstractSocket::UnknownSocketError && connectToHost()) {
        return runCommand(m_runningCommand);
    }

    m_errorOccurred = true;

    emit errorOccurred(
        QString("Connection error: %1").arg(socket->errorString())
        );

    return sendResponse();
}

void RedisClient::DefaultTransporter::stateChanged(QAbstractSocket::SocketState socketState)
{
    qDebug() << "socket state changed:" <<socketState;
}

void RedisClient::DefaultTransporter::reconnect()
{
    emit logEvent("Reconnect to host");
    socket->abort();
    connectToHost();
}

void RedisClient::DefaultTransporter::sslError(const QList<QSslError> errors)
{
    m_errorOccurred = true;

    for (QSslError err : errors) {
        emit errorOccurred(QString("SSL error: %1").arg(err.errorString()));
    }
}

void RedisClient::DefaultTransporter::encrypted()
{
    emit logEvent("SSL encryption: OK");
}
