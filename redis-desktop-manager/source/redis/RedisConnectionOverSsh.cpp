#include "RedisConnectionOverSsh.h"
#include "Command.h"
#include "Response.h"

#define MAX_BUFFER_SIZE 536800 //response part limit

RedisConnectionOverSsh::RedisConnectionOverSsh(const RedisConnectionConfig &c)
    : RedisConnectionAbstract(c),  isHostKeyAlreadyAdded(false), socketConnected(false)
{

}

void RedisConnectionOverSsh::init()
{
    if (!sshClient.isNull()) {
        return;
    }

    RedisConnectionAbstract::init();

    sshClient = QSharedPointer<QxtSshClient>(new QxtSshClient);
    syncLoop = QSharedPointer<QEventLoop>(new QEventLoop);
    syncTimer = QSharedPointer<QTimer>(new QTimer);

    syncTimer->setSingleShot(true);

    QObject::connect(syncTimer.data(), SIGNAL(timeout()), syncLoop.data(), SLOT(quit()));
    QObject::connect(sshClient.data(), SIGNAL(connected()), this, SLOT(OnSshConnected())); 

    QObject::connect(
        sshClient.data(), SIGNAL(error(QxtSshClient::Error)), 
        this, SLOT(OnSshConnectionError(QxtSshClient::Error))
        );
}

RedisConnectionOverSsh::~RedisConnectionOverSsh(void)
{    
}

bool RedisConnectionOverSsh::connect() 
{
    init();

    //set password
    sshClient->setPassphrase(config.sshPassword);    

    //connect to ssh server
    syncTimer->start(config.connectionTimeout);
    sshClient->connectToHost(config.sshUser, config.sshHost, config.sshPort);
    syncLoop->exec();

    if (!connected && !syncTimer->isActive()) {
        emit errorOccurred(QString("SSH connection timeout, check connection settings"));
        connected = false;
        return connected;
    }

    //connect to redis 
    socket = sshClient->openTcpSocket(config.host, config.port);

    if (socket == NULL) {
        socketConnected = false;
        return socketConnected;
    }

    QObject::connect(socket, SIGNAL(readyRead()), this, SLOT(OnSocketReadyRead())); 

    syncTimer->start(config.connectionTimeout);
    syncLoop->exec();

    if (!socketConnected && !syncTimer->isActive()) {
        socketConnected = false;
        emit errorOccurred(QString("SSH connection established, but redis connection error occurred"));
        return socketConnected;
    }

    if (config.useAuth()) {
        execute(QString("AUTH %1").arg(config.auth));
    }

    return socketConnected;    
}

void RedisConnectionOverSsh::disconnect()
{
    if (sshClient.isNull())
        return;

    sshClient->disconnectFromHost();

    connected = false;
}

void RedisConnectionOverSsh::OnSshConnectionError(QxtSshClient::Error error)
{
    if (!isHostKeyAlreadyAdded && QxtSshClient::HostKeyUnknownError == error) {
        QxtSshKey hostKey = sshClient->hostKey();

        sshClient->addKnownHost(config.sshHost, hostKey);

        sshClient->resetState();

        sshClient->connectToHost(config.sshUser, config.sshHost, config.sshPort);

        isHostKeyAlreadyAdded = true;
        return;
    }

    if (syncLoop->isRunning()) {
        syncLoop->exit();
    }

    QString errorMessage; 

    switch (error) {

        case QxtSshClient::AuthenticationError:
            errorMessage = "Authentication Error";
            break;
        case QxtSshClient::HostKeyUnknownError:
            errorMessage = "Host Key Unknown Error";
            break;
        case QxtSshClient::HostKeyInvalidError:
            errorMessage = "HostKey Invalid";
            break;
        case QxtSshClient::HostKeyMismatchError:
            errorMessage = "HostKey Mismatch";
            break;
        case QxtSshClient::ConnectionRefusedError:
            errorMessage = "Connection Refused";
            break;
        case QxtSshClient::UnexpectedShutdownError:
            errorMessage = "Unexpected Shutdown";
            break;
        case QxtSshClient::HostNotFoundError:
            errorMessage = "Host Not Found";
            break;
        case QxtSshClient::SocketError:
            errorMessage = "Socket Error";
            break;
        case QxtSshClient::UnknownError:
            errorMessage = "Unknown Error";
            break;
    }

    emit errorOccurred(QString("SSH Connection error: %1").arg(errorMessage));
}

