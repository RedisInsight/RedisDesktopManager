#include "abstracttransporter.h"
#include <QDebug>

RedisClient::AbstractTransporter::AbstractTransporter(RedisClient::Connection *connection)
    : m_isInitialized(false), m_isCommandRunning(false), m_connection(connection)
{
    //connect signals & slots between connection & transporter
    connect(connection, SIGNAL(addCommandToWorker(Command)), this, SLOT(addCommand(Command)));
    connect(this, SIGNAL(errorOccurred(const QString&)), connection, SIGNAL(error(const QString&)));
    connect(this, SIGNAL(logEvent(const QString&)), connection, SIGNAL(log(const QString&)));    
}

RedisClient::AbstractTransporter::~AbstractTransporter()
{
}

void RedisClient::AbstractTransporter::addCommand(Command cmd)
{
    if (cmd.isHiPriorityCommand())
        commands.prepend(cmd);
    else
        commands.enqueue(cmd);

    emit commandAdded();
    processCommandQueue();
}

void RedisClient::AbstractTransporter::cancelCommands(QObject *owner)
{    
    if (runningCommand.getOwner() == owner) {
        runningCommand.cancel();
    }

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
    if (executionTimer)
        executionTimer->stop();

    if (runningCommand.isCanceled())
        return;

    auto callback = runningCommand.getCallBack();

    if (callback && runningCommand.getOwner()) { // New API
        ResponseEmitter emiter(m_response);
        emiter.sendResponse(runningCommand.getOwner(), callback);
    }

    emit logEvent(QString("%1 > [runCommand] %2 -> response received : %3")
                  .arg(m_connection->config.name())
                  .arg(runningCommand.getRawString())
                  .arg(m_response.getLoadedItemsCount()));

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
    //todo
}

void RedisClient::AbstractTransporter::executionTimeout()
{
    emit errorOccurred("Execution timeout");

    //return sendResponse();
}
