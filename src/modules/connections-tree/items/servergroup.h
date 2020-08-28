#pragma once
#include <QList>
#include <QObject>
#include "connections-tree/operations.h"
#include "sortabletreeitem.h"

namespace ConnectionsTree {

class Model;

class ServerGroup : public QObject, public SortableTreeItem {
    Q_OBJECT
 public:
  ServerGroup(const QString &name, Model& m);

  QString getDisplayName() const override;

  QString type() const override { return "server_group"; }

  QList<QSharedPointer<TreeItem>> getAllChilds() const override;

  uint childCount(bool recursive = false) const override;

  QSharedPointer<TreeItem> child(uint row) const override;

  QSharedPointer<TreeItem> takeChild(uint row);

  void insertChildAt(uint row, QSharedPointer<TreeItem> srv);

  void removeChild(QSharedPointer<TreeItem> srv);

  void setName(const QString &name);

  void addServer(QSharedPointer<TreeItem> s);

  bool isExpanded() const override;

  void unload() override;

 signals:
  void editActionRequested();
  void deleteActionRequested();

 protected:
  QHash<QString, std::function<void()>> eventHandlers() override;

 private:
  QString m_name;

  QList<QSharedPointer<TreeItem>> m_servers;
};
}  // namespace ConnectionsTree
