#pragma once
#include <QList>
#include <QObject>

#include "connections-tree/operations.h"
#include "sortabletreeitem.h"

namespace ConnectionsTree {

class Model;

class ServerItem : public QObject, public SortableTreeItem {
  Q_OBJECT
 public:
  ServerItem(QSharedPointer<Operations> operations,
             Model &model,
             QWeakPointer<TreeItem> parent = QWeakPointer<TreeItem>());

  ~ServerItem();

  QString getDisplayName() const override;

  QString type() const override { return "server"; }

  QVariantMap metadata() const override;

  QList<QSharedPointer<TreeItem>> getAllChilds() const override;

  uint childCount(bool recursive = false) const override;

  QSharedPointer<TreeItem> child(uint row) const override;

  QWeakPointer<TreeItem> parent() const override;

  void setParent(QWeakPointer<TreeItem> p);

  void setWeakPointer(QWeakPointer<ServerItem>);

  bool isDatabaseListLoaded() const;

  QSharedPointer<Operations> getOperations();

  int row() const override;

 public slots:
  void unload();

 private slots:
  void load();
  void reload();
  void edit();
  void remove();
  void openConsole();

 signals:
  void editActionRequested();
  void deleteActionRequested();

 protected:
  QHash<QString, std::function<void()>> eventHandlers() override;

 private:
  QSharedPointer<Operations> m_operations;
  QList<QSharedPointer<TreeItem>> m_databases;
  QWeakPointer<ServerItem> m_self;
  QWeakPointer<TreeItem> m_parent;
  QModelIndex m_index;
};
}  // namespace ConnectionsTree
