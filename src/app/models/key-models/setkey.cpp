#include "setkey.h"

SetKeyModel::SetKeyModel(QSharedPointer<RedisClient::Connection> connection, QString fullPath, int dbIndex, int ttl)
       : KeyModel(connection, fullPath, dbIndex, ttl)
{
}

QString SetKeyModel::getType()
{
    return "set";
}

QStringList SetKeyModel::getColumnNames()
{
    return QStringList();
}

QHash<int, QByteArray> SetKeyModel::getRoles()
{
    return QHash<int, QByteArray>();
}

QString SetKeyModel::getData(int rowIndex, int dataRole)
{
    return QString();
}

void SetKeyModel::setData(int rowIndex, int dataRole, QString value)
{

}

void SetKeyModel::addRow()
{

}

unsigned long SetKeyModel::rowsCount()
{
    return 0;
}

void SetKeyModel::loadRows(unsigned long rowStart, unsigned long count, std::function<void ()> callback)
{

}

void SetKeyModel::clearRowCache()
{

}

void SetKeyModel::removeRow(int)
{

}

bool SetKeyModel::isRowLoaded(int)
{
    return false;
}

bool SetKeyModel::isMultiRow() const
{
    return true;
}

//void SetKeyModel::loadValue()
//{
//    QStringList command;
//    command << "SMEMBERS" << keyName;

//    db->runCommand(RedisClient::Command(command, this, "loadedValue", dbIndex));
//}

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

//void SetKeyModel::loadedUpdateStatus(RedisClient::Response result)
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
