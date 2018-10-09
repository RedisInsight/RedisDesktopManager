#pragma once
#include <QList>
#include <QObject>
#include "connections-tree/operations.h"
#include "treeitem.h"

namespace ConnectionsTree {

class Model;

class ServerItem : public QObject, public TreeItem {
  Q_OBJECT
 public:
  ServerItem(const QString &name, QSharedPointer<Operations> operations,
             Model &model);

  ~ServerItem();

  QString getDisplayName() const override;

  QString getType() const override { return "server"; }

  QVariantMap metadata() const;

  QList<QSharedPointer<TreeItem>> getAllChilds() const override;

  uint childCount(bool recursive = false) const override;

  QSharedPointer<TreeItem> child(uint row) const override;

  QWeakPointer<TreeItem> parent() const override;

  int row() const override;

  void setRow(int r);

  bool isEnabled() const override;

  void setName(const QString &name);

  void setWeakPointer(QWeakPointer<ServerItem>);

  bool isDatabaseListLoaded() const;

  QSharedPointer<Operations> getOperations();

 private slots:
  void load();
  void unload();
  void reload();
  void edit();
  void remove();
  void openConsole();

 signals:
  void editActionRequested();
  void deleteActionRequested();

 private:
  QString m_name;
  int m_row;
  QSharedPointer<Operations> m_operations;
  QList<QSharedPointer<TreeItem>> m_databases;
  QWeakPointer<ServerItem> m_self;
  QModelIndex m_index;
};
}  // namespace ConnectionsTree
