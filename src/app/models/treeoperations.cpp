#include "treeoperations.h"

#include <asyncfuture.h>
#include <qredisclient/redisclient.h>
#include <QRegExp>
#include <QRegularExpression>
#include <QRegularExpressionMatchIterator>
#include <QSet>
#include <QtConcurrent>
#include <algorithm>

#include "app/events.h"
#include "connections-tree/items/serveritem.h"
#include "connections-tree/items/databaseitem.h"
#include "connections-tree/items/namespaceitem.h"
#include "connections-tree/keysrendering.h"

TreeOperations::TreeOperations(const ServerConfig &config,
    QSharedPointer<Events> events)
    : m_events(events), m_dbCount(0),
      m_connectionMode(RedisClient::Connection::Mode::Normal),
      m_config(config){
  m_connection = QSharedPointer<RedisClient::Connection>(
              new RedisClient::Connection(config));
  m_events->registerLoggerForConnection(*m_connection);
}

void TreeOperations::loadDatabases(
    QSharedPointer<RedisClient::Connection> c,
    QSharedPointer<AsyncFuture::Deferred<void>> d,
    std::function<void(RedisClient::DatabaseList, const QString&)> callback) {
  if (!d) return;

  auto connection = c->clone(false);
  auto connectionWRef = connection.toWeakRef();

  d->onCanceled([connectionWRef]() {
    QtConcurrent::run([connectionWRef]() {
      auto connection = connectionWRef.toStrongRef();
      if (connection) connection->disconnect();
    });
  });

  m_events->registerLoggerForConnection(*connection);

  if (!connect(connection)) {
    return callback(RedisClient::DatabaseList(),
                    QString("Cannot connect to redis-server"));
  }

  if (d && d->future().isCanceled()) {
    return;
  }

  RedisClient::DatabaseList availableDatabeses = connection->getKeyspaceInfo();

  if (connection->mode() == RedisClient::Connection::Mode::Cluster) {
    return callback(availableDatabeses, QString());
  }

  // detect all databases
  RedisClient::Response scanningResp;
  int lastDbIndex =
      (availableDatabeses.size() == 0) ? 0 : availableDatabeses.lastKey() + 1;

  if (m_dbCount > 0) {
    for (int index = lastDbIndex; index < m_dbCount; index++) {
      availableDatabeses.insert(index, 0);
    }

    return callback(availableDatabeses, QString());
  } else {
    m_dbCount = lastDbIndex;

    auto collectedDatabases = QSharedPointer<RedisClient::DatabaseList>(
        new RedisClient::DatabaseList(availableDatabeses));

    recursiveSelectScan(d, connection, collectedDatabases, callback);
  }
}

void TreeOperations::recursiveSelectScan(
    QSharedPointer<AsyncFuture::Deferred<void>> d,
    QSharedPointer<RedisClient::Connection> c,
    QSharedPointer<RedisClient::DatabaseList> dbList,
    std::function<void(RedisClient::DatabaseList, const QString&)> callback) {
  if (d && d->future().isCanceled()) {
    return;
  }

  if (m_dbCount >= m_config.databaseScanLimit() || !c) {
    return callback(*dbList, QString());
  }

  auto errHandler = [callback, dbList](const QString& err) {
    if (dbList && dbList->size() > 0) {
      callback(*dbList, QString());
    } else {
      callback(RedisClient::DatabaseList(), err);
    }
  };

  c->cmd(
      {"select", QString::number(m_dbCount).toLatin1()}, this, -1,
      [this, dbList, c, callback, d](const RedisClient::Response& scanningResp) {
        if (d && d->future().isCanceled()) {
          return;
        }

        if (!scanningResp.isOkMessage()) {
          callback(*dbList, QString());
          return;
        }

        dbList->insert(m_dbCount, 0);
        m_dbCount++;

        recursiveSelectScan(d, c, dbList, callback);
      },
      errHandler);
}

