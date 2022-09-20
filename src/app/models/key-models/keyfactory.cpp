#include "keyfactory.h"

#include <qredisclient/redisclient.h>
#include <qredisclient/utils/text.h>

#include <QFile>
#include <QObject>

#include "bfkey.h"
#include "hashkey.h"
#include "listkey.h"
#include "rejsonkey.h"
#include "setkey.h"
#include "sortedsetkey.h"
#include "stream.h"
#include "stringkey.h"
#include "unknownkey.h"

KeyFactory::KeyFactory() {}

void KeyFactory::loadKey(
    QSharedPointer<RedisClient::Connection> connection, QByteArray keyFullPath,
    int dbIndex,
    std::function<void(QSharedPointer<ValueEditor::Model>, const QString&)>
        callback) {
  auto processError = [callback, keyFullPath](const QString& err) {
    QString msg(QCoreApplication::translate(
        "RESP", "Cannot load key %1, connection error occurred: %2"));
    callback(QSharedPointer<ValueEditor::Model>(),
             msg.arg(printableString(keyFullPath)).arg(err));
  };

  auto loadModel = [this, connection, keyFullPath, dbIndex, callback,
                    processError](RedisClient::Response resp) {
    QSharedPointer<ValueEditor::Model> result;

    if (resp.isErrorMessage() ||
        resp.type() != RedisClient::Response::Type::Status) {
      QString msg(QCoreApplication::translate(
          "RESP", "Cannot load key %1, connection error occurred: %2"));
      callback(
          result,
          msg.arg(printableString(keyFullPath)).arg(resp.value().toString()));
      return;
    }

    QString type = resp.value().toString();

    if (type == "none") {
      QString msg(QCoreApplication::translate(
          "RESP",
          "Cannot load key %1 because it doesn't exist in database."
          " Please reload connection tree and try again."));
      callback(result, msg.arg(printableString(keyFullPath)));
      return;
    }

    auto parseTtl = [this, type, connection, keyFullPath, dbIndex, callback,
                     processError](const RedisClient::Response& ttlResult) {
      long long ttl = -1;

      if (ttlResult.type() == RedisClient::Response::Integer) {
        ttl = ttlResult.value().toLongLong();
      }

      auto result = createModel(type, connection, keyFullPath, dbIndex, ttl);

      callback(result, QString());
    };

    connection->cmd({"ttl", keyFullPath}, this, -1, parseTtl, processError);
  };

  try {
    connection->cmd({"type", keyFullPath}, this, dbIndex, loadModel,
                    processError);
  } catch (const RedisClient::Connection::Exception& e) {
    callback(QSharedPointer<ValueEditor::Model>(),
             QCoreApplication::translate("RESP",
                                         "Cannot retrieve type of the key: ") +
                 QString(e.what()));
  }
}

void KeyFactory::createNewKeyRequest(
    QSharedPointer<RedisClient::Connection> connection,
    QSharedPointer<ConnectionsTree::Operations::OpenNewKeyDialogCallback> callback,
    int dbIndex, QString keyPrefix) {
  if (connection.isNull() || dbIndex < 0) return;
  emit newKeyDialog(NewKeyRequest(connection, dbIndex, callback, keyPrefix));
}

void KeyFactory::submitNewKeyRequest(NewKeyRequest r) {
  QSharedPointer<ValueEditor::Model> result = createModel(
      r.keyType(), r.connection(), r.keyName().toUtf8(), r.dbIndex(), -1);

  if (!result) return;

  auto onRowAdded = [this, r, result](const QString& err) {
    if (err.size() > 0) {
      emit error(err);
      return;
    }

    r.callback();
    emit keyAdded();
  };

  r.connection()->cmd(
      {"PING"}, this, r.dbIndex(),
      [onRowAdded, result, r](const RedisClient::Response& resp) {
        auto testResp = resp.value().toByteArray();
        if (testResp != "PONG") {
          return onRowAdded(testResp);
        }

        auto val = r.value();

        if (!r.valueFilePath().isEmpty() && QFile::exists(r.valueFilePath())) {
          QFile valueFile(r.valueFilePath());

          if (!valueFile.open(QIODevice::ReadOnly)) {
            return onRowAdded(QCoreApplication::translate(
                "RESP", "Cannot open file with key value"));
          }

          val["value"] = valueFile.readAll();
        }

        result->addRow(val, onRowAdded);
      },
      onRowAdded);
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
  } else if (type == "ReJSON-RL" || type == "ReJSON") {
    return QSharedPointer<ValueEditor::Model>(
        new ReJSONKeyModel(connection, keyFullPath, dbIndex, ttl));
  } else if (type == "stream") {
    return QSharedPointer<ValueEditor::Model>(
        new StreamKeyModel(connection, keyFullPath, dbIndex, ttl));
  } else if (type.startsWith("MBbloom")) {
      QString ff = type.endsWith("CF")? "cf" : "bf";
      return QSharedPointer<ValueEditor::Model>(
          new BloomFilterKeyModel(connection, keyFullPath, dbIndex, ttl, ff));
  }

  return QSharedPointer<ValueEditor::Model>(
      new UnknownKeyModel(connection, keyFullPath, dbIndex, ttl, type));
}