void RedisConnectionOverSsh::OnSshConnected()
{
    connected = true;

    if (syncLoop->isRunning()) {
        syncLoop->exit();
    }
}

void RedisConnectionOverSsh::OnSocketReadyRead()
{
    //on first emit
    if (!socketConnected) {
        socketConnected = true;            
    }    

    if (syncLoop->isRunning()) {
        syncLoop->exit();
    }

    // ignore signals if running blocking version
    if (!commandRunning) {
        return;
    }
    
    readingBuffer = socket->read(MAX_BUFFER_SIZE);

    if (readingBuffer.size() == 0) {
        return;
    }

    executionTimer->stop();
    resp.appendToSource(readingBuffer);        

    if (resp.isValid()) {
        return sendResponse();    
    } else {
        emit operationProgress(resp.getLoadedItemsCount(), runningCommand.getOwner());
        executionTimer->start(config.executeTimeout); //restart execution timer
    }
}


QString RedisConnectionOverSsh::getLastError()
{
    if (socket == NULL) {
        return QString("SSH connection error");
    }

    return socket->errorString();
}


QVariant RedisConnectionOverSsh::execute(QString command)
{        
    if (command.isEmpty()) {
        return QVariant();
    }

    /*
     *    Send command
     */
    QByteArray byteArray = Command::getByteRepresentation(command);
    const char* cString = byteArray.constData();

    socket->write(cString, byteArray.size());

    //wait for ready read
    syncTimer->start(config.executeTimeout);
    syncLoop->exec();

    if (!syncTimer->isActive()) {
        return QVariant();
    }

    /*
     *    Get response
     */    
    Response response;    QByteArray availableData;    
    int currExecutionTime = 0; bool dataReaded = false;

    while(!response.isValid()) {

        availableData = socket->read(MAX_BUFFER_SIZE);

        if (availableData.size() > 0) 
        {
            response.appendToSource(availableData);    

        } else {                        

            while (currExecutionTime <= config.executeTimeout) 
            {
                waitForData(5);
                currExecutionTime +=5;

                availableData = socket->read(MAX_BUFFER_SIZE);

                if (availableData.size() > 0) 
                {
                    response.appendToSource(availableData);
                    currExecutionTime = 0;
                    dataReaded = true;

                    break;
                }
            }

            if (dataReaded) {
                dataReaded = false;
                continue;
            }

            break;
        }

    }    

    return response.getValue();
}

void RedisConnectionOverSsh::runCommand(const Command &command)
{
    if (command.hasDbIndex()) {
        selectDb(command.getDbIndex());
    }

    resp.clear();
    commandRunning = true;
    runningCommand = command;
    executionTimer->start(config.executeTimeout);

    if (command.isEmpty()) {
        return sendResponse();
    }    

    /*
     *    Send command
     */
    QByteArray byteArray = command.getByteRepresentation();
    const char* cString = byteArray.constData();

    socket->write(cString, byteArray.size());
}

bool RedisConnectionOverSsh::waitForData(int ms)
{
    //wait for data
    QEventLoop loop;
    QTimer timeoutTimer;

    //configure sync objects
    timeoutTimer.setSingleShot(true);
    QObject::connect(&timeoutTimer, SIGNAL(timeout()), &loop, SLOT(quit()));

    timeoutTimer.start(ms); 
    loop.exec();

    return true;
}
