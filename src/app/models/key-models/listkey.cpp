#include "listkey.h"
#include "modules/redisclient/command.h"
#include "modules/redisclient/commandexecutor.h"

ListKeyModel::ListKeyModel(QSharedPointer<RedisClient::Connection> connection, QString fullPath, int dbIndex, int ttl)
    : ListLikeKeyModel(connection, fullPath, dbIndex, ttl)
{    
    loadRowCount();
}

QString ListKeyModel::getType()
{
    return "list";
}

void ListKeyModel::updateRow(int rowIndex, const QVariantMap &row)
{
    if (!isRowLoaded(rowIndex) || !isRowValid(row))
        throw Exception("Invalid row");

    if (isActualPositionChanged(rowIndex))
        throw Exception("Can't delete row from list, because row already has changed."
                        " Reload values and try again.");

    QByteArray newRow(row["value"].toByteArray());
    setListRow(rowIndex, newRow);
    m_rowsCache.replace(rowIndex,newRow);
}

void ListKeyModel::addRow(const QVariantMap &row)
{
    if (!isRowValid(row))
        throw Exception("Invalid row");

    addListRow(row["value"].toByteArray());
    m_rowCount++;
}

void ListKeyModel::loadRows(unsigned long rowStart, unsigned long count, std::function<void ()> callback)
{           
    QVariantList rows = getRowsRange("LRANGE", rowStart, count).toList();

    foreach (QVariant row, rows) {
        m_rowsCache.push_back(row.toByteArray());
    }

    callback();
}

void ListKeyModel::removeRow(int i)
{
    if (!isRowLoaded(i))
        return;

    if (isActualPositionChanged(i))
        throw Exception("Can't delete row from list, because row already has changed."
                        " Reload values and try again.");

    // Replace value by system string
    QString customSystemValue("---VALUE_REMOVED_BY_RDM---");        
    setListRow(i, customSystemValue.toUtf8());

    // Remove all system values from list
    deleteListRow(0, customSystemValue.toUtf8());

    m_rowCount--;
    m_rowsCache.removeAt(i);

    if (m_rowCount == 0) {
        m_isKeyRemoved = true;
        emit removed();
    }
}

void ListKeyModel::loadRowCount()
{    
    m_rowCount = getRowCount("LLEN");
}

bool ListKeyModel::isActualPositionChanged(int row)
{
    using namespace RedisClient;

    QByteArray cachedValue = m_rowsCache[row];

    // check position update
    Command getValueByIndex(QStringList()
                            << "LRANGE" << m_keyFullPath
                            << QString::number(row) << QString::number(row),
                            m_dbIndex);

    Response result;

    try {
        result = CommandExecutor::execute(m_connection, getValueByIndex);
    } catch (const RedisClient::CommandExecutor::Exception& e) {
        throw Exception("Connection error: " + QString(e.what()));
    }

    QVariantList currentState = result.getValue().toList();

    return currentState.size() != 1 || currentState[0].toByteArray() != QString(cachedValue);
}

void ListKeyModel::addListRow(const QByteArray &value)
{
    using namespace RedisClient;
    Command addCmd(m_dbIndex);
    (addCmd << "LPUSH" << m_keyFullPath).append(value);

    try {
        CommandExecutor::execute(m_connection, addCmd);
    } catch (const RedisClient::CommandExecutor::Exception& e) {
        throw Exception("Connection error: " + QString(e.what()));
    }
}

void ListKeyModel::setListRow(int pos, const QByteArray &value)
{
    using namespace RedisClient;
    Command addCmd(m_dbIndex);
    (addCmd << "LSET" << m_keyFullPath << QString::number(pos)).append(value);

    try {
        CommandExecutor::execute(m_connection, addCmd);
    } catch (const RedisClient::CommandExecutor::Exception& e) {
        throw Exception("Connection error: " + QString(e.what()));
    }
}

void ListKeyModel::deleteListRow(int count, const QByteArray &value)
{
    using namespace RedisClient;
    Command deleteCmd(m_dbIndex);
    (deleteCmd << "LREM" << m_keyFullPath << QString::number(count)).append(value);

    try {
        CommandExecutor::execute(m_connection, deleteCmd);
    } catch (const RedisClient::CommandExecutor::Exception& e) {
        throw Exception("Connection error: " + QString(e.what()));
    }
}


