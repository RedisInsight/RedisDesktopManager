#pragma once
#include <qredisclient/redisclient.h>
#include <qredisclient/utils/text.h>
#include <QByteArray>
#include <QCoreApplication>
#include <QDebug>

#include <QPair>
#include <QSharedPointer>
#include <QString>
#include <QVariant>
#include "modules/value-editor/keymodel.h"
#include "rowcache.h"

template <typename T>
class KeyModel : public ValueEditor::Model {
 public:
  KeyModel(QSharedPointer<RedisClient::Connection> connection,
           QByteArray fullPath, int dbIndex, int ttl,
           QByteArray rowsCountCmd = QByteArray(),
           QByteArray rowsLoadCmd = QByteArray())
      : m_connection(connection),
        m_keyFullPath(fullPath),
        m_dbIndex(dbIndex),
        m_ttl(ttl),
        m_rowCount(0),
        m_isMultiRow(!rowsCountCmd.isEmpty()),
        m_rowsCountCmd(rowsCountCmd),
        m_rowsLoadCmd(rowsLoadCmd),
        m_notifier(new ValueEditor::ModelSignals(), &QObject::deleteLater) {}

  virtual ~KeyModel() { m_notifier.clear(); }

  virtual QString getKeyName() override {
    return printableString(m_keyFullPath);
  }

  virtual QString getKeyTitle() override {
    return QString("%1::db%2::%3")
        .arg(m_connection->getConfig().name())
        .arg(m_dbIndex)
        .arg(getKeyName());
  }

  virtual long long getTTL() override { return m_ttl; }

  virtual bool isMultiRow() const override { return m_isMultiRow; }

  virtual bool isRowLoaded(int rowIndex) override {
    return m_rowsCache.isRowLoaded(rowIndex);
  }

  virtual unsigned long rowsCount() override { return m_rowCount; }

  virtual void setKeyName(const QByteArray& newKeyName,
                          ValueEditor::Model::Callback c) override {
    RedisClient::Response result;

    try {
      result = m_connection->commandSync(
          {"RENAMENX", m_keyFullPath, newKeyName}, m_dbIndex);
    } catch (const RedisClient::Connection::Exception& e) {
      return c(QCoreApplication::translate("RDM", "Cannot rename key %1: %2")
                   .arg(getKeyName())
                   .arg(e.what()));
    }

    if (result.value().toInt() == 0) {
      emit m_notifier->error(
          QCoreApplication::translate("RDM",
                                      "Key with new name %1 already exist in "
                                      "database or original key was "
                                      "removed")
              .arg(getKeyName()));
      return;
    }

    m_keyFullPath = newKeyName;
  }

  virtual void setTTL(const long long ttl,
                      ValueEditor::Model::Callback c) override {
    RedisClient::Response result;
    qDebug() << QString("TTL=%1").arg(ttl);
    try {
      if (ttl >= 0)
        result = m_connection->commandSync(
            {"EXPIRE", m_keyFullPath, QString::number(ttl).toLatin1()},
            m_dbIndex);
      else
        result =
            m_connection->commandSync({"PERSIST", m_keyFullPath}, m_dbIndex);
    } catch (const RedisClient::Connection::Exception& e) {
      emit m_notifier->error(
          QCoreApplication::translate("RDM", "Cannot set TTL for key %1: %2")
              .arg(getKeyName())
              .arg(e.what()));
      return;
    }

    if (result.value().toInt() == 0) {
      emit m_notifier->error(
          QCoreApplication::translate("RDM", "Cannot set TTL for key %1")
              .arg(getKeyName()));
      return;
    }
    if (ttl >= 0)
      m_ttl = ttl;
    else
      m_ttl = -1;
  }

  virtual void removeKey(ValueEditor::Model::Callback c) override {
    RedisClient::Response result;

    try {
      result = m_connection->commandSync({"DEL", m_keyFullPath}, m_dbIndex);
    } catch (const RedisClient::Connection::Exception& e) {
      emit m_notifier->error(
          QCoreApplication::translate("RDM", "Cannot remove key %1: %2")
              .arg(getKeyName())
              .arg(e.what()));
      return;
    }

    m_notifier->removed();
  }

  virtual void loadRows(QVariant rowStart, unsigned long count,
                        LoadRowsCallback callback) override {
    if (m_rowsLoadCmd.mid(1, 4).toLower() == "scan") {
      QList<QByteArray> cmdParts = m_rowsLoadCmd.split(' ');
      cmdParts.replace(cmdParts.indexOf("%1"), m_keyFullPath);

      RedisClient::ScanCommand cmd(cmdParts, m_dbIndex);
      auto self = ValueEditor::Model::sharedFromThis().toWeakRef();

      try {
        m_connection->retrieveCollection(
            cmd, [this, callback, rowStart, self](QVariant result, QString) {
              if (!self) {
                return;
              }

              if (result.type() == QVariant::Type::List) {
                try {
                  auto loadedRows = result.toList();
                  addLoadedRowsToCache(loadedRows, rowStart);
                  callback(QString(),
                           loadedRows.size() / (getColumnNames().size() - 1));
                } catch (const std::runtime_error& e) {
                  callback(QString(e.what()), 0);
                }
              }
            });
      } catch (const RedisClient::Connection::Exception& e) {
        callback(QString(e.what()), 0);
      }
    } else {
      getRowsRange(
          getRangeCmd(rowStart, count),
          [this, callback, rowStart](const QString& err, QVariantList result) {
            if (!err.isEmpty()) return callback(err, 0);

            addLoadedRowsToCache(result, rowStart);
            callback(QString(), result.size());
          });
    }
  }

