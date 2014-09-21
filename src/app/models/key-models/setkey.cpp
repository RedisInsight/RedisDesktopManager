#include "setkey.h"
#include "modules/redisclient/command.h"
#include "modules/redisclient/commandexecutor.h"

SetKeyModel::SetKeyModel(QSharedPointer<RedisClient::Connection> connection, QString fullPath, int dbIndex, int ttl)
       : KeyModel(connection, fullPath, dbIndex, ttl)
{
    loadRowCount();
}

QString SetKeyModel::getType()
{
    return "set";
}

QStringList SetKeyModel::getColumnNames()
{
    return QStringList() << "Value";
}

QHash<int, QByteArray> SetKeyModel::getRoles()
{
    QHash<int, QByteArray> roles;
    roles[Roles::Value] = "value";
    return roles;
}

QString SetKeyModel::getData(int rowIndex, int dataRole)
{
    if (!isRowLoaded(rowIndex) || dataRole != Roles::Value)
        return QString();

    return m_rowsCache[rowIndex];
}

void SetKeyModel::setData(int rowIndex, int dataRole, QString value)
{

}

void SetKeyModel::addRow()
{

}

unsigned long SetKeyModel::rowsCount()
{
    return m_rowCount;
}

void SetKeyModel::loadRows(unsigned long rowStart, unsigned long count, std::function<void ()> callback)
{
    if (isPartialLoadingSupported()) {
        //TBD
    } else {

        if (!m_rowsCache.isEmpty())
            return;

        QStringList rows = getRowsRange("SMEMBERS").toStringList();
        unsigned int rowIndex = rowStart;

        foreach (QString row, rows) {
            m_rowsCache[rowIndex] = row.toUtf8();
            rowIndex++;
        }
    }

    callback();
}

void SetKeyModel::clearRowCache()
{
    m_rowsCache.clear();
}

void SetKeyModel::removeRow(int)
{

}

bool SetKeyModel::isRowLoaded(int rowIndex)
{
    return m_rowsCache.contains(rowIndex);
}

bool SetKeyModel::isMultiRow() const
{
    return true;
}

void SetKeyModel::loadRowCount()
{
    m_rowCount = getRowCount("SCARD");
}

//void SetKeyModel::updateValue(const QString& value, const QModelIndex *cellIndex)
//{
//    QStandardItem * item = itemFromIndex(*cellIndex);
//    QString oldValue = item->text();
//    item->setText(value);

//    QString deleteOld = QString("SREM %1 %2")
//        .arg(keyName)
//        .arg(oldValue);

//    db->runCommand(RedisClient::Command(deleteOld, this, dbIndex));

//    QString addNew = QString("SADD %1 %2")
//        .arg(keyName)
//        .arg(value);

//    db->runCommand(RedisClient::Command(addNew, this, "loadedUpdateStatus", dbIndex));
//}
