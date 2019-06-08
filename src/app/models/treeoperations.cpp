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
#include "connections-tree/items/databaseitem.h"
#include "connections-tree/items/namespaceitem.h"
#include "connections-tree/keysrendering.h"

TreeOperations::TreeOperations(
    QSharedPointer<RedisClient::Connection> connection,
    QSharedPointer<Events> events)
    : m_connection(connection), m_events(events), m_dbCount(0) {
  m_events->registerLoggerForConnection(*connection);
}

bool TreeOperations::loadDatabases(
    std::function<void(RedisClient::DatabaseList)> callback) {
  auto connection = m_connection->clone();
  m_events->registerLoggerForConnection(*connection);

  connect(connection);

  RedisClient::DatabaseList availableDatabeses = connection->getKeyspaceInfo();

  if (connection->mode() != RedisClient::Connection::Mode::Cluster) {
    // detect all databases
    RedisClient::Response scanningResp;
    int lastDbIndex =
        (availableDatabeses.size() == 0) ? 0 : availableDatabeses.lastKey() + 1;

    if (m_dbCount > 0) {
      for (int index = lastDbIndex; index < m_dbCount; index++) {
        availableDatabeses.insert(index, 0);
      }
    } else {
      uint dbScanLimit = conf().databaseScanLimit();

      for (int index = lastDbIndex; index < dbScanLimit; index++) {
        try {
          scanningResp = connection->commandSync(
              {"select", QString::number(index).toLatin1()});
        } catch (const RedisClient::Connection::Exception& e) {
          throw ConnectionsTree::Operations::Exception(
              QCoreApplication::translate("RDM", "Connection error: ") +
              QString(e.what()));
        }

        if (!scanningResp.isOkMessage()) {
          break;
        }

        availableDatabeses.insert(index, 0);
        ++lastDbIndex;
      }
      m_dbCount = lastDbIndex;
    }
  }

  callback(availableDatabeses);
  return true;
}

ServerConfig TreeOperations::conf() const {
  return static_cast<ServerConfig>(m_connection->getConfig());
}

void TreeOperations::connect(QSharedPointer<RedisClient::Connection> c) {
  if (c->isConnected()) return;

  try {
    if (!c->connect(true)) {
      emit m_events->error(
          QCoreApplication::translate(
              "RDM", "Cannot connect to server '%1'. Check log for details.")
              .arg(m_connection->getConfig().name()));
      return;
    }

  } catch (const RedisClient::Connection::Exception& e) {
    emit m_events->error(
        QCoreApplication::translate("RDM", "Connection error: ") +
        QString(e.what()));
    return;
  }
}

void TreeOperations::requestBulkOperation(
    ConnectionsTree::AbstractNamespaceItem& ns,
    BulkOperations::Manager::Operation op,
    BulkOperations::AbstractOperation::OperationCallback callback) {
  QString pattern =
      QString("%1%2*")
          .arg(QString::fromUtf8(ns.getFullPath()))
          .arg(ns.getFullPath().size() > 0 ? conf().namespaceSeparator() : "");
  QRegExp filter(pattern, Qt::CaseSensitive, QRegExp::Wildcard);

  emit m_events->requestBulkOperation(m_connection, ns.getDbIndex(), op, filter,
                                      callback);
}

QFuture<void> TreeOperations::getDatabases(
    std::function<void(RedisClient::DatabaseList)> callback) {
  return QtConcurrent::run(this, &TreeOperations::loadDatabases, callback);
}

