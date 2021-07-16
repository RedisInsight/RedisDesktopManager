#include "keyfactory.h"

#include <qredisclient/redisclient.h>
#include <qredisclient/utils/text.h>

#include <QObject>
#include <QFile>

#include "hashkey.h"
#include "listkey.h"
#include "rejsonkey.h"
#include "setkey.h"
#include "sortedsetkey.h"
#include "stream.h"
#include "stringkey.h"

KeyFactory::KeyFactory() {}

void KeyFactory::loadKey(
    QSharedPointer<RedisClient::Connection> connection, QByteArray keyFullPath,
    int dbIndex,
    std::function<void(QSharedPointer<ValueEditor::Model>, const QString&)>
        callback) {
  auto loadModel = [this, connection, keyFullPath, dbIndex, callback](
                       RedisClient::Response resp, QString) {
    QSharedPointer<ValueEditor::Model> result;

    if (resp.isErrorMessage() ||
        resp.type() != RedisClient::Response::Type::Status) {
      QString msg(QCoreApplication::translate(
          "RDM", "Cannot load key %1, connection error occurred: %2"));
      callback(
          result,
          msg.arg(printableString(keyFullPath)).arg(resp.value().toString()));
      return;
    }

    QString type = resp.value().toString();

    if (type == "none") {
      QString msg(QCoreApplication::translate(
          "RDM",
          "Cannot load key %1 because it doesn't exist in database."
          " Please reload connection tree and try again."));
      callback(result, msg.arg(printableString(keyFullPath)));
      return;
    }

    auto parseTtl = [this, type, connection, keyFullPath, dbIndex,
                     callback](const RedisClient::Response& ttlResult) {
      long long ttl = -1;

      if (ttlResult.type() == RedisClient::Response::Integer) {
        ttl = ttlResult.value().toLongLong();
      }

      auto result = createModel(type, connection, keyFullPath, dbIndex, ttl);

      if (!result)
        return callback(result, QCoreApplication::translate(
                                    "RDM", "Unsupported Redis Data type %1")
                                    .arg(type));

      callback(result, QString());
    };

    auto processTtlError = [callback, result, keyFullPath](const QString& err) {
      QString msg(QCoreApplication::translate(
          "RDM", "Cannot load TTL for key %1, connection error occurred: %2"));
      callback(result, msg.arg(printableString(keyFullPath)).arg(err));
    };

    connection->cmd({"ttl", keyFullPath}, this, -1, parseTtl, processTtlError);
  };

  RedisClient::Command typeCmd({"type", keyFullPath}, this, loadModel, dbIndex);

  try {
    RedisClient::Response typeResult = connection->runCommand(typeCmd);
    if (typeResult.isPermissionError()) {
      emit error(typeResult.value().toString());
    }
  } catch (const RedisClient::Connection::Exception& e) {
    callback(
        QSharedPointer<ValueEditor::Model>(),
        QCoreApplication::translate("RDM", "Cannot retrieve type of the key: ") +
            QString(e.what()));
  }
}

void KeyFactory::createNewKeyRequest(
    QSharedPointer<RedisClient::Connection> connection,
    std::function<void()> callback, int dbIndex, QString keyPrefix) {
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
                                      "RDM", "Cannot open file with key value"));
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
  } else if (type == "ReJSON-RL") {
    return QSharedPointer<ValueEditor::Model>(
        new ReJSONKeyModel(connection, keyFullPath, dbIndex, ttl));
  } else if (type == "stream") {
    return QSharedPointer<ValueEditor::Model>(
        new StreamKeyModel(connection, keyFullPath, dbIndex, ttl));
  }

  return QSharedPointer<ValueEditor::Model>();
}
