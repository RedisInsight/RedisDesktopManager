#pragma once
#include <QObject>
#include <QSharedPointer>

namespace ConnectionsTree {
class ServerGroup;
}

class ConnectionGroup {
  Q_GADGET

  Q_PROPERTY(QString name READ name WRITE setName)

 public:
  ConnectionGroup();

  ConnectionGroup(QSharedPointer<ConnectionsTree::ServerGroup> g);

  QString name() const;

  void setName(const QString& n);

  QSharedPointer<ConnectionsTree::ServerGroup> serverGroup() const;

 private:
  QSharedPointer<ConnectionsTree::ServerGroup> m_group;
};

Q_DECLARE_METATYPE(ConnectionGroup)
