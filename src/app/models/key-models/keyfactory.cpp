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
        RedisClient::Response ttlResult = RedisClient::CommandExecutor::execute(connection, ttlCmd);

        int ttl = -1;

        if (ttlResult.getType() == RedisClient::Response::Integer) {
            ttl = ttlResult.getValue().toInt();
        }

        if (type == "string") {
            result = QSharedPointer<ValueEditor::Model>(new StringKeyModel(connection, keyFullPath, dbIndex, ttl));
        } else if (type == "list") {
            result = QSharedPointer<ValueEditor::Model>(new ListKeyModel(connection, keyFullPath, dbIndex, ttl));
        } else if (type == "set") {
            result = QSharedPointer<ValueEditor::Model>(new SetKeyModel(connection, keyFullPath, dbIndex, ttl));
        } else if (type == "zset") {
            result = QSharedPointer<ValueEditor::Model>(new SortedSetKeyModel(connection, keyFullPath, dbIndex, ttl));
        } else if (type == "hash") {
            result = QSharedPointer<ValueEditor::Model>(new HashKeyModel(connection, keyFullPath, dbIndex, ttl));
        }

        callback(result);

    }, dbIndex);

    connection->runCommand(typeCmd);
}

