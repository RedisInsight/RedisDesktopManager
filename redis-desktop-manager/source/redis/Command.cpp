#include "Command.h"

Command::Command()
    : owner(nullptr), commandWithArguments(""), dbIndex(-1)
{
    
}

Command::Command(const QString& cmdString, QObject * owner, int db)
    : owner(owner), commandWithArguments(splitCommandString(cmdString)), dbIndex(db)
{
}

Command::Command(const QStringList& cmd, QObject * owner, int db)
    : owner(owner), commandWithArguments(cmd), dbIndex(db)
{
}

Command::Command(const QString& cmdString, QObject * owner, const QString& invokeMethod, int db)
    : owner(owner), commandWithArguments(splitCommandString(cmdString)), dbIndex(db), callBackMethod(invokeMethod)
{

}

Command::Command(const QStringList& cmd, QObject * owner, const QString& invokeMethod, int db)
    : owner(owner), commandWithArguments(cmd), dbIndex(db), callBackMethod(invokeMethod)
{

}

QStringList Command::splitCommandString(const QString &cmd)
{
    //todo implement normal parsing
    return cmd.split(" ", QString::SkipEmptyParts, Qt::CaseInsensitive);
}

bool Command::hasCallback()
{
    return !callBackMethod.isEmpty();
}

QString Command::getCallbackName()
{
    return callBackMethod;
}

void Command::setCallBackName(const QString & name)
{
    callBackMethod = name;
}

bool Command::hasDbIndex() const
{
    return dbIndex >= 0;
}

int Command::getDbIndex() const
{
    return dbIndex;
}

QString Command::getFormattedString() const
{
    return Command::getFormatted(commandWithArguments);
}

QString Command::getRawString() const
{
    return commandWithArguments.join(' ');
}

bool Command::isEmpty() const
{
    return commandWithArguments.isEmpty();
}

QObject * Command::getOwner()
{
    return owner;
}

void Command::setOwner(QObject * o)
{
    owner = o;
}

QString Command::getFormatted(const QString& command)
{
    QStringList parts = command.split(" ", QString::SkipEmptyParts, Qt::CaseInsensitive);

    QString formattedCmd("*");
    formattedCmd.append(QString("%1\r\n").arg(parts.length()));

    for (QString part : parts) {
        formattedCmd.append(QString("$%1\r\n%2\r\n")
            .arg(QString("%1").arg(part.length()), part));
    }

    return formattedCmd;
}

QString Command::getFormatted(const QStringList& command)
{
    QString formattedCmd("*");
    formattedCmd.append(QString("%1\r\n").arg(command.length()));

    for (QString part : command) {
        formattedCmd.append(QString("$%1\r\n%2\r\n")
            .arg(QString("%1").arg(part.length()), part));
    }

    return formattedCmd;
}

