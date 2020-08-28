#include "connectiongroup.h"

#include "connections-tree/items/servergroup.h"

ConnectionGroup::ConnectionGroup(QSharedPointer<ConnectionsTree::ServerGroup> g)
    : m_group(g) {}

ConnectionGroup::ConnectionGroup() : m_group(nullptr) {}

QString ConnectionGroup::name() const {
  if (!m_group) return QString();

  return m_group->getDisplayName();
}

void ConnectionGroup::setName(const QString &n) {
  if (m_group) m_group->setName(n);
}

QSharedPointer<ConnectionsTree::ServerGroup> ConnectionGroup::serverGroup() const {
  return m_group;
}
