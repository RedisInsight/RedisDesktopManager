#include "command.h"
#include <QSet>

RedisClient::Command::Command()
    : owner(nullptr), commandWithArguments(), dbIndex(-1), commandCanceled(false)
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

RedisClient::Command::Command(const QStringList &cmd, QObject *owner, std::function<void (RedisClient::Response)> callback, int db)
    : owner(owner), commandWithArguments(cmd),
      dbIndex(db), commandCanceled(false), m_callback(callback)
{

}

RedisClient::Command::Command(const QStringList &cmd, int db)
    : owner(nullptr), commandWithArguments(cmd),
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
    QSet<QChar> delimiters;
    delimiters << QChar('"') << QChar('\'');
    QChar currentDelimiter = '\0';

    while (i < command.length())
    {
        if(command.at(i).isSpace() && !inQuote)
        {
            if (part.length() > 0)
                parts.append(part);
            part = QString();
        }
        else if (delimiters.contains(command.at(i))
                 && (!inQuote || currentDelimiter == command.at(i)))
        {
            if (i > 0 && command.at(i-1) == QChar('\\')) {
                part.remove(part.size()-1, 1);
                part.append(command.at(i++));
                continue;
            }

            if (inQuote) {
                parts.append(part);
                currentDelimiter = '\0';
            } else {
                currentDelimiter = command.at(i);
            }

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

void RedisClient::Command::setCallBack(QObject *context, std::function<void (RedisClient::Response)> callback)
{
    owner = context;
    m_callback = callback;
}

std::function<void (RedisClient::Response)> RedisClient::Command::getCallBack()
{
    return m_callback;
}

bool RedisClient::Command::hasDbIndex() const
{
    return dbIndex >= 0;
}

bool RedisClient::Command::isSelectCommand() const
{
    if (commandWithArguments.length() < 2)
        return false;

    return commandWithArguments.at(0).toLower() == "select";
}

int RedisClient::Command::getDbIndex() const
{
    return dbIndex;
}

QString RedisClient::Command::getRawString() const
{
    return commandWithArguments.join(' ');
}

QStringList RedisClient::Command::getSplitedRepresentattion() const
{
    return commandWithArguments;
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
            /*&& hasCallback()*/ /* && getOwner() != nullptr*/;
}
