#include "treeitem.h"
#include "connections-tree/model.h"

ConnectionsTree::TreeItem::TreeItem(Model &m)
    : m_model(m), m_locked(false), m_expanded(false) {}

QVariantMap ConnectionsTree::TreeItem::metadata() const {
  QVariantMap meta;
  meta["name"] = getDisplayName();
  meta["full_name"] = getName();
  meta["type"] = type();
  meta["locked"] = isLocked();
  meta["state"] = isEnabled();
  return meta;
}

int ConnectionsTree::TreeItem::row() const {
  if (!parent()) return 0;

  auto p = parent().toStrongRef();

  for (uint index = 0; index < p->childCount(); ++index) {
    if (p->child(index).data() == this) return index;
  }

  return 0;
}

QWeakPointer<ConnectionsTree::TreeItem> ConnectionsTree::TreeItem::getSelf() {
  if (m_selfPtr) return m_selfPtr;

  if (!parent()) return QWeakPointer<TreeItem>();

  QSharedPointer<TreeItem> p = parent().toStrongRef();

  if (!p) return QWeakPointer<TreeItem>();

  m_selfPtr = p->child(row()).toWeakRef();

  return m_selfPtr;
}

ConnectionsTree::Model &ConnectionsTree::TreeItem::model() { return m_model; }

void ConnectionsTree::TreeItem::lock() {
  m_locked = true;
  if (getSelf())
    emit m_model.itemChanged(getSelf());
}

void ConnectionsTree::TreeItem::unlock() {
  m_locked = false;
  if (getSelf())
    emit m_model.itemChanged(getSelf());
}

QHash<QString, std::function<bool()>>
ConnectionsTree::TreeItem::eventHandlers() {
  QHash<QString, std::function<bool()>> events;
  events["cancel"] = [this]() { cancelCurrentOperation(); return true; };
  return events;
}

void ConnectionsTree::TreeItem::handleEvent(QString event) {
  if (!eventHandlers().contains(event)) return;

  if (isLocked() && event != "cancel") {
    qDebug() << "Item is locked. Ignore event: " << event;

    emit m_model.itemChanged(getSelf());
    return;
  }

  auto handler = eventHandlers()[event];

  try {
    lock();
    bool shouldUnlock = handler();

    if (shouldUnlock) {
      unlock();
    }
  } catch (...) {
    qWarning() << "Error on event processing: " << event;
    unlock();
  }
}

void ConnectionsTree::TreeItem::cancelCurrentOperation() {
  m_currentOperation.cancel();
}
