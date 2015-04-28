#include "abstractkey.h"

#include <QVariant>
#include "modules/redisclient/redisclient.h"

KeyModel::KeyModel(QSharedPointer<RedisClient::Connection> connection, QString fullPath, int dbIndex, int ttl)
    : m_connection(connection), m_keyFullPath(fullPath), m_dbIndex(dbIndex),
      m_ttl(ttl), m_isKeyRemoved(false), m_rowCount(-1), m_currentState(KeyModel::State::Initial)
{

}

QString KeyModel::getKeyName()
{
    return m_keyFullPath;
}

int KeyModel::getTTL()
{
    return m_ttl;
}

QString KeyModel::getState()
{
    switch (m_currentState) {
        case Initial: return "initital";
        case DataLoaded: return "loaded";
        case Error:
        default: return "error";
    }
}

bool KeyModel::isPartialLoadingSupported()
{
    return m_connection->getServerVersion() >= 2.8;
}

unsigned long KeyModel::rowsCount()
{
    return m_rowCount;
}

void KeyModel::setKeyName(const QString &newKeyName)
{
    auto cmd = RedisClient::Command(m_dbIndex)
               << "RENAMENX" << m_keyFullPath << newKeyName;

    RedisClient::Response result;

    try {
        result = RedisClient::CommandExecutor::execute(m_connection, cmd);
    } catch (const RedisClient::CommandExecutor::Exception& e) {
        throw Exception("Connection error: " + QString(e.what()));
    }

    if (result.getValue().toInt() == 0) {
        throw Exception("Key with new name already exist in database");
    }

    m_keyFullPath = newKeyName;
}

void KeyModel::setTTL(int)
{
    // TBD
}

void KeyModel::removeKey()
{   
    auto cmd = RedisClient::Command(m_dbIndex);
    cmd << "DEL" << m_keyFullPath << m_keyFullPath;

    RedisClient::Response result;

    try {
        result = RedisClient::CommandExecutor::execute(m_connection, cmd);
    } catch (const RedisClient::CommandExecutor::Exception& e) {
        throw Exception("Connection error: " + QString(e.what()));
    }

    m_isKeyRemoved = true;
    emit removed();
}

int KeyModel::getRowCount(const QString &countCmd)
{
    RedisClient::Command updateCmd(m_dbIndex);
    updateCmd << countCmd << m_keyFullPath;

    RedisClient::Response result;

    try {
        result = RedisClient::CommandExecutor::execute(m_connection, updateCmd);
    } catch (const RedisClient::CommandExecutor::Exception& e) {
        throw Exception("Connection error: " + QString(e.what()));
    }

    if (result.getType() == RedisClient::Response::Integer) {
        return result.getValue().toUInt();
    }

    return -1;
}

//FIXME: make this call async
QVariant KeyModel::getRowsRange(const QString &baseCmd, unsigned long rowStart, unsigned long count)
{
    QStringList cmd;

    if (rowStart == 0 && count == 0) {
        cmd << baseCmd << m_keyFullPath;
    } else {
        unsigned long rowEnd = std::min(m_rowCount, rowStart + count) - 1;

        if (baseCmd.contains(QChar(' '))) {
            QStringList suffixCmd(baseCmd.split(QChar(' ')));

            cmd << suffixCmd.at(0);
            suffixCmd.removeFirst();
            cmd << m_keyFullPath << QString::number(rowStart) << QString::number(rowEnd);
            cmd += suffixCmd;

        } else {
            cmd << baseCmd << m_keyFullPath << QString::number(rowStart) << QString::number(rowEnd);
        }
    }

    RedisClient::Command updateCmd(cmd, m_dbIndex);
    RedisClient::Response result;

    try {
        result = RedisClient::CommandExecutor::execute(m_connection, updateCmd);
    } catch (const RedisClient::CommandExecutor::Exception& e) {
        throw Exception("Connection error: " + QString(e.what()));
    }

    if (result.getType() != RedisClient::Response::MultiBulk) {
        throw Exception("getRowsRange() error - can't load values from server");
    }

    return result.getValue();
}

bool KeyModel::isRowValid(const QVariantMap &row)
{
    if (row.isEmpty())
        return false;

    QSet<QString> validKeys;

    foreach (QByteArray role, getRoles().values()) {
         validKeys.insert(role);
    }

    QMapIterator<QString, QVariant> i(row);

    while (i.hasNext()) {
        i.next();

        if(!validKeys.contains(i.key()))
            return false;
    }

    return true;
}

void KeyModel::setRemovedIfEmpty()
{
    if (m_rowCount == 0) {
        m_isKeyRemoved = true;
        emit removed();
    }
}


ListLikeKeyModel::ListLikeKeyModel(QSharedPointer<RedisClient::Connection> connection,
                                   QString fullPath, int dbIndex, int ttl)
    : KeyModel(connection, fullPath, dbIndex, ttl)
{

}

QStringList ListLikeKeyModel::getColumnNames()
{
    return QStringList() << "row"  << "value";
}

QHash<int, QByteArray> ListLikeKeyModel::getRoles()
{
    QHash<int, QByteArray> roles;
    roles[Roles::Value] = "value";
    roles[Roles::RowNumber] = "row";
    roles[Roles::BinaryValue] = "binary_value";
    return roles;
}

QVariant ListLikeKeyModel::getData(int rowIndex, int dataRole)
{
    if (!isRowLoaded(rowIndex))
        return QVariant();

    switch (dataRole) {
        case Value:
            return m_rowsCache[rowIndex];
        case RowNumber:
            return QString::number(rowIndex+1);
        case BinaryValue:
            return valueToBinary(m_rowsCache[rowIndex]);
    }

    return QVariant();

    return m_rowsCache[rowIndex];
}

void ListLikeKeyModel::clearRowCache()
{
    m_rowsCache.clear();
}

bool ListLikeKeyModel::isRowLoaded(int rowIndex)
{
    return 0 <= rowIndex && rowIndex < m_rowsCache.size();
}

bool ListLikeKeyModel::isMultiRow() const
{
    return true;
}

QVariant valueToBinary(const QByteArray &value)
{
   QVariantList list;

   for(int index=0; index < value.length(); ++index) {
       list.append(QVariant((unsigned char)value.at(index)));
   }
   return QVariant(list);
}