bool TreeOperations::connect(QSharedPointer<RedisClient::Connection> c) {
  if (c->isConnected()) return true;

  try {
    if (!c->connect(true)) {
      emit m_events->error(
          QCoreApplication::translate(
              "RESP", "Cannot connect to server '%1'. Check log for details.")
              .arg(m_connection->getConfig().name()));
      return false;
    }

    m_connectionMode = c->mode();
    return true;
  } catch (const RedisClient::Connection::SSHSupportException& e) {
      emit m_events->error(
          QCoreApplication::translate("RESP", "Open Source version of RESP.app <b>doesn't support SSH tunneling</b>.<br /><br /> "
                                             "To get fully-featured application, please buy subscription on "
                                             "<a href='https://resp.app/subscriptions'>resp.app</a>. <br/><br />"
                                             "Every single subscription gives us funds to continue "
                                             "the development process and provide support to our users. <br />"
                                             "If you have any questions please feel free to contact us "
                                             "at <a href='mailto:support@resp.app'>support@resp.app</a> "
                                             "or join <a href='https://t.me/RedisDesktopManager'>Telegram chat</a>.")
      );
      return false;
  } catch (const RedisClient::Connection::Exception& e) {
    emit m_events->error(
        QCoreApplication::translate("RESP", "Connection error: ") +
        QString(e.what()));
    return false;
  }
}

void TreeOperations::requestBulkOperation(
    ConnectionsTree::AbstractNamespaceItem& ns,
    BulkOperations::Manager::Operation op,
    BulkOperations::AbstractOperation::OperationCallback callback) {
  QString pattern =
      QString("%1%2*")
          .arg(QString::fromUtf8(ns.getFullPath()))
          .arg(ns.getFullPath().size() > 0 ? m_config.namespaceSeparator()
                                           : "");
  QRegExp filter(pattern, Qt::CaseSensitive, QRegExp::Wildcard);

  auto dbIndex = ns.getDbIndex();

  getReadyConnection([this, dbIndex, filter, op,
                      callback](QSharedPointer<RedisClient::Connection> c) {
    // NOTE(u_glide): Use "clean" connection wihout logger here for better
    // performance
    emit m_events->requestBulkOperation(c->clone(), dbIndex, op, filter,
                                        callback);
  });
}

void TreeOperations::getReadyConnection(TreeOperations::PendingOperation callback)
{
    if (m_config.askForSshPassword() && m_config.sshPassword().isEmpty()) {
        m_pendingOperation = callback;
        m_config.setOwner(sharedFromThis().toWeakRef());
        emit secretRequired(m_config, ServerConfig::SSH_SECRET_ID);
    } else {
        return callback(m_connection);
    }
}

QFuture<void> TreeOperations::getDatabases(
    QSharedPointer<GetDatabasesCallback> callback) {
  m_dbScanOp = QSharedPointer<AsyncFuture::Deferred<void>>(
      new AsyncFuture::Deferred<void>());

  getReadyConnection(
      [this, callback](QSharedPointer<RedisClient::Connection> c) {
        QtConcurrent::run(this, &TreeOperations::loadDatabases, c, m_dbScanOp,
                          [callback](RedisClient::DatabaseList dbs, const QString& err){
            callback->call(dbs, err);
        });
      });

  return m_dbScanOp->future();
}

