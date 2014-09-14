#include "keyfactory.h"
#include "stringkey.h"
#include "setkey.h"
#include "sortedsetkey.h"
#include "hashkey.h"
#include "listkey.h"

KeyFactory::KeyFactory(QSharedPointer<RedisClient::Connection> connection)
    : m_connection(connection)
{
}

void KeyFactory::loadKey(QString keyFullPath, int dbIndex, std::function<void (QSharedPointer<ValueEditor::Model>)> callback)
{
    RedisClient::Command typeCmd(QStringList() << "type" << keyFullPath, this, [this, keyFullPath, dbIndex, callback] (RedisClient::Response resp) {

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

        // TBD : load TTL
        int ttl = -1;

        if (type == "string") {
            result = QSharedPointer<ValueEditor::Model>(new StringKeyModel(m_connection, keyFullPath, dbIndex, ttl))
        } else if (type == "list") {
            result = QSharedPointer<ValueEditor::Model>(new ListKeyModel(m_connection, keyFullPath, dbIndex, ttl))
        } else if (type == "set") {
            result = QSharedPointer<ValueEditor::Model>(new SetKeyModel(m_connection, keyFullPath, dbIndex, ttl))
        } else if (type == "zset") {
            result = QSharedPointer<ValueEditor::Model>(new SortedSetKeyModel(m_connection, keyFullPath, dbIndex, ttl))
        } else if (type == "hash") {
            result = QSharedPointer<ValueEditor::Model>(new HashKeyModel(m_connection, keyFullPath, dbIndex, ttl))
        }

        callback(result);

    }, dbIndex);
}

