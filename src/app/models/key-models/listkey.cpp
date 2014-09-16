#include "listkey.h"
#include "modules/redisclient/command.h"
#include "modules/redisclient/commandexecutor.h"

ListKeyModel::ListKeyModel(QSharedPointer<RedisClient::Connection> connection, QString fullPath, int dbIndex, int ttl)
    : KeyModel(connection, fullPath, dbIndex, ttl), m_rowCount(-1)
{    
    loadRowCount();
}

QString ListKeyModel::getType()
{
    return "list";
}

QStringList ListKeyModel::getColumnNames()
{
    return QStringList() << "Value";
}

QHash<int, QByteArray> ListKeyModel::getRoles()
{
    QHash<int, QByteArray> roles;
    roles[Roles::Value] = "value";
    return roles;
}

QString ListKeyModel::getData(int rowIndex, int dataRole)
{
    if (!isRowLoaded(rowIndex) || dataRole != Roles::Value)
        return QString();

    return m_rowsCache[rowIndex];
}

void ListKeyModel::setData(int rowIndex, int dataRole, QString value)
{

}

void ListKeyModel::addRow()
{

}

unsigned long ListKeyModel::rowsCount()
{
    return m_rowCount;
}

void ListKeyModel::loadRows(unsigned long rowStart, unsigned long count, std::function<void ()> callback)
{
    if (isPartialLoadingSupported()) {
        //TBD
    } else {
        unsigned long rowEnd = std::min(m_rowCount, rowStart + count) - 1;

        RedisClient::Command updateCmd(QStringList() << "LRANGE" << QString::number(rowStart) << QString::number(rowEnd), m_dbIndex);
        RedisClient::Response result = RedisClient::CommandExecutor::execute(m_connection, updateCmd);

        if (result.getType() != RedisClient::Response::MultiBulk) {
            throw ListKeyModel::Exception("loadRows() error - can't load list from server");
        }

        QStringList rows = result.getValue().toStringList();
        unsigned int rowIndex = rowStart;

        foreach (QString row, rows) {
            m_rowsCache[rowIndex] = row.toUtf8();
            rowIndex++;
        }
    }

    callback();
}

void ListKeyModel::clearRowCache()
{

}

void ListKeyModel::removeRow(int)
{

}

bool ListKeyModel::isRowLoaded(int rowIndex)
{
    return m_rowsCache.contains(rowIndex);
}

bool ListKeyModel::isMultiRow() const
{
    return true;
}

void ListKeyModel::loadRowCount()
{
    RedisClient::Command updateCmd(QStringList() << "LLEN" << m_keyFullPath, m_dbIndex);
    RedisClient::Response result = RedisClient::CommandExecutor::execute(m_connection, updateCmd);

    if (result.getType() == RedisClient::Response::Integer) {
        m_rowCount = result.getValue().toUInt();
    }
}

//void ListKeyModel::loadValue()
//{
//    QStringList command;
//    command << "LRANGE" << keyName <<"0" << "-1";

//    db->runCommand(RedisClient::Command(command, this, "loadedValue", dbIndex));
//}

//void ListKeyModel::setCurrentPage(int page)
//{
//    if (page == currentPage) {
//        return;
//    }

//    clear();

//    QStringList labels("Value");
//    setHorizontalHeaderLabels(labels);

//    currentPage = page;

//    int size = rawData->size();

//    setRowCount( (itemsOnPageLimit > size)? size : itemsOnPageLimit);

//    int startShiftPosition = itemsOnPageLimit * (currentPage - 1);
//    int limit = startShiftPosition  + itemsOnPageLimit;

//    for (int i = startShiftPosition, row = 0; i < limit && i < size; ++i, ++row) {

//        QStandardItem * value = new QStandardItem(rawData->at(i));
//        value->setData(QVariant(i), KeyModel::KEY_VALUE_TYPE_ROLE);

//        setItem(row, 0, value);
//    }
//}

//void ListKeyModel::updateValue(const QString& value, const QModelIndex *cellIndex)
//{
//    QStandardItem * item = itemFromIndex(*cellIndex);
//    item->setText(value);
//    int itemIndex = item->data(KeyModel::KEY_VALUE_TYPE_ROLE).toInt();

//    QStringList addNew;
//    addNew << "LSET" << keyName << QString::number(itemIndex) << value;

//    db->runCommand(RedisClient::Command(addNew, this, "loadedUpdateStatus", dbIndex));
//}

//void ListKeyModel::loadedUpdateStatus(RedisClient::Response result)
//{
//    if (result.isErrorMessage())
//    {
//        emit valueUpdateError(result.getValue().toString());
//    }
//    else
//    {
//        emit valueUpdated();
//    }
//}


