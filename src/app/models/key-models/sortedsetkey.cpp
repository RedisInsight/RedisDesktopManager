#include "sortedsetkey.h"

SortedSetKeyModel::SortedSetKeyModel(QSharedPointer<RedisClient::Connection> connection, QString fullPath, int dbIndex, int ttl)
    : KeyModel(connection, fullPath, dbIndex, ttl)
{
    loadRowCount();
}

QString SortedSetKeyModel::getType()
{
    return "zset";
}

QStringList SortedSetKeyModel::getColumnNames()
{
    return QStringList() << "row" << "value" << "score";
}

QHash<int, QByteArray> SortedSetKeyModel::getRoles()
{
    QHash<int, QByteArray> roles;
    roles[Roles::RowNumber] = "row";
    roles[Roles::Value] = "value";
    roles[Roles::Score] = "score";
    return roles;
}

QVariant SortedSetKeyModel::getData(int rowIndex, int dataRole)
{
    if (!isRowLoaded(rowIndex))
        return QVariant();

    QPair<QByteArray, double> row = m_rowsCache[rowIndex];

    if (dataRole == Roles::Value)
        return row.first;
    else if (dataRole ==Roles::Score)
        return QString::number(row.second);
    else if (dataRole == Roles::RowNumber)
        return QString::number(rowIndex+1);

    return QVariant();
}

void SortedSetKeyModel::updateRow(int rowIndex, const QVariantMap &)
{

}

void SortedSetKeyModel::addRow(const QVariantMap &)
{

}

unsigned long SortedSetKeyModel::rowsCount()
{
    return m_rowCount;
}

void SortedSetKeyModel::loadRows(unsigned long rowStart, unsigned long count, std::function<void ()> callback)
{
    if (isPartialLoadingSupported()) {
        //TBD
    } else {
        QStringList rows = getRowsRange("ZRANGE WITHSCORES", rowStart, count).toStringList();

        unsigned int rowIndex = rowStart;

        for (QStringList::iterator item = rows.begin();
             item != rows.end(); ++item, rowIndex++) {

            QPair<QByteArray, double> value;
            value.first = item->toUtf8();
            ++item;

            if (item == rows.end())
                throw Exception("Partial data loaded from server");

            value.second = item->toDouble();
            m_rowsCache[rowIndex] = value;
        }
    }

    callback();
}

void SortedSetKeyModel::clearRowCache()
{
    m_rowsCache.clear();
}

void SortedSetKeyModel::removeRow(int)
{

}

bool SortedSetKeyModel::isRowLoaded(int rowIndex)
{
    return m_rowsCache.contains(rowIndex);
}

bool SortedSetKeyModel::isMultiRow() const
{
    return true;
}

void SortedSetKeyModel::loadRowCount()
{
    m_rowCount = getRowCount("ZCARD");
}

//void SortedSetKeyModel::updateValue(const QString& value, const QModelIndex *cellIndex)
//{
//    QStandardItem * currentItem = itemFromIndex(*cellIndex);

//    QString itemType = currentItem->data(KeyModel::KEY_VALUE_TYPE_ROLE).toString();

//    if (itemType == "member")
//    {
//        QStringList removeCmd;
//        removeCmd << "ZREM"
//            << keyName
//            << currentItem->text();

//        db->runCommand(RedisClient::Command(removeCmd, this, dbIndex));

//        QStandardItem * scoreItem = item(currentItem->row(), 1);

//        QStringList addCmd;

//        addCmd << "ZADD"
//            << keyName
//            << scoreItem->text()
//            << value;

//        db->runCommand(RedisClient::Command(addCmd, this, "loadedUpdateStatus", dbIndex));

//    } else if (itemType == "score") {

//        bool converted = false;
//        double changedScore = value.toDouble(&converted);

//        if (!converted)
//            return;
        
//        double currentScore = currentItem->text().toDouble();
//        double incr = changedScore - currentScore;

//        QStandardItem * memberItem = item(currentItem->row(), 0);

//        QStringList updateCmd;
//        updateCmd << "ZINCRBY"
//            << keyName
//            << QString::number(incr)
//            << memberItem->text();

//        db->runCommand(RedisClient::Command(updateCmd, this, "loadedUpdateStatus", dbIndex));
//    }

//    currentItem->setText(value);
//}