  virtual void clearRowCache() override { m_rowsCache.clear(); }

  virtual QSharedPointer<ValueEditor::ModelSignals> getConnector()
      const override {
    return m_notifier;
  }

  virtual QSharedPointer<RedisClient::Connection> getConnection()
      const override {
    return m_connection;
  }

  virtual unsigned int dbIndex() const override { return m_dbIndex; }

  virtual void loadRowsCount(ValueEditor::Model::Callback c) override {
    if (!isMultiRow()) {
      m_rowCount = 1;
      return c(QString());
    }

    executeCmd({m_rowsCountCmd, m_keyFullPath}, c,
               [this](RedisClient::Response r, Callback c) {
                 m_rowCount = r.value().toUInt();
                 c(QString());
               },
               RedisClient::Response::Type::Integer);
  }

 protected:
  // multi row internal operations
  virtual QList<QByteArray> getRangeCmd(QVariant rowStartId,
                                        unsigned long count) {
    QList<QByteArray> cmd;

    unsigned long rowStart = rowStartId.toULongLong();
    unsigned long rowEnd = std::min(m_rowCount, rowStart + count) - 1;

    if (m_rowsLoadCmd.contains(' ')) {
      QList<QByteArray> suffixCmd(m_rowsLoadCmd.split(' '));

      cmd << suffixCmd.takeFirst();
      cmd << m_keyFullPath << QString::number(rowStart).toLatin1()
          << QString::number(rowEnd).toLatin1();
      cmd += suffixCmd;

    } else {
      cmd << m_rowsLoadCmd << m_keyFullPath
          << QString::number(rowStart).toLatin1()
          << QString::number(rowEnd).toLatin1();
    }
    return cmd;
  }

  virtual void getRowsRange(
      const QList<QByteArray>& rangeCmd,
      std::function<void(const QString&, QVariantList)> callback) {
    try {
      m_connection->command(
          rangeCmd, getConnector().data(),
          [this, callback](RedisClient::Response r, QString e) {
            if (!e.isEmpty()) {
              return callback(e, QVariantList());
            }

            if (r.type() != RedisClient::Response::Array) {
              return callback(QCoreApplication::translate(
                                  "RDM", "Cannot load rows for key %1: %2")
                                  .arg(getKeyName()),
                              QVariantList());
            }

            return callback(QString(), r.value().toList());
          },
          m_dbIndex);
    } catch (const RedisClient::Connection::Exception& e) {
      callback(
          QCoreApplication::translate("RDM", "Cannot load rows for key %1: %2")
              .arg(getKeyName())
              .arg(e.what()),
          QVariantList());
    }
  }

  // row validator
  virtual bool isRowValid(const QVariantMap& row) {
    if (row.isEmpty()) return false;

    QSet<QString> validKeys;

    foreach (QByteArray role, getRoles().values()) { validKeys.insert(role); }

    QMapIterator<QString, QVariant> i(row);

    while (i.hasNext()) {
      i.next();

      if (!validKeys.contains(i.key())) return false;
    }

    return true;
  }

  virtual void setRemovedIfEmpty() {
    if (m_rowCount == 0) {
      m_notifier->removed();
    }
  }

  typedef std::function<void(RedisClient::Response r, Callback c)> CmdHandler;

  virtual void executeCmd(QList<QByteArray> cmd, Callback c,
                          CmdHandler handler = CmdHandler(),
                          RedisClient::Response::Type expectedType =
                              RedisClient::Response::Type::Unknown) {
    m_connection->cmd(
        cmd, m_notifier.data(), m_dbIndex,
        [c, handler, expectedType](RedisClient::Response r) {
          if (expectedType != RedisClient::Response::Type::Unknown &&
              r.type() != expectedType) {
            return c(QCoreApplication::translate(
                         "RDM", "Server returned unexpected response: ") +
                     r.value().toString());
          }

          if (handler) {
            return handler(r, c);
          } else {
            return c(QString());
          }
        },
        [c](QString err) {
          return c(QCoreApplication::translate("RDM", "Connection error: ") +
                   err);
        });
  }

  virtual void addLoadedRowsToCache(const QVariantList& rows,
                                    QVariant rowStart) = 0;

 protected:
  QSharedPointer<RedisClient::Connection> m_connection;
  QByteArray m_keyFullPath;
  int m_dbIndex;
  long long m_ttl;
  unsigned long m_rowCount;
  bool m_isMultiRow;

  // CMD strings
  QByteArray m_rowsCountCmd;
  QByteArray m_rowsLoadCmd;

  MappedCache<T> m_rowsCache;
  QSharedPointer<ValueEditor::ModelSignals> m_notifier;
};