void TreeOperations::loadNamespaceItems(
    QSharedPointer<ConnectionsTree::AbstractNamespaceItem> parent,
    const QString& filter, std::function<void(const QString& err)> callback,
    QSet<QByteArray> expandedNs) {
  QString keyPattern = filter.isEmpty() ? conf().keysPattern() : filter;

  auto renderingCallback =
      [this, callback, filter, parent, expandedNs](
          const RedisClient::Connection::RawKeysList& keylist,
          const QString& err) {
        if (!err.isEmpty()) {
          return callback(err);
        }

        auto settings = ConnectionsTree::KeysTreeRenderer::RenderingSettigns{
            QRegExp(filter), getNamespaceSeparator(), parent->getDbIndex()};

        AsyncFuture::observe(
            QtConcurrent::run(&ConnectionsTree::KeysTreeRenderer::renderKeys,
                              sharedFromThis(), keylist, parent, settings,
                              expandedNs))
            .subscribe([callback]() { callback(QString()); });
      };

  auto thinRenderingCallback =
      [this, callback, parent, expandedNs](
          const RedisClient::Connection::NamespaceItems& items,
          const QString& err) {
        if (!err.isEmpty()) {
          return callback(err);
        }

        ConnectionsTree::KeysTreeRenderer::renderNamespaceItems(
            sharedFromThis(), items, parent, expandedNs);

        callback(QString());
      };

  connect(m_connection);

  try {
    if (m_connection->mode() == RedisClient::Connection::Mode::Cluster) {
      m_connection->getClusterKeys(renderingCallback, keyPattern);
    } else {
      if (conf().luaKeysLoading()) {
        m_connection->getNamespaceItems(thinRenderingCallback,
                                        getNamespaceSeparator(), filter,
                                        parent->getDbIndex());
      } else {
        m_connection->getDatabaseKeys(renderingCallback, keyPattern,
                                      parent->getDbIndex());
      }
    }

  } catch (const RedisClient::Connection::Exception& error) {
    callback(QCoreApplication::translate("RDM", "Cannot load keys: %1")
                 .arg(error.what()));
  }
}

void TreeOperations::disconnect() { m_connection->disconnect(); }

void TreeOperations::resetConnection() {
  auto oldConnection = m_connection;
  setConnection(oldConnection->clone());

  QtConcurrent::run([oldConnection]() { oldConnection->disconnect(); });
}

QString TreeOperations::getNamespaceSeparator() {
  return conf().namespaceSeparator();
}

QString TreeOperations::defaultFilter() { return conf().keysPattern(); }

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
  emit m_events->createNewConnection(m_connection->getConfig());
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
        emit m_events->closeDbKeys(m_connection, key.getDbIndex(), filter);
      },
      [this, &callback](const QString& err) {
        QString errorMsg =
            QCoreApplication::translate("RDM", "Delete key error: %1").arg(err);
        callback(errorMsg);
        m_events->error(errorMsg);
      });
}

void TreeOperations::deleteDbKeys(ConnectionsTree::DatabaseItem& db) {
  requestBulkOperation(db, BulkOperations::Manager::Operation::DELETE_KEYS,
                       [this, &db](QRegExp filter, int, const QStringList&) {
                         db.reload();
                         emit m_events->closeDbKeys(m_connection,
                                                    db.getDbIndex(), filter);
                       });
}

void TreeOperations::deleteDbNamespace(ConnectionsTree::NamespaceItem& ns) {
  requestBulkOperation(ns, BulkOperations::Manager::Operation::DELETE_KEYS,
                       [this, &ns](QRegExp filter, int, const QStringList&) {
                         emit m_events->closeDbKeys(m_connection,
                                                    ns.getDbIndex(), filter);
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
      m_connection, db.getDbIndex(),
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

QFuture<qlonglong> TreeOperations::getUsedMemory(const QByteArray& key,
                                                 int dbIndex) {
  auto d = QSharedPointer<AsyncFuture::Deferred<qlonglong>>(
      new AsyncFuture::Deferred<qlonglong>());

  m_connection->cmd(
      {"MEMORY", "USAGE", key}, this, dbIndex,
      [d](RedisClient::Response r) {
        QVariant result = r.value();

        if (result.canConvert(QVariant::LongLong)) {
          d->complete(result.toLongLong());
        } else {
          d->complete(0);
        }
      },
      [this, d](const QString& err) {
        QString errorMsg =
            QCoreApplication::translate("RDM", "Cannot used memory for key: %1")
                .arg(err);
        m_events->error(errorMsg);
        d->complete(0);
      });

  return d->future();
}

QString TreeOperations::mode() {
  if (m_connection->mode() == RedisClient::Connection::Mode::Cluster) {
    return QString("cluster");
  } else if (m_connection->mode() == RedisClient::Connection::Mode::Sentinel) {
    return QString("sentinel");
  } else {
    return QString("standalone");
  }
}

bool TreeOperations::isConnected() const { return m_connection->isConnected(); }

void TreeOperations::setConnection(QSharedPointer<RedisClient::Connection> c) {
  m_connection = c;
  m_events->registerLoggerForConnection(*c);
}
