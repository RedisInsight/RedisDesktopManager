#include "setkey.h"
#include "modules/redisclient/command.h"
#include "modules/redisclient/commandexecutor.h"

SetKeyModel::SetKeyModel(QSharedPointer<RedisClient::Connection> connection, QString fullPath, int dbIndex, int ttl)
       : ListLikeKeyModel(connection, fullPath, dbIndex, ttl)
{
    loadRowCount();
}

QString SetKeyModel::getType()
{
    return "set";
}

void SetKeyModel::updateRow(int rowIndex, const QVariantMap &row)
{
    if (!isRowLoaded(rowIndex) || !isRowValid(row))
        throw Exception("Invalid row");

    QByteArray cachedRow = m_rowsCache[rowIndex];
    QByteArray newRow(row["value"].toByteArray());

    // delete old value
    deleteSetRow(cachedRow);
    m_rowsCache.removeAt(rowIndex);

    // add new value
    addSetRow(newRow);
    m_rowsCache.insert(rowIndex, newRow);
}

void SetKeyModel::addRow(const QVariantMap &row)
{
    if (!isRowValid(row))
        throw Exception("Invalid row");

    addSetRow(row["value"].toByteArray());
    m_rowCount++;
}

void SetKeyModel::loadRows(unsigned long rowStart, unsigned long, std::function<void ()> callback)
{
    if (isPartialLoadingSupported()) {
        QSharedPointer<RedisClient::ScanCommand> cmd(new RedisClient::ScanCommand(
                                                         QString("SSCAN %1 0").arg(m_keyFullPath),
                                                         this, m_dbIndex));

        m_connection->retrieveCollection(cmd, [this, callback, rowStart](QVariant result) {
            if (result.type() == QMetaType::QVariantList) {
                QVariantList rows = result.toList();

                foreach (QVariant row, rows) {
                    m_rowsCache.push_back(row.toByteArray());
                }
            }
            callback();
        });
    } else {

        if (!m_rowsCache.isEmpty())
            return;

        QVariantList rows = getRowsRange("SMEMBERS").toList();

        foreach (QVariant row, rows) {
            m_rowsCache.push_back(row.toByteArray());
        }
    }

    callback();
}

void SetKeyModel::removeRow(int i)
{
    if (!isRowLoaded(i))
        return;

    QByteArray value = m_rowsCache.value(i);   
    deleteSetRow(value);

    m_rowCount--;
    m_rowsCache.removeAt(i);

    setRemovedIfEmpty();
}

void SetKeyModel::loadRowCount()
{
    m_rowCount = getRowCount("SCARD");
}

void SetKeyModel::addSetRow(const QByteArray &value)
{
    using namespace RedisClient;
    Command addCmd(m_dbIndex);
    (addCmd << "SADD" << m_keyFullPath).append(value);
    CommandExecutor::execute(m_connection, addCmd);
}

RedisClient::Response SetKeyModel::deleteSetRow(const QByteArray &value)
{
    using namespace RedisClient;
    Command deleteCmd(m_dbIndex);
    (deleteCmd << "SREM" << m_keyFullPath).append(value);
    return CommandExecutor::execute(m_connection, deleteCmd);
}
