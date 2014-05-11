#include "command.h"

RedisClient::Command::Command()
    : owner(nullptr), commandWithArguments(""), dbIndex(-1), commandCanceled(false)
{
    
}

RedisClient::Command::Command(const QString& cmdString, QObject * owner, int db)
    : owner(owner), commandWithArguments(splitCommandString(cmdString)), 
    dbIndex(db), commandCanceled(false)
{
}

RedisClient::Command::Command(const QStringList& cmd, QObject * owner, int db)
    : owner(owner), commandWithArguments(cmd), 
    dbIndex(db), commandCanceled(false)
{
}

RedisClient::Command::Command(const QString& cmdString, QObject * owner, const QString& invokeMethod, int db)
    : owner(owner), commandWithArguments(splitCommandString(cmdString)), 
    dbIndex(db), callBackMethod(invokeMethod), commandCanceled(false)
{

}

RedisClient::Command::Command(const QStringList& cmd, QObject * owner, const QString& invokeMethod, int db)
    : owner(owner), commandWithArguments(cmd), 
    dbIndex(db), callBackMethod(invokeMethod), commandCanceled(false)
{

}

QStringList RedisClient::Command::splitCommandString(const QString &command)
{
    QStringList parts = QStringList();
    int i = 0;
    bool inQuote = false;
    QString part = QString();
    while (i < command.length())
    {
        if(command.at(i).isSpace() && !inQuote)
        {
            if (part.length() > 0)
                parts.append(part);
            part = QString();
        }
        else if (command.at(i) == QChar('"'))
        {
            if (inQuote)
                parts.append(part);
            part = QString();
            inQuote = !inQuote;
        }
        else
        {
            part.append(command.at(i));
        }
        ++i;
    }
    if (parts.length() < 1 || part.length() > 0)
        parts.append(part);

    return parts;
}

bool RedisClient::Command::hasCallback() const
{
    return !callBackMethod.isEmpty();
}

QString RedisClient::Command::getCallbackName()
{
    return callBackMethod;
}

QString RedisClient::Command::getProgressCallbackName()
{
    return progressMethod;
}

void RedisClient::Command::setCallBackName(const QString & name)
{
    callBackMethod = name;
}

void RedisClient::Command::setProgressCallBackName(const QString &name)
{
    progressMethod = name;
}

bool RedisClient::Command::hasDbIndex() const
{
    return dbIndex >= 0;
}

bool RedisClient::Command::isSelectCommand(int *dbIndex = nullptr) const
{
    if (commandWithArguments.length() < 2)
        return false;

    if (commandWithArguments.at(0).toLower() == "select") {

        if (dbIndex != nullptr)
            *dbIndex = commandWithArguments.at(0).toInt();

        return true;
    }

    return false;
}

int RedisClient::Command::getDbIndex() const
{
    return dbIndex;
}

QString RedisClient::Command::getRawString() const
{
    return commandWithArguments.join(' ');
}

bool RedisClient::Command::isEmpty() const
{
    return commandWithArguments.isEmpty();
}

QObject * RedisClient::Command::getOwner() const
{
    return owner;
}

void RedisClient::Command::setOwner(QObject * o)
{
    owner = o;
}

QByteArray RedisClient::Command::getByteRepresentation() const
{
    QByteArray result;
    result.append(QString("*%1\r\n").arg(commandWithArguments.length()));

    QByteArray partArray;

    for (QString part : commandWithArguments) {
        partArray = part.toUtf8();
        result.append("$");
        result.append(QString::number(partArray.size()));
        result.append("\r\n");
        result.append(partArray);
        result.append("\r\n");
    }

    return result;
}

void RedisClient::Command::cancel()
{
    commandCanceled = true;
}

bool RedisClient::Command::isCanceled() const
{
    return commandCanceled;
}

bool RedisClient::Command::isValid() const
{
    return !isCanceled() && !isEmpty()
            && hasCallback() && getOwner() != nullptr;
}
