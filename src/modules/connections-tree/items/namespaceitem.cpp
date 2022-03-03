#include "namespaceitem.h"
#include <qredisclient/utils/text.h>
#include <QMenu>
#include <QMessageBox>
#include "app/apputils.h"
#include "connections-tree/model.h"
#include "connections-tree/utils.h"
#include "connections-tree/keysrendering.h"
#include "databaseitem.h"
#include "keyitem.h"
#include "loadmoreitem.h"

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
                      .arg(keysCount());

  if (m_usedMemory > 0) {
    title.append(QString(" <b>[%1]</b>").arg(humanReadableSize(m_usedMemory)));
  }

  return title;
}

QByteArray NamespaceItem::getName() const {
  qsizetype pos = m_fullPath.lastIndexOf(m_operations->getNamespaceSeparator());

  if (pos >= 0) {
      return m_fullPath.mid(pos + m_operations->getNamespaceSeparator().size());
  } else {
      return m_fullPath;
  }
}

bool NamespaceItem::isEnabled() const { return m_removed == false; }

QByteArray NamespaceItem::getFullPath() const { return m_fullPath; }

void NamespaceItem::setRemoved() {
  m_removed = true;

  clear();

  emit m_model.itemChanged(getSelf());
}

QVariantMap NamespaceItem::metadata() const {
  QVariantMap metadata = TreeItem::metadata();
  metadata["full_path"] = getFullPath();
  return metadata;
}

void NamespaceItem::load() {
  auto onKeysRendered = QSharedPointer<RenderRawKeysCallback>(
      new RenderRawKeysCallback(getSelf(), [this]() {
        ensureLoaderIsCreated();

        unlock();
        setExpanded(true);
        emit m_model.itemChanged(getSelf());
        m_model.expandItem(getSelf());
      }));

  if (m_rawChildKeys.size() > 0) {
    auto rawKeys = m_rawChildKeys;
    m_rawChildKeys.clear();

    return renderRawKeys(rawKeys, m_filter, onKeysRendered, true, false);
  }

  QString nsFilter = QString("%1%2*")
                         .arg(QString::fromUtf8(m_fullPath))
                         .arg(m_operations->getNamespaceSeparator());

  if (!m_filter.isEmpty()) {
    if (m_filter.pattern().startsWith(nsFilter.chopped(1))) {
      nsFilter = m_filter.pattern();
    } else {
      nsFilter = QString("%1%2%3")
                     .arg(QString::fromUtf8(m_fullPath))
                     .arg(m_operations->getNamespaceSeparator())
                     .arg(m_filter.pattern());
    }
  }

  auto callback = QSharedPointer<Operations::LoadNamespaceItemsCallback>(
      new Operations::LoadNamespaceItemsCallback(
          getSelf(), [this, nsFilter, onKeysRendered](
                         const RedisClient::Connection::RawKeysList &keylist,
                         const QString &err) {
            if (!err.isEmpty()) {
              unlock();
              return showLoadingError(err);
            }

            return renderRawKeys(keylist, m_filter, onKeysRendered, true,
                                 false);
          }));

  m_operations->loadNamespaceItems(m_dbIndex, nsFilter, callback);
}

void NamespaceItem::reload() {
  clear();
  load();
}

QHash<QString, std::function<bool()>> NamespaceItem::eventHandlers() {
  auto events = AbstractNamespaceItem::eventHandlers();

  events.insert("click", [this]() {
    if (m_childItems.size() == 0) {
      load();
      return false;
    } else if (!isExpanded()) {
      setExpanded(true);
      emit m_model.itemChanged(getSelf());
      m_model.expandItem(getSelf());      
    }
    return true;
  });

  events.insert("add_key", [this]() {
    auto callback = QSharedPointer<Operations::OpenNewKeyDialogCallback>(
        new Operations::OpenNewKeyDialogCallback(getSelf(), [this]() {
          confirmAction(
              nullptr,
              QCoreApplication::translate(
                  "RESP",
                  "Key was added. Do you want to reload keys in "
                  "selected namespace?"),
              [this]() { reload(); },
              QCoreApplication::translate("RESP", "Key was added"));
        }));
    m_operations->openNewKeyDialog(
        m_dbIndex, callback,
        QString("%1%2")
            .arg(QString::fromUtf8(getFullPath()))
            .arg(m_operations->getNamespaceSeparator()));
    return true;
  });

  events.insert("reload", [this]() { reload(); return false; });

  events.insert("delete", [this]() { m_operations->deleteDbNamespace(*this); return true; });

  return events;
}
