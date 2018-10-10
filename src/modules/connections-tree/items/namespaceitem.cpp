#include "namespaceitem.h"
#include <qredisclient/utils/text.h>
#include <QMenu>
#include <QMessageBox>
#include "connections-tree/model.h"
#include "connections-tree/utils.h"
#include "databaseitem.h"
#include "keyitem.h"

using namespace ConnectionsTree;

NamespaceItem::NamespaceItem(const QByteArray &fullPath,
                             QSharedPointer<Operations> operations,
                             QWeakPointer<TreeItem> parent, Model &model,
                             uint dbIndex)
    : AbstractNamespaceItem(model, parent, operations, dbIndex),
      m_fullPath(fullPath),
      m_removed(false) {
  m_displayName = m_fullPath.mid(
      m_fullPath.lastIndexOf(m_operations->getNamespaceSeparator()) + 1);

  m_eventHandlers.insert("click", [this]() {
    if (m_childItems.size() == 0) {
      lock();
      load();
    } else if (!isExpanded()) {
      setExpanded(true);
      emit m_model.itemChanged(getSelf());
      emit m_model.expandItem(getSelf());
    }
  });

  m_eventHandlers.insert("reload", [this]() {
    lock();

    if (m_childItems.size()) {
      clear();
    }

    load();
  });

  m_eventHandlers.insert("delete",
                         [this]() { m_operations->deleteDbNamespace(*this); });
}

QString NamespaceItem::getDisplayName() const {
  return QString("%1 (%2)")
      .arg(printableString(m_displayName, true))
      .arg(childCount(true));
}

QByteArray NamespaceItem::getName() const { return m_displayName; }

bool NamespaceItem::isEnabled() const { return m_removed == false; }

QByteArray NamespaceItem::getFullPath() const { return m_fullPath; }

void NamespaceItem::setRemoved() {
  m_removed = true;

  clear();

  emit m_model.itemChanged(getSelf());
}

void NamespaceItem::load() {
  QString nsFilter = QString("%1%2*")
                         .arg(QString::fromUtf8(m_fullPath))
                         .arg(m_operations->getNamespaceSeparator());

  m_operations->loadNamespaceItems(
      qSharedPointerDynamicCast<AbstractNamespaceItem>(getSelf()), nsFilter,
      [this](const QString &err) {
        unlock();
        if (!err.isEmpty()) return showLoadingError(err);

        setExpanded(true);
        emit m_model.itemChanged(getSelf());
        emit m_model.expandItem(getSelf());
      },
      m_model.m_expanded);
}
