#include "keyfactory.h"
#include "stringkey.h"
#include "setkey.h"
#include "sortedsetkey.h"
#include "hashkey.h"
#include "listkey.h"
#include "modules/redisclient/redisclient.h"

KeyFactory::KeyFactory()
{}

void KeyFactory::loadKey(QSharedPointer<RedisClient::Connection> connection,
                         QString keyFullPath, int dbIndex,
                         std::function<void (QSharedPointer<ValueEditor::Model>)> callback)
{
    RedisClient::Command typeCmd(QStringList() << "type" << keyFullPath, this,
                                 [this, connection, keyFullPath, dbIndex, callback] (RedisClient::Response resp) {

        QSharedPointer<ValueEditor::Model> result;

        if (resp.isErrorMessage() || resp.getType() != RedisClient::Response::Type::Status) {
            callback(result);
            return;
        }

        QString type = resp.getValue().toString();        

        if (type == "none") {
            callback(result);
            return;
        }

        RedisClient::Command ttlCmd(QStringList() << "ttl" << keyFullPath, dbIndex);
        RedisClient::Response ttlResult;

        try {
            ttlResult = RedisClient::CommandExecutor::execute(connection, ttlCmd);
        } catch (const RedisClient::CommandExecutor::Exception& e) {
            callback(result);
            return;
        }

        int ttl = -1;

        if (ttlResult.getType() == RedisClient::Response::Integer) {
            ttl = ttlResult.getValue().toInt();
        }

        result = createModel(type, connection, keyFullPath, dbIndex, ttl);

        callback(result);

    }, dbIndex);

    try {
        connection->runCommand(typeCmd);
    } catch (const RedisClient::Connection::Exception& e) {
        throw Exception("Connection error: " + QString(e.what()));
    }
}

void KeyFactory::addKey(QSharedPointer<RedisClient::Connection> connection, QString keyFullPath, int dbIndex,
                        QString type, const QVariantMap &row)
{
    QSharedPointer<ValueEditor::Model> result = createModel(type, connection, keyFullPath, dbIndex, -1);

    if (!result)
        return;

    result->addRow(row);
}

QSharedPointer<ValueEditor::Model> KeyFactory::createModel(QString type, QSharedPointer<RedisClient::Connection> connection,
                                                           QString keyFullPath, int dbIndex, int ttl)
{
    if (type == "string") {
        return QSharedPointer<ValueEditor::Model>(new StringKeyModel(connection, keyFullPath, dbIndex, ttl));
    } else if (type == "list") {
        return QSharedPointer<ValueEditor::Model>(new ListKeyModel(connection, keyFullPath, dbIndex, ttl));
    } else if (type == "set") {
        return QSharedPointer<ValueEditor::Model>(new SetKeyModel(connection, keyFullPath, dbIndex, ttl));
    } else if (type == "zset") {
        return QSharedPointer<ValueEditor::Model>(new SortedSetKeyModel(connection, keyFullPath, dbIndex, ttl));
    } else if (type == "hash") {
        return QSharedPointer<ValueEditor::Model>(new HashKeyModel(connection, keyFullPath, dbIndex, ttl));
    }

    return QSharedPointer<ValueEditor::Model>();
}

