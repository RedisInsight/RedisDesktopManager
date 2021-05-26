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
           QByteArray fullPath, int dbIndex, long long ttl,
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
        m_scanCursor(0),
        m_notifier(new ValueEditor::ModelSignals(), &QObject::deleteLater) {}

  virtual ~KeyModel() {
    m_notifier.clear();
    m_connection.clear();
  }

  virtual QString getKeyName() override {
    return printableString(m_keyFullPath);
  }

  virtual QString getKeyTitle(int limit=-1) override {
    QString fullTitle = QString("%1::db%2::%3")
        .arg(m_connection->getConfig().name())
        .arg(m_dbIndex)
        .arg(getKeyName());

    int length = fullTitle.size();

    if (limit == -1 || length <= limit){
        return fullTitle;
    } else {
        return QString("%1 ... %2").arg(fullTitle.mid(0, limit/2)).arg(fullTitle.mid(length - limit/2));
    }
  }

  virtual long long getTTL() override { return m_ttl; }

  virtual bool isMultiRow() const override { return m_isMultiRow; }

  virtual bool isRowLoaded(int rowIndex) override {
    return m_rowsCache.isRowLoaded(rowIndex);
  }

  virtual unsigned long rowsCount() override {
    if (isMultiRow())
      return m_rowCount;
    else
      return 1;
  }

  virtual void setKeyName(const QByteArray& newKeyName,
                          ValueEditor::Model::Callback c) override {
    // NOTE(u_glide): DUMP + RESTORE + DEL is cluster compatible alternative to RENAME command
    executeCmd(
        {"DUMP", m_keyFullPath}, c,
        [this, newKeyName](RedisClient::Response r, Callback c) {
          executeCmd(
              {"RESTORE", newKeyName,
               QString::number(m_ttl > 0 ? m_ttl : 0).toLatin1(),
               r.value().toByteArray()},
              c,
              [this, newKeyName](RedisClient::Response r, Callback c) {
                if (!r.isOkMessage()) {
                  return c(QCoreApplication::translate(
                               "RDM", "Cannot rename key %1: %2")
                               .arg(getKeyName())
                               .arg(r.value().toString()));
                }

                executeCmd(
                    {"DEL", m_keyFullPath}, [](const QString&) {},
                    [](RedisClient::Response, Callback) {});

                m_keyFullPath = newKeyName;
                c(QString());
              },
              RedisClient::Response::Type::Status);
        },
        RedisClient::Response::Type::String);
  }

  virtual void setTTL(const long long ttl,
                      ValueEditor::Model::Callback c) override {
    executeCmd(
        {"EXPIRE", m_keyFullPath, QString::number(ttl).toLatin1()}, c,
        [this, ttl](RedisClient::Response r, Callback c) {
          if (r.value().toInt() == 0) {
            return c(
                QCoreApplication::translate("RDM", "Cannot set TTL for key %1")
                    .arg(getKeyName()));
          }

          if (ttl >= 0)
            m_ttl = ttl;
          else
            m_ttl = -1;

          c(QString());
        },
        RedisClient::Response::Type::Integer);
  }

  virtual void persistKey(Callback c) override {
    executeCmd(
        {"PERSIST", m_keyFullPath}, c,
        [this](RedisClient::Response r, Callback c) {
          if (r.value().toInt() == 0) {
            return c(QCoreApplication::translate(
                         "RDM",
                         "Cannot persist key '%1'. <br> Key does not exist or "
                         "does not have an assigned TTL value")
                         .arg(getKeyName()));
          }

          m_ttl = -1;

          c(QString());
        },
        RedisClient::Response::Type::Integer);
  }

  virtual void removeKey(ValueEditor::Model::Callback c) override {
    executeCmd({"DEL", m_keyFullPath}, c,
               [this](RedisClient::Response, Callback c) {
                 m_notifier->removed();
                 c(QString());
               });
  }

  virtual void loadRows(QVariant rowStart, unsigned long count,
                        LoadRowsCallback callback) override {
    if (m_rowsLoadCmd.mid(1, 4).toLower() == "scan") {
      QList<QByteArray> cmdParts = {m_rowsLoadCmd, m_keyFullPath,
                                    QString::number(m_scanCursor).toLatin1(),
                                    "COUNT", QString::number(count).toLatin1()};

      auto self = ValueEditor::Model::sharedFromThis().toWeakRef();

      m_connection->cmd(
          cmdParts, m_notifier.data(), -1,
          [this, callback, rowStart, self](RedisClient::Response r) {
            if (!r.isValidScanResponse()) {
              callback(QCoreApplication::translate(
                           "RDM", "Cannot parse scan response"),
                       0);
              return;
            }

            if (r.getCursor() > 0) {
              m_scanCursor = r.getCursor();
            }

            try {
              unsigned long addedRows =
                  addLoadedRowsToCache(r.getCollection(), rowStart);
              callback(QString(), addedRows);
            } catch (const std::runtime_error& e) {
              callback(QString(e.what()), 0);
            }
          },
          [self, callback](QString err) {
            if (!self) {
              return;
            }

            return callback(
                QCoreApplication::translate("RDM", "Connection error: ") + err,
                0);
          });

    } else {
      getRowsRange(
          getRangeCmd(rowStart, count),
          [this, callback, rowStart](const QString& err, QVariantList result) {
            if (!err.isEmpty()) return callback(err, 0);

            unsigned long addedRows = addLoadedRowsToCache(result, rowStart);
            callback(QString(), addedRows);
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

    executeCmd(
        {m_rowsCountCmd, m_keyFullPath}, c,
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
          -1);
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
        cmd, m_notifier.data(), -1,
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

  virtual int addLoadedRowsToCache(const QVariantList& rows,
                                   QVariant rowStart) = 0;

  QVariant filter(const QString& key) const override {
    return m_filters.value(key, QVariant());
  };

  void setFilter(const QString& k, QVariant v) override {
      m_filters[k] = v;
      qDebug() << "filter:" << k << v;
  }

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
  long long m_scanCursor;
  QSharedPointer<ValueEditor::ModelSignals> m_notifier;

  QVariantMap m_filters;
};
