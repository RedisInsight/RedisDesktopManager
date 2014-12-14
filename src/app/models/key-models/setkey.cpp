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
    if (isRowValid(row))
        throw Exception("Invalid row");

    addSetRow(row["value"].toString());
}

void SetKeyModel::loadRows(unsigned long rowStart, unsigned long count, std::function<void ()> callback)
{
    if (isPartialLoadingSupported()) {
        //TBD
    } else {

        if (!m_rowsCache.isEmpty())
            return;

        QStringList rows = getRowsRange("SMEMBERS").toStringList();        

        foreach (QString row, rows) {
            m_rowsCache.push_back(row.toUtf8());
        }
    }

    callback();
}

void SetKeyModel::removeRow(int i)
{
    if (!isRowLoaded(i))
        return;

    QByteArray value = m_rowsCache.value(i);

    using namespace RedisClient;

    Command deleteValues(QStringList() << "SREM" << m_keyFullPath << value, m_dbIndex);
    Response result = CommandExecutor::execute(m_connection, deleteValues);

    m_rowCount--;
    m_rowsCache.removeAt(i);
    Q_UNUSED(result);

    setRemovedIfEmpty();
}

void SetKeyModel::loadRowCount()
{
    m_rowCount = getRowCount("SCARD");
}

void SetKeyModel::addSetRow(const QString &value)
{
    using namespace RedisClient;
    Command addCmd(QStringList() << "SADD" << m_keyFullPath << value, m_dbIndex);
    CommandExecutor::execute(m_connection, addCmd);
}

void SetKeyModel::deleteSetRow(const QString &value)
{
    using namespace RedisClient;
    Command addCmd(QStringList() << "SREM" << m_keyFullPath << value, m_dbIndex);
    CommandExecutor::execute(m_connection, addCmd);
}
