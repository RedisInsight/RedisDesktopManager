#include "abstractnamespaceitem.h"
#include "connections-tree/model.h"
#include "connections-tree/operations.h"
#include "keyitem.h"
#include "namespaceitem.h"

#include <QMessageBox>

using namespace ConnectionsTree;

AbstractNamespaceItem::AbstractNamespaceItem(
    Model& model, QWeakPointer<TreeItem> parent,
    QSharedPointer<Operations> operations, uint dbIndex)
    : TreeItem(model),
      m_parent(parent),
      m_operations(operations),
      m_filter(operations->defaultFilter()),
      m_expanded(false),
      m_dbIndex(dbIndex) {}

QList<QSharedPointer<TreeItem> > AbstractNamespaceItem::getAllChilds() const {
  return m_childItems;
}

QSharedPointer<TreeItem> AbstractNamespaceItem::child(uint row) const {
  if (row < m_childItems.size()) return m_childItems.at(row);

  return QSharedPointer<TreeItem>();
}

QRegExp AbstractNamespaceItem::getFilter() const {
  QSharedPointer<AbstractNamespaceItem> parentItem = qSharedPointerDynamicCast<AbstractNamespaceItem>(m_parent);

  if ((!m_filter.isEmpty() && m_filter.pattern().compare(m_operations->defaultFilter()) != 0) || parentItem == 0)  {
    return m_filter;
  }

  return parentItem->getFilter();
}

QWeakPointer<TreeItem> AbstractNamespaceItem::parent() const {
  return m_parent;
}

uint AbstractNamespaceItem::childCount(bool recursive) const {
  if (!recursive) return m_childItems.size();

  uint count = 0;
  for (auto item : m_childItems) {
    if (item->supportChildItems()) {
      count += item->childCount(true);
    } else {
      count += 1;
    }
  }
  return count;
}

void AbstractNamespaceItem::clear() {
  emit m_model.itemChildsUnloaded(getSelf());
  m_childItems.clear();
  m_childNamespaces.clear();
}

void AbstractNamespaceItem::notifyModel() {
  qDebug() << "Notify model about loaded childs";
  emit m_model.itemChildsLoaded(getSelf());
  emit m_model.itemChanged(getSelf());
}

void AbstractNamespaceItem::showLoadingError(const QString& err) {
  emit m_model.itemChanged(getSelf());
  emit m_model.error(err);
}
