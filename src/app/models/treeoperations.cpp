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

bool TreeOperations::loadDatabases(
    std::function<void(RedisClient::DatabaseList)> callback) {
  auto connection = m_connection->clone(false);
  m_events->registerLoggerForConnection(*connection);

  connect(connection);

  if (!connection->isConnected()) return false;

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
      uint dbScanLimit = m_config.databaseScanLimit();

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

    m_connectionMode = c->mode();

  } catch (const RedisClient::Connection::SSHSupportException& e) {
      emit m_events->error(
          QCoreApplication::translate("RDM", "Open Source version of Redis Desktop Manager <b>doesn't support SSH tunneling</b>.<br /><br /> "
                                             "To get fully-featured application, please buy subscription on "
                                             "<a href='https://redisdesktop.com/subscriptions'>redisdesktop.com</a>. <br/><br />"
                                             "Every single subscription gives us funds to continue "
                                             "the development process and provide support to our users. <br />"
                                             "If you have any questions please feel free to contact us "
                                             "at <a href='mailto:support@redisdesktop.com'>support@redisdesktop.com</a> "
                                             "or join <a href='https://t.me/RedisDesktopManager'>Telegram chat</a>.")
      );
      return;
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
          .arg(ns.getFullPath().size() > 0 ? m_config.namespaceSeparator() : "");
  QRegExp filter(pattern, Qt::CaseSensitive, QRegExp::Wildcard);

  // NOTE(u_glide): Use "clean" connection wihout logger here for better performance
  emit m_events->requestBulkOperation(m_connection->clone(), ns.getDbIndex(), op, filter,
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
  QString keyPattern = filter.isEmpty() ? m_config.keysPattern() : filter;

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

  connect(m_connection);

  auto processErr = [callback](const QString& err) {
    return callback(
        QCoreApplication::translate("RDM", "Cannot load keys: %1").arg(err));
  };

  if (!m_connection->isConnected()) return;

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

QString TreeOperations::connectionName() const
{
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
          QString errorMsg = QCoreApplication::translate(
                                 "RDM", "Cannot used memory for key: %1")
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