void TreeOperations::loadNamespaceItems(
    uint dbIndex, const QString& filter,
    QSharedPointer<LoadNamespaceItemsCallback> callback) {
  QString keyPattern = filter.isEmpty() ? m_config.keysPattern() : filter;

  if (m_filterHistory.contains(keyPattern)) {
    m_filterHistory[keyPattern] = m_filterHistory[keyPattern].toInt() + 1;
  } else {
    m_filterHistory[keyPattern] = 1;
  }
  m_config.setFilterHistory(m_filterHistory);
  emit filterHistoryUpdated();

  QSettings settings;
  qlonglong scanLimit = settings.value("app/scanLimit", DEFAULT_SCAN_LIMIT).toLongLong();

  getReadyConnection([this, dbIndex, filter, callback,
                      keyPattern, scanLimit](QSharedPointer<RedisClient::Connection> c) {
    if (!connect(c)) return;

    auto processErr = [callback](const QString& err) {
      return callback->call(
          RedisClient::Connection::RawKeysList(),
          QCoreApplication::translate("RESP", "Cannot load keys: %1").arg(err));
    };

    auto callbackWrapper = [callback](const RedisClient::Connection::RawKeysList &keys,
        const QString &err) {
      return callback->call(keys, err);
    };

    try {
      if (m_connection->mode() == RedisClient::Connection::Mode::Cluster) {
        m_connection->getClusterKeys(callbackWrapper, keyPattern, scanLimit);
      } else {
        m_connection->cmd(
            {"ping"}, this, dbIndex,
            [this, callbackWrapper, keyPattern,
             processErr, scanLimit](const RedisClient::Response& r) {
              if (r.isErrorMessage()) {
                return processErr(r.value().toString());
              }
              m_connection->getDatabaseKeys(callbackWrapper, keyPattern, -1, scanLimit);
            },
            [processErr](const QString& err) { return processErr(err); });
      }

    } catch (const RedisClient::Connection::Exception& error) {
      processErr(error.what());
    }
  });
}

void TreeOperations::disconnect() { m_connection->disconnect(); }

void TreeOperations::resetConnection() {
  auto oldConnection = m_connection;
  setConnection(oldConnection->clone());

  QtConcurrent::run([oldConnection]() { oldConnection->disconnect(); });
}

QString TreeOperations::getNamespaceSeparator() {
  return m_config.namespaceSeparator();
}

QString TreeOperations::defaultFilter() { return m_config.keysPattern(); }

QVariantMap TreeOperations::getFilterHistory() {
    m_filterHistory = m_config.filterHistory();
    return m_filterHistory;
}

QString TreeOperations::connectionName() const {
    return m_config.name();
}

void TreeOperations::openKeyTab(QSharedPointer<ConnectionsTree::KeyItem> key,
                                bool openInNewTab) {
  getReadyConnection(
      [this, key, openInNewTab](QSharedPointer<RedisClient::Connection> c) {
        emit m_events->openValueTab(c, key, openInNewTab);
      });
}

void TreeOperations::openConsoleTab(int dbIndex) {
  getReadyConnection(
      [this, dbIndex](QSharedPointer<RedisClient::Connection> c) {
        emit m_events->openConsole(c, dbIndex, true);
      });
}

void TreeOperations::openNewKeyDialog(int dbIndex,
                                      QSharedPointer<OpenNewKeyDialogCallback> callback,
                                      QString keyPrefix) {
  getReadyConnection([this, dbIndex, callback,
                      keyPrefix](QSharedPointer<RedisClient::Connection> c) {
    emit m_events->newKeyDialog(c, callback, dbIndex, keyPrefix);
  });
}

void TreeOperations::openServerStats() {
  getReadyConnection([this](QSharedPointer<RedisClient::Connection> c) {
    emit m_events->openServerStats(c);
  });
}

void TreeOperations::duplicateConnection() {
  emit createNewConnection(m_config);
}

void TreeOperations::notifyDbWasUnloaded(int dbIndex) {
  emit m_events->closeDbKeys(m_connection, dbIndex);
}

void TreeOperations::deleteDbKey(ConnectionsTree::KeyItem& key,
                                 QSharedPointer<DeleteDbKeyCallback> callback) {
  getReadyConnection(
      [this, &key, callback](QSharedPointer<RedisClient::Connection> c) {
        c->cmd(
            {"DEL", key.getFullPath()}, this, key.getDbIndex(),
            [this, &key](RedisClient::Response) {
              key.setRemoved();
              QRegExp filter(key.getFullPath(), Qt::CaseSensitive,
                             QRegExp::Wildcard);
              if (m_events)
                m_events->closeDbKeys(m_connection, key.getDbIndex(), filter);
            },
            [this, callback](const QString& err) {
              QString errorMsg =
                  QCoreApplication::translate("RESP", "Delete key error: %1")
                      .arg(err);
              callback->call(errorMsg);
              if (m_events) m_events->error(errorMsg);
            });
      });
}

