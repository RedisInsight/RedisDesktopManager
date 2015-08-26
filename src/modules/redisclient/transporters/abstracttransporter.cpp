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
        cleanRunningCommand();
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

    if (m_emitter) { // NOTE(u_glide): Command has callback
       m_emitter->sendResponse(m_response);
    }

    logActiveResponse();
    cleanRunningCommand();
    processCommandQueue();
}

void RedisClient::AbstractTransporter::processCommandQueue()
{
    if (m_isCommandRunning || m_commands.isEmpty()) {
        return;
    }

    auto command = m_commands.dequeue();
    auto callback = command.getCallBack();
    auto owner = command.getOwner();
    if (callback && owner) {
        m_emitter = QSharedPointer<ResponseEmitter>(
                    new ResponseEmitter(owner, callback));
    }

    runCommand(command);
}

void RedisClient::AbstractTransporter::cleanRunningCommand()
{
    m_isCommandRunning = false;
    m_emitter.clear();
}

void RedisClient::AbstractTransporter::logActiveResponse()
{
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
}

void RedisClient::AbstractTransporter::executionTimeout()
{
    emit errorOccurred("Execution timeout");
}
