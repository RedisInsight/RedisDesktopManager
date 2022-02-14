#pragma once
#include <qredisclient/connection.h>

#include <QFuture>
#include <QMap>
#include <QSharedPointer>
#include <QString>
#include <QStringList>
#include <functional>

#include "exception.h"
#include "modules/common/callbackwithowner.h"

namespace Console {
class Operations;
}

namespace ConnectionsTree {

class KeyItem;
class NamespaceItem;
class AbstractNamespaceItem;
class DatabaseItem;
class TreeItem;

class Operations {
  ADD_EXCEPTION
 public:
  /**
   * List of databases with keys counters
   * @emit databesesLoaded
   **/
  using DbMapping = QMap<int, int>;
  using GetDatabasesCallback =
      CallbackWithOwner<TreeItem, DbMapping, const QString&>;

  virtual QFuture<void> getDatabases(QSharedPointer<GetDatabasesCallback>) = 0;

  /**
   * @brief loadNamespaceItems
   * @param dbIndex
   * @param filter
   * @param callback
   */
  using LoadNamespaceItemsCallback =
      CallbackWithOwner<TreeItem, const RedisClient::Connection::RawKeysList&,
                        const QString&>;

  virtual void loadNamespaceItems(uint dbIndex, const QString& filter,
                                  QSharedPointer<LoadNamespaceItemsCallback>) = 0;

  /**
   * Cancel all operations & close connection
   * @brief disconnect
   */
  virtual void disconnect() = 0;

  /**
    Cancel all operations & reconnect
   * @brief resetConnection
   */
  virtual void resetConnection() = 0;

  /**
   * @brief getNamespaceSeparator
   * @return
   */
  virtual QString getNamespaceSeparator() = 0;

  virtual QString iconColor() = 0;

  virtual QString defaultFilter() = 0;

  virtual QVariantMap getFilterHistory() = 0;

  virtual QString connectionName() const = 0;

  virtual void openKeyTab(QSharedPointer<KeyItem> key, bool openInNewTab) = 0;

  virtual void openConsoleTab(int dbIndex = 0) = 0;

  using OpenNewKeyDialogCallback = CallbackWithOwner<TreeItem>;

  virtual void openNewKeyDialog(int dbIndex, QSharedPointer<OpenNewKeyDialogCallback> callback,
                                QString keyPrefix = QString()) = 0;

  virtual void openServerStats() = 0;

  virtual void duplicateConnection() = 0;

  virtual void notifyDbWasUnloaded(int dbIndex) = 0;

  using DeleteDbKeyCallback = CallbackWithOwner<TreeItem, const QString&>;

  virtual void deleteDbKey(ConnectionsTree::KeyItem& key,
                           QSharedPointer<DeleteDbKeyCallback> callback) = 0;

  virtual void deleteDbKeys(ConnectionsTree::DatabaseItem& db) = 0;

  virtual void deleteDbNamespace(ConnectionsTree::NamespaceItem& ns) = 0;

  virtual void setTTL(ConnectionsTree::AbstractNamespaceItem& ns) = 0;

  virtual void copyKeys(ConnectionsTree::AbstractNamespaceItem& ns) = 0;

  virtual void importKeysFromRdb(ConnectionsTree::DatabaseItem& ns) = 0;

  using FlushDbCallback = CallbackWithOwner<TreeItem, const QString&>;

  virtual void flushDb(int dbIndex, QSharedPointer<FlushDbCallback> callback) = 0;

  using OpenKeyIfExistsCallback = CallbackWithOwner<TreeItem, const QString&, bool>;

  virtual void openKeyIfExists(
      const QByteArray& key,
      QSharedPointer<ConnectionsTree::DatabaseItem> parent,
      QSharedPointer<OpenKeyIfExistsCallback> callback) = 0;

  virtual QString mode() = 0;

  virtual bool isConnected() const = 0;

  virtual QFuture<bool> connectionSupportsMemoryOperations() = 0;

  using GetUsedMemoryCallback = CallbackWithOwner<TreeItem, qlonglong>;

  virtual void getUsedMemory(
      const QList<QByteArray>& keys, int dbIndex,
      QSharedPointer<GetUsedMemoryCallback> result,
      QSharedPointer<GetUsedMemoryCallback> progress) = 0;

  virtual ~Operations() {}
};
}  // namespace ConnectionsTree
