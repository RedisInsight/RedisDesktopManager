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
    roles[Roles::BinaryValue] = "binary_value";
    return roles;
}

QVariant StringKeyModel::getData(int rowIndex, int dataRole)
{
    if (rowIndex > 0 || m_value.isNull())
        return QVariant();

    if (dataRole == Roles::Value)
        return m_value;

    if (dataRole == Roles::BinaryValue)
        return valueToBinary(m_value);

    return QVariant();
}

void StringKeyModel::updateRow(int rowIndex, const QVariantMap &row)
{
    if (rowIndex > 0 || !isRowValid(row))
        return;   

    QByteArray value = row.value("value").toByteArray();

    if (value.isEmpty())
        return;

    using namespace RedisClient;

    Command updateCmd(m_dbIndex);
    (updateCmd << "SET" << m_keyFullPath).append(value);

    Response result;
    try {
        result = CommandExecutor::execute(m_connection, updateCmd);
    } catch (const RedisClient::CommandExecutor::Exception& e) {
        throw Exception("Connection error: " + QString(e.what()));
    }

    if (result.isOkMessage()) {
        m_value = value;
        emit dataLoaded();
    }
}

void StringKeyModel::addRow(const QVariantMap &row)
{
    updateRow(0, row);
}

unsigned long StringKeyModel::rowsCount()
{
    return m_value.size() == 0 ? 0 : 1;
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
    m_value.clear();
}

void StringKeyModel::removeRow(int)
{
    m_value.clear();
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
    RedisClient::Command valueCmd(m_dbIndex);
    valueCmd << "GET" << m_keyFullPath;

    RedisClient::Response result;
    try {
        result = RedisClient::CommandExecutor::execute(m_connection, valueCmd);
    } catch (const RedisClient::CommandExecutor::Exception& e) {
        throw Exception("Connection error: " + QString(e.what()));
    }

    if (result.getType() != RedisClient::Response::Bulk) {
        return false;
    }

    m_value = result.getValue().toByteArray();

    emit dataLoaded();

    return true;
}
