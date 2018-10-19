#include "keyfactory.h"
#include <qredisclient/redisclient.h>
#include <qredisclient/utils/text.h>

#include <QObject>
#include "hashkey.h"
#include "listkey.h"
#include "rejsonkey.h"
#include "setkey.h"
#include "sortedsetkey.h"
#include "stringkey.h"

KeyFactory::KeyFactory() {}

void KeyFactory::loadKey(
    QSharedPointer<RedisClient::Connection> connection, QByteArray keyFullPath,
    int dbIndex,
    std::function<void(QSharedPointer<ValueEditor::Model>, const QString&)>
        callback) {
  RedisClient::Command typeCmd(
      {"type", keyFullPath}, this,
      [this, connection, keyFullPath, dbIndex, callback](
          RedisClient::Response resp, QString) {
        QSharedPointer<ValueEditor::Model> result;

        if (resp.isErrorMessage() ||
            resp.getType() != RedisClient::Response::Type::Status) {
          QString msg(QCoreApplication::translate(
              "RDM", "Cannot load key %1, connection error occurred: %2"));
          callback(
              result,
              msg.arg(printableString(keyFullPath)).arg(resp.toRawString()));
          return;
        }

        QString type = resp.getValue().toString();

        if (type == "none") {
          QString msg(QCoreApplication::translate(
              "RDM",
              "Cannot load key %1 because it doesn't exist in database."
              " Please reload connection tree and try again."));
          callback(result, msg.arg(printableString(keyFullPath)));
          return;
        }

        RedisClient::Response ttlResult;

        try {
          ttlResult = connection->commandSync({"ttl", keyFullPath}, dbIndex);
        } catch (const RedisClient::Connection::Exception& e) {
          QString msg(QCoreApplication::translate(
              "RDM",
              "Cannot load TTL for key %1, connection error occurred: %2"));
          callback(
              result,
              msg.arg(printableString(keyFullPath)).arg(QString(e.what())));
          return;
        }

        long long ttl = -1;

        if (ttlResult.getType() == RedisClient::Response::Integer) {
          ttl = ttlResult.getValue().toLongLong();
        }

        result = createModel(type, connection, keyFullPath, dbIndex, ttl);

        if (!result)
          return callback(result, QCoreApplication::translate(
                                      "RDM", "Unsupported Redis Data type %1")
                                      .arg(type));

        callback(result, QString());
      },
      dbIndex);

  try {
    connection->runCommand(typeCmd);
  } catch (const RedisClient::Connection::Exception& e) {
    throw Exception(
        QCoreApplication::translate("RDM", "Cannot retrive type of the key: ") +
        QString(e.what()));
  }
}

void KeyFactory::addKey(QSharedPointer<RedisClient::Connection> connection,
                        QByteArray keyFullPath, int dbIndex, QString type,
                        const QVariantMap& row) {
  QSharedPointer<ValueEditor::Model> result =
      createModel(type, connection, keyFullPath, dbIndex, -1);

  if (!result) return;

  result->addRow(row);
}

QSharedPointer<ValueEditor::Model> KeyFactory::createModel(
    QString type, QSharedPointer<RedisClient::Connection> connection,
    QByteArray keyFullPath, int dbIndex, long long ttl) {
  if (type == "string") {
    return QSharedPointer<ValueEditor::Model>(
        new StringKeyModel(connection, keyFullPath, dbIndex, ttl));
  } else if (type == "list") {
    return QSharedPointer<ValueEditor::Model>(
        new ListKeyModel(connection, keyFullPath, dbIndex, ttl));
  } else if (type == "set") {
    return QSharedPointer<ValueEditor::Model>(
        new SetKeyModel(connection, keyFullPath, dbIndex, ttl));
  } else if (type == "zset") {
    return QSharedPointer<ValueEditor::Model>(
        new SortedSetKeyModel(connection, keyFullPath, dbIndex, ttl));
  } else if (type == "hash") {
    return QSharedPointer<ValueEditor::Model>(
        new HashKeyModel(connection, keyFullPath, dbIndex, ttl));
  } else if (type == "ReJSON-RL") {
    return QSharedPointer<ValueEditor::Model>(
        new ReJSONKeyModel(connection, keyFullPath, dbIndex, ttl));
  }

  return QSharedPointer<ValueEditor::Model>();
}
