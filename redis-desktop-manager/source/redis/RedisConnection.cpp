#include "RedisConnection.h"
#include "Command.h"


RedisConnection::RedisConnection(const RedisConnectionConfig & c) 
    : RedisConnectionAbstract(c), socket(nullptr)
{    
}

void RedisConnection::init()
{
    if (socket != nullptr) {
        return;
    }

    RedisConnectionAbstract::init();

    socket = QSharedPointer<QTcpSocket>(new QTcpSocket());

    QObject::connect(socket.data(), SIGNAL(readyRead()), this, SLOT(readyRead()));
    QObject::connect(socket.data(), SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(error(QAbstractSocket::SocketError)));    
}

RedisConnection::~RedisConnection()
{    
}

bool RedisConnection::connect()
{
    init();

    socket->connectToHost(config.host, config.port);

    if (socket->waitForConnected(config.connectionTimeout)) 
    {
        connected = true;

        if (config.useAuth()) {
            execute(QString("AUTH %1").arg(config.auth));
        }
    } else {
        connected = false;
    }

    return connected;
}

void RedisConnection::disconnect()
{
    if (socket.isNull())
        return;

    socket->disconnectFromHost();

    connected = false;
}

QString RedisConnection::getLastError()
{
    return socket->errorString();
}

QVariant RedisConnection::execute(QString command)
{        
    if (command.isEmpty()) {
        return QVariant();
    }
    
    // Send command
    QByteArray cmd = Command::getByteRepresentation(command);
    socket->write(cmd);
    socket->flush();    

    if (!socket->waitForReadyRead(config.executeTimeout)) {

        QAbstractSocket::SocketError error = socket->error();

        if (error == QAbstractSocket::UnknownSocketError && connect()) {
            return execute(command);
        }

        return QVariant();
    }

    /*
     *    Get response
     */    
    Response response; QByteArray res;

    while(!response.isValid()) {    

        if (socket->bytesAvailable() > 0) 
        {
            res = socket->readAll();
            response.appendToSource(res);    

        } else {
            
            // TODO: move config.executeTimeout to config options - user probably want to increase this value for unstable connections            

            if (!socket->waitForReadyRead(config.executeTimeout)) 
            {
                break;
            }
        }

    }    

    return response.getValue();
}


void RedisConnection::runCommand(const Command &command)
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

    // Send command
    QByteArray cmd = command.getByteRepresentation();
    socket->write(cmd);
    socket->flush();  
}

void RedisConnection::readyRead()
{
    // ignore signals if running blocking version
    if (!commandRunning || socket->bytesAvailable() <= 0) {
        return;
    }
    
    executionTimer->stop();
    readingBuffer = socket->readAll();
    resp.appendToSource(readingBuffer);        

    if (resp.isValid()) {
        return sendResponse();    
    } else {
        emit operationProgress(resp.getLoadedItemsCount(), runningCommand.getOwner());
        executionTimer->start(config.executeTimeout); //restart execution timer
    }
}

void RedisConnection::error(QAbstractSocket::SocketError error)
{
    // ignore signals if running blocking version
    if (!commandRunning && connected) {
        return;
    }

    if (error == QAbstractSocket::UnknownSocketError && connect()) {
        return runCommand(runningCommand);
    }

    emit errorOccurred(
        QString("Connection error: %1").arg(socket->errorString())
        );

    return sendResponse();    
}