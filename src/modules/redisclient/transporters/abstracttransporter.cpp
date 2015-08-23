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
        m_commands.prepend(cmd);
    else
        m_commands.enqueue(cmd);

    emit commandAdded();
    processCommandQueue();
}

void RedisClient::AbstractTransporter::cancelCommands(QObject *owner)
{    
    if (m_isCommandRunning && m_runningCommand.getOwner() == owner) {
        m_runningCommand.cancel();
        emit logEvent("Running command was canceled.");
    }

    QListIterator<Command> cmd(m_commands);

    while (cmd.hasNext())
    {
        auto currentCommand = cmd.next();

        if (currentCommand.getOwner() == owner) {
            currentCommand.cancel();
            emit logEvent("Command was canceled.");
        }
    }    
}

void RedisClient::AbstractTransporter::sendResponse()
{
    if (m_executionTimer)
        m_executionTimer->stop();

    if (m_runningCommand.isCanceled())
        return;

    auto callback = m_runningCommand.getCallBack();

    if (callback && m_runningCommand.getOwner()) { // New API
        ResponseEmitter emiter(m_response);
        emiter.sendResponse(m_runningCommand.getOwner(), callback);
    }

    QString result;

    if (m_response.isErrorMessage()) {
        result = m_response.toString();
    } else if (m_response.isOkMessage()) {
        result = "OK";
    } else {
        result = QString("Loaded items: %1").arg(
                    m_response.getLoadedItemsCount());
    }

    emit logEvent(QString("%1 > [runCommand] %2 -> response received : %3")
                  .arg(m_connection->config.name())
                  .arg(m_runningCommand.getRawString())
                  .arg(result));

    m_isCommandRunning = false;

    processCommandQueue();
}

void RedisClient::AbstractTransporter::processCommandQueue()
{
    if (m_isCommandRunning || m_commands.isEmpty()) {
        return;
    }

    runCommand(m_commands.dequeue());
}

void RedisClient::AbstractTransporter::executionTimeout()
{
    emit errorOccurred("Execution timeout");
}