void TreeOperations::deleteDbKeys(ConnectionsTree::DatabaseItem& db) {
  auto self = sharedFromThis().toWeakRef();
  requestBulkOperation(
      db, BulkOperations::Manager::Operation::DELETE_KEYS,
      [self, this, &db](QRegExp filter, int, const QStringList&) {
        if (!self) {
          return;
        }
        uint dbIndex = db.getDbIndex();
        db.reload();

        if (m_events && m_connection) {
          getReadyConnection(
              [this, dbIndex, filter](QSharedPointer<RedisClient::Connection> c) {
                emit m_events->closeDbKeys(c, dbIndex, filter);
              });
        }
      });
}

void TreeOperations::deleteDbNamespace(ConnectionsTree::NamespaceItem& ns) {
  auto self = sharedFromThis().toWeakRef();
  requestBulkOperation(
      ns, BulkOperations::Manager::Operation::DELETE_KEYS,
      [this, self, &ns](QRegExp filter, int, const QStringList&) {
        if (!self) {
          return;
        }
        uint dbIndex = ns.getDbIndex();
        ns.setRemoved();

        if (m_events && m_connection) {
          getReadyConnection(
              [this, dbIndex, filter](QSharedPointer<RedisClient::Connection> c) {
                emit m_events->closeDbKeys(c, dbIndex, filter);
              });
        }
      });
}

void TreeOperations::setTTL(ConnectionsTree::AbstractNamespaceItem& ns) {
  requestBulkOperation(ns, BulkOperations::Manager::Operation::TTL,
                       [](QRegExp, int, const QStringList&) {});
}

void TreeOperations::copyKeys(ConnectionsTree::AbstractNamespaceItem& ns) {
  requestBulkOperation(ns, BulkOperations::Manager::Operation::COPY_KEYS,
                       [](QRegExp, int, const QStringList&) {});
}

void TreeOperations::importKeysFromRdb(ConnectionsTree::DatabaseItem& db) {
  getReadyConnection([this, &db](QSharedPointer<RedisClient::Connection> c) {
    emit m_events->requestBulkOperation(
        c->clone(), db.getDbIndex(),
        BulkOperations::Manager::Operation::IMPORT_RDB_KEYS, QRegExp(".*"),
        [&db](QRegExp, int, const QStringList&) { db.reload(); });
  });
}

void TreeOperations::flushDb(int dbIndex,
                             QSharedPointer<FlushDbCallback> callback) {

  auto callbackWrapper = [callback](const QString &err) {
    callback->call(err);
  };

  getReadyConnection(
      [dbIndex, callbackWrapper](QSharedPointer<RedisClient::Connection> c) {
        try {
          c->flushDbKeys(dbIndex, callbackWrapper);
        } catch (const RedisClient::Connection::Exception& e) {
          throw ConnectionsTree::Operations::Exception(
              QCoreApplication::translate("RESP", "Cannot flush database: ") +
              QString(e.what()));
        }
      });
}

QFuture<bool> TreeOperations::connectionSupportsMemoryOperations() {
    return m_connection->isCommandSupported({"MEMORY", "HELP"});
}

