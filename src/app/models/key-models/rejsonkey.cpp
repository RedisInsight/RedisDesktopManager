#include "rejsonkey.h"
#include <qredisclient/connection.h>

ReJSONKeyModel::ReJSONKeyModel(QSharedPointer<RedisClient::Connection> connection,
                               QByteArray fullPath, int dbIndex, long long ttl)
    : KeyModel(connection, fullPath, dbIndex, ttl, false,
               QByteArray(), QByteArray(), QByteArray())
{
}

QString ReJSONKeyModel::getType()
{
    return "ReJSON";
}

QStringList ReJSONKeyModel::getColumnNames()
{
    return QStringList(); // Single value type - No columns
}

QHash<int, QByteArray> ReJSONKeyModel::getRoles()
{
    QHash<int, QByteArray> roles;
    roles[Roles::Value] = "value";
    return roles;
}

QVariant ReJSONKeyModel::getData(int rowIndex, int dataRole)
{
    if (!isRowLoaded(rowIndex))
        return QVariant();

    if (dataRole == Roles::Value)
        return m_rowsCache[rowIndex];

    return QVariant();
}

void ReJSONKeyModel::updateRow(int rowIndex, const QVariantMap &row)
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
        result = m_connection->commandSync({"JSON.SET", m_keyFullPath, ".", value}, m_dbIndex);
    } catch (const RedisClient::Connection::Exception& e) {
        throw Exception(QObject::tr("Connection error: ") + QString(e.what()));
    }

    if (result.isOkMessage()) {
        m_rowsCache.clear();
        m_rowsCache.addLoadedRange({0, 0}, (QList<QByteArray>() << value));
        m_notifier->dataLoaded();
    }
}

void ReJSONKeyModel::addRow(const QVariantMap &row)
{
    updateRow(0, row);
}

void ReJSONKeyModel::loadRows(unsigned long, unsigned long, std::function<void(const QString&)> callback)
{
    try {
        m_connection->command({"JSON.GET", m_keyFullPath},
                              getConnector().data(),
                              [this, callback](RedisClient::Response r, QString e)
        {
            if (r.getType() != RedisClient::Response::Bulk || !e.isEmpty()) {
                return callback(QString("Cannot load value"));
            }

            m_rowsCache.clear();
            m_rowsCache.push_back(r.getValue().toByteArray());
            m_notifier->dataLoaded();

            callback(QString());
        }, m_dbIndex);
    } catch (const RedisClient::Connection::Exception& e) {
        throw Exception(QObject::tr("Connection error: ") + QString(e.what()));
    }
}

void ReJSONKeyModel::removeRow(int)
{
    m_rowCount--;
    setRemovedIfEmpty();
}
