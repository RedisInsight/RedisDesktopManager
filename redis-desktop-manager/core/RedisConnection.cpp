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
        if (config.useAuth()) {
            execute(QString("AUTH %1").arg(config.auth));
        }

        connected = (execute("PING") == "PONG");

        if (!connected)
            emit errorOccurred("Redis server require password or password invalid");

    } else {
        connected = false;
        emit errorOccurred("Connection timeout");
    }    

    if (connected) {
        emit log(QString("%1 > connected").arg(config.name));
    } else {
        emit log(QString("%1 > connection failed").arg(config.name));
        disconnect();
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
    QByteArray cmd = Command(command).getByteRepresentation();
    socket->write(cmd);
    socket->flush();    

    emit log(QString("%1 > [execute] %2").arg(config.name).arg(command));

    if (!socket->waitForReadyRead(config.executeTimeout)) {

        QAbstractSocket::SocketError error = socket->error();

        if (error == QAbstractSocket::UnknownSocketError && connect()) {
            return execute(command);
        } else {
            emit errorOccurred("Execution timeout exceeded");
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
                emit log(QString("%1 > [execute] %2 -> response partially received. Execution timeout").arg(config.name).arg(command));
                break;
            }
        }

    }    

    emit log(
        QString("%1 > [execute] %2 -> response received: \n %3")
        .arg(config.name)
        .arg(command)
        .arg(response.toString()));

    return response.getValue();
}


void RedisConnection::runCommand(const Command &command)
{
    emit log(QString("%1 > [runCommand] %2").arg(config.name).arg(command.getRawString()));

    if (command.isEmpty()
        || (command.hasDbIndex() && !selectDb(command.getDbIndex())) ) {
            return sendResponse();
    } 

    resp.clear();
    commandRunning = true;
    runningCommand = command;
    executionTimer->start(config.executeTimeout);  

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
