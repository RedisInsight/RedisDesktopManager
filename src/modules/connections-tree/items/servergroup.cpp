#include "servergroup.h"

#include "connections-tree/model.h"
#include "connections-tree/items/serveritem.h"
#include "connections-tree/utils.h"

using namespace ConnectionsTree;

ServerGroup::ServerGroup(const QString& name, Model& model)
    : SortableTreeItem(model), m_name(name) {}

QString ServerGroup::getDisplayName() const { return m_name; }

QList<QSharedPointer<TreeItem> > ServerGroup::getAllChilds() const {
  return m_servers;
}

uint ServerGroup::childCount(bool) const {
  return static_cast<uint>(m_servers.size());
}

QSharedPointer<TreeItem> ServerGroup::child(uint row) const {
  if (row < m_servers.size()) {
    return m_servers.at(row);
  }

  return QSharedPointer<TreeItem>();
}

QSharedPointer<TreeItem> ServerGroup::takeChild(uint row)
{
   if (!child(row))
       return QSharedPointer<TreeItem>();

   return m_servers.takeAt(row);
}

void ServerGroup::insertChildAt(uint row, QSharedPointer<TreeItem> srv)
{
    if (!srv)
        return;

    m_servers.insert(row, srv);
}

void ServerGroup::removeChild(QSharedPointer<TreeItem> srv)
{
    m_servers.removeAll(srv);
}

QHash<QString, std::function<void()> > ServerGroup::eventHandlers() {
  auto events = TreeItem::eventHandlers();

  events.insert("edit", [this]() { emit editActionRequested(); });

  events.insert("delete", [this]() {
    confirmAction(nullptr,
                  QCoreApplication::translate(
                      "RDM", "Do you really want to delete group <b>with all connections</b>?"),
                  [this]() { emit deleteActionRequested(); });
  });

  return events;
}

void ServerGroup::setName(const QString& name) { m_name = name; }

void ServerGroup::addServer(QSharedPointer<TreeItem> s) {
    auto srv = s.dynamicCast<ServerItem>();
    m_servers.append(s);
}

bool ServerGroup::isExpanded() const { return true; }

void ServerGroup::unload() {
  for (auto child : m_servers) {
    auto item = child.dynamicCast<SortableTreeItem>();

    if (!item) continue;

    item->unload();
  }
}
