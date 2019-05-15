#include "namespaceitem.h"
#include <qredisclient/utils/text.h>
#include <QMenu>
#include <QMessageBox>
#include "app/apputils.h"
#include "connections-tree/model.h"
#include "connections-tree/utils.h"
#include "databaseitem.h"
#include "keyitem.h"

using namespace ConnectionsTree;

NamespaceItem::NamespaceItem(const QByteArray &fullPath,
                             QSharedPointer<Operations> operations,
                             QWeakPointer<TreeItem> parent, Model &model,
                             uint dbIndex, QRegExp filter)
    : AbstractNamespaceItem(model, parent, operations, dbIndex, filter),
      m_fullPath(fullPath),
      m_removed(false) {}

QString NamespaceItem::getDisplayName() const {
  QString title = QString("%1 (%2)")
                      .arg(printableString(getName(), true))
                      .arg(childCount(true));

  if (m_usedMemory > 0) {
    title.append(QString(" <b>[%1]</b>").arg(humanReadableSize(m_usedMemory)));
  }

  return title;
}

QByteArray NamespaceItem::getName() const {
  return m_fullPath.mid(
      m_fullPath.lastIndexOf(m_operations->getNamespaceSeparator()) + 1);
}

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

void NamespaceItem::reload() {
  lock();

  if (m_childItems.size()) {
    clear();
  }

  load();
}

QHash<QString, std::function<void()>> NamespaceItem::eventHandlers() {
  auto events = AbstractNamespaceItem::eventHandlers();

  events.insert("click", [this]() {
    if (m_childItems.size() == 0) {
      lock();
      load();
    } else if (!isExpanded()) {
      setExpanded(true);
      emit m_model.itemChanged(getSelf());
      emit m_model.expandItem(getSelf());
    }
  });

  events.insert("add_key", [this]() {
    m_operations->openNewKeyDialog(
        m_dbIndex,
        [this]() {
          confirmAction(nullptr,
                        QCoreApplication::translate(
                            "RDM",
                            "Key was added. Do you want to reload keys in "
                            "selected namespace?"),
                        [this]() { reload(); },
                        QCoreApplication::translate("RDM", "Key was added"));
        },
        QString("%1%2")
            .arg(QString::fromUtf8(getFullPath()))
            .arg(m_operations->getNamespaceSeparator()));
  });

  events.insert("reload", [this]() { reload(); });

  events.insert("delete", [this]() { m_operations->deleteDbNamespace(*this); });

  return events;
}