void TreeOperations::openKeyIfExists(const QByteArray& fullPath,
    QSharedPointer<ConnectionsTree::DatabaseItem> parent,
    QSharedPointer<OpenKeyIfExistsCallback> callback) {
  if (!parent) {
    qWarning() << "TreeOperations::openKeyIfExists > Invalid parent";
    return;
  }

  getReadyConnection([this, fullPath, parent,
                      callback](QSharedPointer<RedisClient::Connection> c) {
    c->cmd(
        {"exists", fullPath}, this, static_cast<int>(parent->getDbIndex()),
        [this, parent, fullPath, callback](RedisClient::Response r) {
          QVariant result = r.value();

          if (result.toByteArray() == "1") {
            auto key = QSharedPointer<ConnectionsTree::KeyItem>(
                new ConnectionsTree::KeyItem(fullPath, parent.toWeakRef(),
                                             parent->model(),
                                             parent->keysShortNameRendering()));

            emit m_events->openValueTab(m_connection, key, true);

            callback->call(QString(), true);
          } else {
            callback->call(QString(), false);
          }
        },
        [callback](const QString& err) { callback->call(err, false); });
  });
}

void TreeOperations::getUsedMemory(const QList<QByteArray>& keys, int dbIndex,
                                   QSharedPointer<GetUsedMemoryCallback> result,
                                   QSharedPointer<GetUsedMemoryCallback> progress) {
  QList<QList<QByteArray>> commands;

  for (int index = 0; index < keys.size(); ++index) {
    commands.append({"MEMORY", "USAGE", keys[index]});
  }

  int expectedResponses = commands.size();
  auto processedResponses = QSharedPointer<int>(new int(0));
  auto totalMemory = QSharedPointer<qlonglong>(new qlonglong(0));

  m_connection->pipelinedCmd(
      commands, this, dbIndex,
      [this, expectedResponses, processedResponses, totalMemory, progress,
       result](RedisClient::Response r, QString err) {
        if (!err.isEmpty()) {
          QString errorMsg =
              QCoreApplication::translate(
                  "RESP", "Cannot determine amount of used memory by key: %1")
                  .arg(err);
          m_events->error(errorMsg);          
        } else {
          QVariant incrResult = r.value();

          if (incrResult.canConvert(QVariant::LongLong)) {
            (*totalMemory) += incrResult.toLongLong();
            (*processedResponses)++;
          } else if (incrResult.canConvert(QVariant::List)) {
            auto responses = incrResult.toList();

            for (auto resp : responses) {
              (*totalMemory) += resp.toLongLong();
              (*processedResponses)++;
            }
          }

          if (progress)
            progress->call(*totalMemory);

          if ((*processedResponses) >= expectedResponses && result) {
            result->call(*totalMemory);
          }
        }
      }, true);
}

QString TreeOperations::mode() {
  if (m_connectionMode == RedisClient::Connection::Mode::Cluster) {
    return QString("cluster");
  } else if (m_connectionMode == RedisClient::Connection::Mode::Sentinel) {
    return QString("sentinel");
  } else {
    return QString("standalone");
  }
}

bool TreeOperations::isConnected() const { return m_connection->isConnected(); }

QSharedPointer<RedisClient::Connection> TreeOperations::connection()
{
    return m_connection;
}

void TreeOperations::setConnection(QSharedPointer<RedisClient::Connection> c) {
  m_connection = c;
  m_events->registerLoggerForConnection(*c);
}

ServerConfig TreeOperations::config()
{
    m_config.setOwner(sharedFromThis().toWeakRef());
    return m_config;
}

void TreeOperations::setConfig(const ServerConfig &c)
{
    m_config = c;
    m_config.setOwner(sharedFromThis().toWeakRef());
    m_connection->setConnectionConfig(m_config);
    emit configUpdated();
}

void TreeOperations::proceedWithSecret(const ServerConfig &c)
{
    m_config = c;
    m_config.setOwner(sharedFromThis().toWeakRef());
    m_connection->setConnectionConfig(m_config);

    if (m_pendingOperation) {
        m_pendingOperation(m_connection);
    } else {
        qWarning() << "Unknown proceedWithSecret request";
        return;
    }
}

QString TreeOperations::iconColor()
{
    return m_config.iconColor();
}
