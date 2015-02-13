#include "command.h"
#include <QSet>

RedisClient::Command::Command()
    : m_owner(nullptr), m_commandWithArguments(), dbIndex(-1), commandCanceled(false)
{
    
}

RedisClient::Command::Command(const QString& cmdString, QObject * owner, int db)
    : m_owner(owner), m_commandWithArguments(splitCommandString(cmdString)),
    dbIndex(db), commandCanceled(false)
{
}

RedisClient::Command::Command(const QStringList& cmd, QObject * owner, int db)
    : m_owner(owner), m_commandWithArguments(convertStringList(cmd)),
    dbIndex(db), commandCanceled(false)
{
}

RedisClient::Command::Command(const QStringList &cmd, QObject *owner, std::function<void (RedisClient::Response)> callback, int db)
    : m_owner(owner), m_commandWithArguments(convertStringList(cmd)),
      dbIndex(db), commandCanceled(false), m_callback(callback)
{

}

RedisClient::Command::Command(const QStringList& cmd, int db)
    : m_owner(nullptr), m_commandWithArguments(convertStringList(cmd)),
      dbIndex(db), commandCanceled(false)
{

}

RedisClient::Command::Command(int db)
    : m_owner(nullptr), m_commandWithArguments(),
      dbIndex(db), commandCanceled(false)
{

}

RedisClient::Command::~Command()
{

}

RedisClient::Command &RedisClient::Command::operator <<(const QString &part)
{
    this->m_commandWithArguments.append(part.toUtf8());

    return *this;
}

RedisClient::Command &RedisClient::Command::append(const QByteArray &part)
{
    m_commandWithArguments.append(part);

    return *this;
}

QList<QByteArray> RedisClient::Command::splitCommandString(const QString &command)
{
    QList<QByteArray> parts;
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
                parts.append(part.toUtf8());
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
                parts.append(part.toUtf8());
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
        parts.append(part.toUtf8());

    return parts;
}

bool RedisClient::Command::hasCallback() const
{
    return (bool)m_callback;
}

void RedisClient::Command::setCallBack(QObject *context, std::function<void (RedisClient::Response)> callback)
{
    m_owner = context;
    m_callback = callback;
}

std::function<void (RedisClient::Response)> RedisClient::Command::getCallBack() const
{
    return m_callback;
}

bool RedisClient::Command::hasDbIndex() const
{
    return dbIndex >= 0;
}

bool RedisClient::Command::isSelectCommand() const
{
    if (m_commandWithArguments.length() < 2)
        return false;

    return m_commandWithArguments.at(0).toLower() == "select";
}

int RedisClient::Command::getDbIndex() const
{
    return dbIndex;
}

QObject *RedisClient::Command::getOwner() const
{
    return m_owner;
}

QString RedisClient::Command::getRawString() const
{
    return m_commandWithArguments.join(' ');
}

QList<QByteArray> RedisClient::Command::getSplitedRepresentattion() const
{
    return m_commandWithArguments;
}

QString RedisClient::Command::getPartAsString(int i)
{
    if (m_commandWithArguments.size() <= i)
        return QString();

    return QString::fromUtf8(m_commandWithArguments.at(i));
}

bool RedisClient::Command::isEmpty() const
{
    return m_commandWithArguments.isEmpty();
}

QByteArray RedisClient::Command::getByteRepresentation() const
{
    QByteArray result;
    result.append(QString("*%1\r\n").arg(m_commandWithArguments.length()));    

    for (QByteArray partArray : m_commandWithArguments) {
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


QList<QByteArray> RedisClient::convertStringList(const QStringList &list)
{
    QList<QByteArray> result;

    for(QString line : list) {
        result.append(line.toUtf8());
    }

    return result;
}
