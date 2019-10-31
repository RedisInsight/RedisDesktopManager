#include "treeitem.h"
#include "connections-tree/model.h"

ConnectionsTree::TreeItem::TreeItem(Model &m) : m_model(m), m_locked(false) {}

QVariant ConnectionsTree::TreeItem::metadata(const QString &key) const {
  if (!metadata().contains(key)) return QVariant();

  return metadata()[key];
}

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

QHash<QString, std::function<void()>>
ConnectionsTree::TreeItem::eventHandlers() {
  QHash<QString, std::function<void()>> events;
  events["cancel"] = [this]() { cancelCurrentOperation(); };
  return events;
}

void ConnectionsTree::TreeItem::handleEvent(QString event) {
  if (!eventHandlers().contains(event)) return;

  if (isLocked() && event != "cancel") {
    qDebug() << "Item is locked. Ignore event: " << event;
    emit m_model.itemChanged(getSelf());
    return;
  }

  try {
    eventHandlers()[event]();
  } catch (...) {
    qWarning() << "Error on event processing: " << event;
  }
}

void ConnectionsTree::TreeItem::cancelCurrentOperation() {
  m_currentOperation.cancel();
}
