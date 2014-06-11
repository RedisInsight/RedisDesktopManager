#include "abstracttransporter.h"

RedisClient::AbstractTransporter::AbstractTransporter(RedisClient::Connection *connection)
    : m_isInitialized(false), m_isCommandRunning(false), m_connection(connection)
{
    //connect signals & slots between connection & transporter
    connect(connection, SIGNAL(addCommandToWorker(const Command&)), this, SLOT(addCommand(const Command&)));
    connect(this, SIGNAL(errorOccurred(const QString&)), connection, SIGNAL(error(const QString&)));
    connect(this, SIGNAL(logEvent(const QString&)), connection, SIGNAL(log(const QString&)));    
}

RedisClient::AbstractTransporter::~AbstractTransporter()
{
    QListIterator<Command> cmd(commands);

    while (cmd.hasNext())
    {
        auto currentCommand = cmd.next();

        QObject::disconnect(currentCommand.getOwner(), SIGNAL(destroyed(QObject *)),
                   this, SLOT(cancelCommands(QObject *)));
    }
}

void RedisClient::AbstractTransporter::addCommand(const Command &cmd)
{
    connect(cmd.getOwner(), SIGNAL(destroyed(QObject *)),
            this, SLOT(cancelCommands(QObject *))
            );

    commands.enqueue(cmd);
    emit commandAdded();
    processCommandQueue();
}

void RedisClient::AbstractTransporter::cancelCommands(QObject *owner)
{
    if (runningCommand.getOwner() == owner)
        runningCommand.cancel();

    QListIterator<Command> cmd(commands);

    while (cmd.hasNext())
    {
        auto currentCommand = cmd.next();

        if (currentCommand.getOwner() == owner) {
            currentCommand.cancel();
        }
    }
}

void RedisClient::AbstractTransporter::sendResponse()
{
    executionTimer->stop();

    if (runningCommand.isCanceled())
        return;

    QString callbackName = runningCommand.getCallbackName();

    QMetaObject::invokeMethod(
        runningCommand.getOwner(), callbackName.toUtf8().constData(),
        Qt::AutoConnection, Q_ARG(RedisClient::Response, m_response)
        );

    emit logEvent(QString("%1 > [runCommand] %2 -> response recieved").arg(m_connection->config.name).arg(runningCommand.getRawString()));

    m_isCommandRunning = false;

    processCommandQueue();
}

void RedisClient::AbstractTransporter::processCommandQueue()
{
    if (m_isCommandRunning || commands.isEmpty()) {
        return;
    }

    runCommand(commands.dequeue());
}

void RedisClient::AbstractTransporter::sendProgressValue()
{
    QString callbackName = runningCommand.getProgressCallbackName();

    if (callbackName.isNull() || callbackName.isEmpty())
        return;

    QMetaObject::invokeMethod(
        runningCommand.getOwner(), callbackName.toUtf8().constData(),
        Qt::AutoConnection, Q_ARG(int, m_response.getLoadedItemsCount())
        );
}

void RedisClient::AbstractTransporter::executionTimeout()
{
    emit errorOccurred("Execution timeout");

    //return sendResponse();
}
