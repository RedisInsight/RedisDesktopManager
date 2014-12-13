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

void ListKeyModel::updateRow(int rowIndex, const QVariantMap &)
{

}

void ListKeyModel::addRow(const QVariantMap &)
{

}

void ListKeyModel::loadRows(unsigned long rowStart, unsigned long count, std::function<void ()> callback)
{
    if (isPartialLoadingSupported()) {
        //TBD
    } else {        
        QStringList rows = getRowsRange("LRANGE", rowStart, count).toStringList();

        unsigned int rowIndex = rowStart;

        foreach (QString row, rows) {
            m_rowsCache[rowIndex] = row.toUtf8();
            rowIndex++;
        }
    }

    callback();
}

void ListKeyModel::removeRow(int i)
{
    if (!m_rowsCache.contains(i))
        return;

    QByteArray value = m_rowsCache.value(i);

    RedisClient::Response result;

    // check position update
    RedisClient::Command getValueByIndex(QStringList()
                                         << "LRANGE" << m_keyFullPath
                                         << QString::number(i) << QString::number(i),
                                         m_dbIndex);

    result = RedisClient::CommandExecutor::execute(m_connection, getValueByIndex);

    QStringList currentState = result.getValue().toStringList();

    if (currentState.size() != 1 || currentState[0] != QString(value)) {
        throw Exception("Can't delete row from list, because row already has changed."
                        " Reload values and try again.");
    }

    // Replace value by system string
    QString customSystemValue("---VALUE_REMOVED_BY_RDM---");
    RedisClient::Command resetValue(QStringList()
                                    << "LSET" << m_keyFullPath
                                    << QString::number(i) << customSystemValue,
                                    m_dbIndex);
    result = RedisClient::CommandExecutor::execute(m_connection, resetValue);

    // Remove all system values from list
    RedisClient::Command deleteValues(QStringList()
                                    << "LREM" << m_keyFullPath << 0 << customSystemValue,
                                    m_dbIndex);

    result = RedisClient::CommandExecutor::execute(m_connection, deleteValues);

    m_rowCount--;
    m_rowsCache.remove(i);
    Q_UNUSED(result);

    if (m_rowCount == 0) {
        m_isKeyRemoved = true;
        emit removed();
    }
}

void ListKeyModel::loadRowCount()
{    
    m_rowCount = getRowCount("LLEN");
}

//void ListKeyModel::updateValue(const QString& value, const QModelIndex *cellIndex)
//{
//    QStandardItem * item = itemFromIndex(*cellIndex);
//    item->setText(value);
//    int itemIndex = item->data(KeyModel::KEY_VALUE_TYPE_ROLE).toInt();

//    QStringList addNew;
//    addNew << "LSET" << keyName << QString::number(itemIndex) << value;

//    db->runCommand(RedisClient::Command(addNew, this, "loadedUpdateStatus", dbIndex));
//}


