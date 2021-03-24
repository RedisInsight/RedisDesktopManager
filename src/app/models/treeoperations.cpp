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

TreeOperations::~TreeOperations() {
  if (m_connection) {
    m_connection->disconnect();
    m_connection->deleteLater();
  }
}

void TreeOperations::loadDatabases(
    QSharedPointer<AsyncFuture::Deferred<void>> d,
    std::function<void(RedisClient::DatabaseList, const QString&)> callback) {
  if (!d)
    return;

  auto connection = m_connection->clone(false);

  d->onCanceled([connection](){
      QtConcurrent::run([connection]() { if (connection) connection->disconnect(); });
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
              "RDM", "Cannot connect to server '%1'. Check log for details.")
              .arg(m_connection->getConfig().name()));
      return false;
    }

    m_connectionMode = c->mode();
    return true;
  } catch (const RedisClient::Connection::SSHSupportException& e) {
      emit m_events->error(
          QCoreApplication::translate("RDM", "Open Source version of RDM <b>doesn't support SSH tunneling</b>.<br /><br /> "
                                             "To get fully-featured application, please buy subscription on "
                                             "<a href='https://rdm.dev/subscriptions'>rdm.dev</a>. <br/><br />"
                                             "Every single subscription gives us funds to continue "
                                             "the development process and provide support to our users. <br />"
                                             "If you have any questions please feel free to contact us "
                                             "at <a href='mailto:support@rdm.dev'>support@rdm.dev</a> "
                                             "or join <a href='https://t.me/RedisDesktopManager'>Telegram chat</a>.")
      );
      return false;
  } catch (const RedisClient::Connection::Exception& e) {
    emit m_events->error(
        QCoreApplication::translate("RDM", "Connection error: ") +
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
          .arg(ns.getFullPath().size() > 0 ? m_config.namespaceSeparator() : "");
  QRegExp filter(pattern, Qt::CaseSensitive, QRegExp::Wildcard);

  // NOTE(u_glide): Use "clean" connection wihout logger here for better performance
  emit m_events->requestBulkOperation(m_connection->clone(), ns.getDbIndex(), op, filter,
                                      callback);
}

QFuture<void> TreeOperations::getDatabases(
    std::function<void(RedisClient::DatabaseList, const QString&)> callback) {
  m_dbScanOp = QSharedPointer<AsyncFuture::Deferred<void>>(
            new AsyncFuture::Deferred<void>());

  QtConcurrent::run(this, &TreeOperations::loadDatabases, m_dbScanOp, callback);

  return m_dbScanOp->future();
}

void TreeOperations::loadNamespaceItems(
    QSharedPointer<ConnectionsTree::AbstractNamespaceItem> parent,
    const QString& filter, std::function<void(const QString& err)> callback,
    QSet<QByteArray> expandedNs) {
  QString keyPattern = filter.isEmpty() ? m_config.keysPattern() : filter;

  if (m_filterHistory.contains(keyPattern)) {
      m_filterHistory[keyPattern] = m_filterHistory[keyPattern].toInt() + 1;
  } else {
      m_filterHistory[keyPattern] = 1;
  }
  m_config.setFilterHistory(m_filterHistory);
  emit filterHistoryUpdated();

  auto renderingCallback =
      [this, callback, filter, parent, expandedNs](
          const RedisClient::Connection::RawKeysList& keylist,
          const QString& err) {
        if (!err.isEmpty()) {
          return callback(err);
        }

        auto settings = ConnectionsTree::KeysTreeRenderer::RenderingSettigns{
            QRegExp(filter), getNamespaceSeparator(), parent->getDbIndex(),
            true};

        AsyncFuture::observe(
            QtConcurrent::run(&ConnectionsTree::KeysTreeRenderer::renderKeys,
                              sharedFromThis(), keylist, parent, settings,
                              expandedNs))
            .subscribe([callback]() { callback(QString()); });
      };

  if (!connect(m_connection)) return;

  auto processErr = [callback](const QString& err) {
    return callback(
        QCoreApplication::translate("RDM", "Cannot load keys: %1").arg(err));
  };  

  try {
    if (m_connection->mode() == RedisClient::Connection::Mode::Cluster) {
      m_connection->getClusterKeys(renderingCallback, keyPattern);
    } else {
      m_connection->cmd(
          {"ping"}, this, parent->getDbIndex(),
          [this, callback, renderingCallback, keyPattern,
           processErr](const RedisClient::Response& r) {
            if (r.isErrorMessage()) {
              return processErr(r.value().toString());
            }
            m_connection->getDatabaseKeys(renderingCallback, keyPattern, -1);
          },
          [processErr](const QString& err) { return processErr(err); });
    }

  } catch (const RedisClient::Connection::Exception& error) {
    processErr(error.what());
  }
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
  emit m_events->openValueTab(m_connection, key, openInNewTab);
}

void TreeOperations::openConsoleTab(int dbIndex) {
  emit m_events->openConsole(m_connection, dbIndex);
}

void TreeOperations::openNewKeyDialog(int dbIndex,
                                      std::function<void()> callback,
                                      QString keyPrefix) {
  emit m_events->newKeyDialog(m_connection, callback, dbIndex, keyPrefix);
}

void TreeOperations::openServerStats() {
  emit m_events->openServerStats(m_connection);
}

void TreeOperations::duplicateConnection() {
  emit createNewConnection(m_config);
}

void TreeOperations::notifyDbWasUnloaded(int dbIndex) {
  emit m_events->closeDbKeys(m_connection, dbIndex);
}

void TreeOperations::deleteDbKey(ConnectionsTree::KeyItem& key,
                                 std::function<void(const QString&)> callback) {
  m_connection->cmd(
      {"DEL", key.getFullPath()}, this, key.getDbIndex(),
      [this, &key](RedisClient::Response) {
        key.setRemoved();
        QRegExp filter(key.getFullPath(), Qt::CaseSensitive, QRegExp::Wildcard);
        if (m_events) m_events->closeDbKeys(m_connection, key.getDbIndex(), filter);
      },
      [this, callback](const QString& err) {
        QString errorMsg =
            QCoreApplication::translate("RDM", "Delete key error: %1").arg(err);
        callback(errorMsg);
        if (m_events) m_events->error(errorMsg);
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

        db.reload();

        if (m_events) {
          emit m_events->closeDbKeys(m_connection, db.getDbIndex(), filter);
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

        ns.setRemoved();

        if (m_events) {
          emit m_events->closeDbKeys(m_connection, ns.getDbIndex(), filter);
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
  emit m_events->requestBulkOperation(
      m_connection->clone(), db.getDbIndex(),
      BulkOperations::Manager::Operation::IMPORT_RDB_KEYS, QRegExp(".*"),
      [&db](QRegExp, int, const QStringList&) { db.reload(); });
}

void TreeOperations::flushDb(int dbIndex,
                             std::function<void(const QString&)> callback) {
  try {
    m_connection->flushDbKeys(dbIndex, callback);
  } catch (const RedisClient::Connection::Exception& e) {
    throw ConnectionsTree::Operations::Exception(
        QCoreApplication::translate("RDM", "Cannot flush database: ") +
        QString(e.what()));
  }
}

QFuture<bool> TreeOperations::connectionSupportsMemoryOperations() {
    return m_connection->isCommandSupported({"MEMORY", "HELP"});
}

void TreeOperations::openKeyIfExists(
    const QByteArray& fullPath,
    QSharedPointer<ConnectionsTree::DatabaseItem> parent,
    std::function<void(const QString&, bool)> callback) {
  if (!parent) {
    qWarning() << "TreeOperations::openKeyIfExists > Invalid parent";
    return;
  }

  m_connection->cmd(
      {"exists", fullPath}, this, static_cast<int>(parent->getDbIndex()),
      [this, parent, fullPath, callback](RedisClient::Response r) {
        QVariant result = r.value();

        if (result.toByteArray() == "1") {
          auto key = QSharedPointer<ConnectionsTree::KeyItem>(
              new ConnectionsTree::KeyItem(fullPath, parent.toWeakRef(),
                                           parent->model()));

          emit m_events->openValueTab(m_connection, key, true);

          callback(QString(), true);
        } else {
          callback(QString(), false);
        }
      },
      [callback](const QString& err) { callback(err, false); });
}

void TreeOperations::getUsedMemory(const QList<QByteArray>& keys, int dbIndex,
                                   std::function<void(qlonglong)> result,
                                   std::function<void(qlonglong)> progress) {
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
                  "RDM", "Cannot determine amount of used memory by key: %1")
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

          progress(*totalMemory);

          if ((*processedResponses) >= expectedResponses) {
            result(*totalMemory);
          }
        }
      });
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
