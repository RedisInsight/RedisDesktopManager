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

void KeyModel::setKeyName(const QString &newKeyName)
{
    QStringList renameCommand;

    renameCommand << "RENAME" << m_keyFullPath << newKeyName;

    //db->runCommand(RedisClient::Command(renameCommand, this, "loadedRenameStatus", dbIndex));
    // TBD
}

void KeyModel::setTTL(int)
{
    // TBD
}

void KeyModel::removeKey()
{
    QStringList deleteCommand;

    deleteCommand << "DEL" << m_keyFullPath;

    //db->runCommand(RedisClient::Command(deleteCommand, this, "loadedDeleteStatus", dbIndex));
    //TBD
}

int KeyModel::getRowCount(const QString &countCmd)
{
    RedisClient::Command updateCmd(QStringList() << countCmd << m_keyFullPath, m_dbIndex);
    RedisClient::Response result = RedisClient::CommandExecutor::execute(m_connection, updateCmd);

    if (result.getType() == RedisClient::Response::Integer) {
        return result.getValue().toUInt();
    }

    return -1;
}

QVariant KeyModel::getRowsRange(const QString &baseCmd, unsigned long rowStart, unsigned long count)
{
    QStringList cmd;

    if (rowStart == -1 && count == -1) {
        cmd = (QStringList() << baseCmd << m_keyFullPath);
    } else {
        unsigned long rowEnd = std::min(m_rowCount, rowStart + count) - 1;

        if (baseCmd.contains(QChar(' '))) {
            QStringList suffixCmd = baseCmd.split(QChar(' '));

            cmd << suffixCmd.at(0);
            suffixCmd.removeFirst();
            cmd << m_keyFullPath << QString::number(rowStart) << QString::number(rowEnd);
            cmd += suffixCmd;

        } else {
            cmd = (QStringList() << baseCmd << m_keyFullPath << QString::number(rowStart) << QString::number(rowEnd));
        }
    }

    RedisClient::Command updateCmd(cmd, m_dbIndex);
    RedisClient::Response result = RedisClient::CommandExecutor::execute(m_connection, updateCmd);

    if (result.getType() != RedisClient::Response::MultiBulk) {
        throw Exception("getRowsRange() error - can't load values from server");
    }

    return result.getValue();
}


