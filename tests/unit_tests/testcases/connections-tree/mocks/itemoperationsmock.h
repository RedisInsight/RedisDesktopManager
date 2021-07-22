#pragma once
#include <QFuture>
#include "connections-tree/items/abstractnamespaceitem.h"
#include "connections-tree/items/keyitem.h"
#include "connections-tree/items/treeitem.h"
#include "connections-tree/operations.h"

class ItemOperationsMock : public ConnectionsTree::Operations {
 public:
  ItemOperationsMock(bool positive_mode = true)
      : m_positive_mode(positive_mode) {}

  QMap<int, int> databases;
  QString databaseLoadingError;

  virtual QFuture<void> getDatabases(
      std::function<void(QMap<int, int>, const QString& err)> callback) override {
    if (m_positive_mode) {
      callback(databases, databaseLoadingError);
      return QFuture<void>();
    } else {
      return QFuture<void>();
    }
  }

  QList<QByteArray> keys;
  void loadNamespaceItems(
      QSharedPointer<ConnectionsTree::AbstractNamespaceItem> parent,
      const QString&, std::function<void(const QString&)> callback,
      QSet<QByteArray>) override {
    if (m_positive_mode) {
      for (QByteArray key : keys) {
        parent->append(QSharedPointer<ConnectionsTree::KeyItem>(
            new ConnectionsTree::KeyItem(key, parent, parent->model())));
      }
      parent->notifyModel();

      callback(QString());
    } else {
      throw ConnectionsTree::Operations::Exception("fake error");
    }
  }

  QString namespaceSeparator = ":";
  QString getNamespaceSeparator() { return namespaceSeparator; }

  QString defaultKeysFilter = "*";
  QString defaultFilter() { return defaultKeysFilter; }

  void disconnect() {}

  virtual void openKeyTab(QSharedPointer<ConnectionsTree::KeyItem>,
                          bool) override {}

  virtual void openConsoleTab(int dbIndex = 0) override {}

  virtual void openServerStats() override {}

  void openNewKeyDialog(int, std::function<void()>,
                        QString = QString()) override {}

  virtual void notifyDbWasUnloaded(int) override {}

  virtual void deleteDbKey(ConnectionsTree::KeyItem&,
                           std::function<void(const QString&)>) override {}

  virtual void deleteDbNamespace(ConnectionsTree::NamespaceItem&) override {}

  virtual void flushDb(int, std::function<void(const QString&)>) override {}

  virtual QString mode() { return QString("fake"); }

  virtual bool isConnected() const { return true; }

  virtual void duplicateConnection() override {}

  virtual QFuture<bool> connectionSupportsMemoryOperations() override {
    return QFuture<bool>();
  }

  virtual void getUsedMemory(const QList<QByteArray>& keys, int dbIndex,
                                           std::function<void(qlonglong)> result,
                                           std::function<void(qlonglong)> progress) override {
  }

  void resetConnection() override {}

  void deleteDbKeys(ConnectionsTree::DatabaseItem &db) override {};
  void setTTL(ConnectionsTree::AbstractNamespaceItem &ns) override {};
  void copyKeys(ConnectionsTree::AbstractNamespaceItem &ns) override {};
  void importKeysFromRdb(ConnectionsTree::DatabaseItem &ns) override {};
  void openKeyIfExists(const QByteArray& key,
                       QSharedPointer<ConnectionsTree::DatabaseItem> parent,
                       std::function<void(const QString&, bool)> callback) override {};

  QString connectionName() const override {
      return "test";
  }

  QVariantMap getFilterHistory() override {
      return QVariantMap();
  }

 protected:
  bool m_positive_mode;
};
