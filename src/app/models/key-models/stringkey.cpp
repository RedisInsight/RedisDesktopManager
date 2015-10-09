#include "stringkey.h"
#include <qredisclient/connection.h>

StringKeyModel::StringKeyModel(QSharedPointer<RedisClient::Connection> connection, QString fullPath, int dbIndex, long long ttl)
    : KeyModel(connection, fullPath, dbIndex, ttl, false,
               QByteArray(), QByteArray(), QByteArray())
{
}

QString StringKeyModel::getType()
{
    return "string";
}

QStringList StringKeyModel::getColumnNames()
{
    return QStringList(); // Single value type - No columns
}

QHash<int, QByteArray> StringKeyModel::getRoles()
{
    QHash<int, QByteArray> roles;
    roles[Roles::Value] = "value";
    roles[Roles::BinaryValue] = "binary_value";
    return roles;
}

QVariant StringKeyModel::getData(int rowIndex, int dataRole)
{
    if (!isRowLoaded(rowIndex))
        return QVariant();

    if (dataRole == Roles::Value)
        return m_rowsCache[rowIndex];

    if (dataRole == Roles::BinaryValue)
        return valueToBinary(m_rowsCache[rowIndex]);

    return QVariant();
}

void StringKeyModel::updateRow(int rowIndex, const QVariantMap &row)
{
    if (rowIndex > 0 || !isRowValid(row)) {
        qDebug() << "Row is not valid";
        return;   
    }

    QByteArray value = row.value("value").toByteArray();

    if (value.isEmpty())
        return;

    RedisClient::Response result;
    try {
        result = m_connection->commandSync("SET", m_keyFullPath, value, m_dbIndex);
    } catch (const RedisClient::Connection::Exception& e) {
        throw Exception("Connection error: " + QString(e.what()));
    }

    if (result.isOkMessage()) {
        m_rowsCache.clear();
        m_rowsCache.addLoadedRange({0, 0}, (QList<QByteArray>() << value));
        m_notifier->dataLoaded();
    }
}

void StringKeyModel::addRow(const QVariantMap &row)
{
    updateRow(0, row);
}

void StringKeyModel::loadRows(unsigned long, unsigned long, std::function<void(const QString&)> callback)
{
    if (loadValue()) {
        callback(QString());
    }
}

void StringKeyModel::removeRow(int)
{
    m_rowCount--;
    setRemovedIfEmpty();
}

bool StringKeyModel::loadValue()
{
    RedisClient::Response result;
    try {
        result = m_connection->commandSync("GET", m_keyFullPath, m_dbIndex);
    } catch (const RedisClient::Connection::Exception& e) {
        throw Exception("Connection error: " + QString(e.what()));
    }

    if (result.getType() != RedisClient::Response::Bulk) {
        return false;
    }

    m_rowsCache.clear();
    m_rowsCache.push_back(result.getValue().toByteArray());

    m_notifier->dataLoaded();

    return true;
}
