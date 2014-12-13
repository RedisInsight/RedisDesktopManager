#include "stringkey.h"
#include "modules/redisclient/command.h"
#include "modules/redisclient/commandexecutor.h"

StringKeyModel::StringKeyModel(QSharedPointer<RedisClient::Connection> connection, QString fullPath, int dbIndex, int ttl)
    : KeyModel(connection, fullPath, dbIndex, ttl)
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
    return roles;
}

QVariant StringKeyModel::getData(int rowIndex, int dataRole)
{
    if (rowIndex > 0 || dataRole != Roles::Value || m_value.isNull())
        return QVariant();

    return m_value;
}

void StringKeyModel::updateRow(int rowIndex, const QVariantMap &row)
{
    if (rowIndex > 0 || !isRowValid(row))
        return;

    QByteArray value = row.value("value").toByteArray();

    if (value.isEmpty())
        return;

    RedisClient::Command updateCmd(QStringList() << "SET" << m_keyFullPath << value, m_dbIndex);
    RedisClient::Response result = RedisClient::CommandExecutor::execute(m_connection, updateCmd);

    if (result.isOkMessage()) {
        m_value = value;
        emit dataLoaded();
    }
}

void StringKeyModel::addRow(const QVariantMap &)
{
    return;
}

unsigned long StringKeyModel::rowsCount()
{
    return 1;
}

void StringKeyModel::loadRows(unsigned long rowStart, unsigned long count, std::function<void ()> callback)
{
    Q_UNUSED(rowStart);
    Q_UNUSED(count);

    if (loadValue()) {
        callback();
    }
}

void StringKeyModel::clearRowCache()
{
    return;
}

void StringKeyModel::removeRow(int)
{
    return;
}

bool StringKeyModel::isRowLoaded(int r)
{
    return r == 0 && !m_value.isNull();
}

bool StringKeyModel::isMultiRow() const
{
    return false;
}

bool StringKeyModel::loadValue()
{
    RedisClient::Command valueCmd(QStringList() << "GET" << m_keyFullPath, m_dbIndex);
    RedisClient::Response result = RedisClient::CommandExecutor::execute(m_connection, valueCmd);

    if (result.getType() != RedisClient::Response::Bulk) {
        return false;
    }

    m_value = result.getValue().toByteArray();

    emit dataLoaded();

    return true;
}
