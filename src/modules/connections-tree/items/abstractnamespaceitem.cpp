#include "abstractnamespaceitem.h"

#include <QApplication>
#include <QMessageBox>
#include <QThread>
#include <algorithm>
#include <functional>

#include "connections-tree/keysrendering.h"
#include "connections-tree/model.h"
#include "connections-tree/operations.h"
#include "keyitem.h"
#include "loadmoreitem.h"
#include "namespaceitem.h"

using namespace ConnectionsTree;

AbstractNamespaceItem::AbstractNamespaceItem(
    Model &model, QWeakPointer<TreeItem> parent,
    QSharedPointer<Operations> operations, uint dbIndex, QRegExp filter)
    : TreeItem(model),
      m_parent(parent),
      m_operations(operations),
      m_filter(filter.isEmpty() ? QRegExp(operations->defaultFilter())
                                : filter),      
      m_dbIndex(dbIndex),
      m_runningOperation(nullptr) {
  QSettings settings;
  m_showNsOnTop = settings
                      .value("app/showNamespacesOnTop",
#if defined(Q_OS_WINDOWS)
                             true
#else
                             false
#endif
                             )
                      .toBool();
}

QList<QSharedPointer<TreeItem>> AbstractNamespaceItem::getAllChilds() const {
  return m_childItems;
}

QList<QSharedPointer<AbstractNamespaceItem>>
AbstractNamespaceItem::getAllChildNamespaces() const {
  return m_childNamespaces.values();
}

QSharedPointer<TreeItem> AbstractNamespaceItem::child(uint row) {
  if (row < m_childItems.size()) return m_childItems.at(row);

  return QSharedPointer<TreeItem>();
}

QWeakPointer<TreeItem> AbstractNamespaceItem::parent() const {
  return m_parent;
}

void AbstractNamespaceItem::append(QSharedPointer<TreeItem> item, bool notifyModel) {
  if (notifyModel)
    m_model.beforeChildLoadedAtPos(getSelf(), m_childItems.size());
  m_childItems.append(item);
  if (notifyModel) m_model.childLoaded(getSelf());
}

bool compareTreeItemsByName(QSharedPointer<TreeItem> first,
                            QSharedPointer<TreeItem> second) {
  return first->getDisplayName() < second->getDisplayName();
}

bool compareTreeItemsByNameAndNsOnTop(QSharedPointer<TreeItem> first,
                                      QSharedPointer<TreeItem> second) {
  if (first->type() != second->type()) return first->type() > second->type();

  return first->getDisplayName() < second->getDisplayName();
}

void AbstractNamespaceItem::insertChild(QSharedPointer<TreeItem> item) {
  auto pos = std::upper_bound(m_childItems.begin(), m_childItems.end(), item,
                              m_showNsOnTop ? compareTreeItemsByNameAndNsOnTop
                                            : compareTreeItemsByName);

  int index = std::distance(m_childItems.begin(), pos);

  m_model.beforeChildLoadedAtPos(getSelf(), index);
  m_childItems.insert(pos, item);
  m_model.childLoaded(getSelf());
}

void AbstractNamespaceItem::appendKeyToIndex(QSharedPointer<KeyItem> key) {
  if (!key) return;

  m_keysIndex.insert(key->getFullPath(), key.toWeakRef());
}

void AbstractNamespaceItem::removeNamespacedKeysFromIndex(QByteArray nsPrefix) {
  auto keys = m_keysIndex.keys();

  for (auto const &fullPath : qAsConst(keys)) {
    if (fullPath.startsWith(nsPrefix)) {
      m_keysIndex.remove(fullPath);
    }
  }
}

QHash<QByteArray, QWeakPointer<KeyItem>> AbstractNamespaceItem::getKeysIndex()
    const {
  return m_keysIndex;
}

QSharedPointer<TreeItem> resolveItemToRemove(QSharedPointer<TreeItem> item) {
  if (!item) return item;

  auto parent = item->parent().toStrongRef();

  if (!parent || parent->type() == "database") return item;

  if (parent->type() == "namespace" && parent->getAllChilds().empty())
    return resolveItemToRemove(parent);

  return item;
}

void AbstractNamespaceItem::removeObsoleteKeys(
    QList<QWeakPointer<KeyItem>> keys) {
  for (const auto &obsoleteKey : keys) {
    auto key = obsoleteKey.toStrongRef();

    if (!key) continue;

    m_keysIndex.remove(key->getFullPath());

    auto parent = key->parent().toStrongRef();

    if (!parent) continue;

    QSharedPointer<TreeItem> itemToRemoveFromModel = key;

    if (parent->type() == "namespace" && parent->getAllChilds().size() == 1) {
      itemToRemoveFromModel = resolveItemToRemove(parent);
    }

    if (!itemToRemoveFromModel) continue;

    int row = itemToRemoveFromModel->row();

    m_model.beforeItemChildRemoved(itemToRemoveFromModel->parent(), row);

    auto parentHoldsItemToRemove =
        itemToRemoveFromModel->parent().toStrongRef();

    if (!parentHoldsItemToRemove) continue;

    parentHoldsItemToRemove->removeChild(row);

    m_model.itemChildRemoved(itemToRemoveFromModel);
  }
}

void AbstractNamespaceItem::removeChild(int index)
{
    bool validIndex = 0 < index && index < m_childItems.size();
    if (!validIndex)
        return;

    m_childItems.removeAt(index);
}

void AbstractNamespaceItem::appendRawKey(const QByteArray &k) {
  m_rawChildKeys.append(k);
}

void AbstractNamespaceItem::appendNamespace(
    QSharedPointer<AbstractNamespaceItem> item) {
  m_childNamespaces[item->getName()] = item;
  insertChild(item.staticCast<TreeItem>());
}

uint AbstractNamespaceItem::childCount(bool recursive) const {
  uint count = 0;

  if (!recursive) {
    count += m_childItems.size();
    return count;
  }

  for (const auto &item : m_childItems) {
    if (item->supportChildItems()) {
      count += item->childCount(true);
    } else {
      count += 1;
    }
  }
  return count;
}

uint AbstractNamespaceItem::keysCount() const {
  uint count = m_rawChildKeys.size();

  for (const auto &item : m_childItems) {
    if (item->supportChildItems()) {
      auto ns = item.dynamicCast<AbstractNamespaceItem>();
      if (ns) {
        count += ns->keysCount();
      }
    } else if (item->type() == "key") {
      count += 1;
    }
  }
  return count;
}

uint AbstractNamespaceItem::keysRenderingLimit() const {
  QSettings appSettings;
  return appSettings.value("app/treeItemMaxChilds", 1000).toUInt();
}

bool AbstractNamespaceItem::keysShortNameRendering() const {
  QSettings appSettings;
  return appSettings.value("app/namespacedKeysShortName", true).toBool();
}

void AbstractNamespaceItem::clear() {
  clearLoader();

  bool notifyModel = false;

  if (m_childItems.size() > 0) {
    notifyModel = true;
    m_model.beforeItemChildsUnloaded(getSelf());
  }

  m_childItems.clear();
  m_childNamespaces.clear();
  m_rawChildKeys.clear();
  m_usedMemory = 0;

  if (type() == "database") {
    m_keysIndex.clear();
  } else {
    auto selfRef = getSelf().toStrongRef().dynamicCast<AbstractNamespaceItem>();
    if (selfRef) {
      auto root = resolveRootItem(selfRef);

      if (root) {
        root->removeNamespacedKeysFromIndex(getFullPath());
      }
    }
  }

  if (notifyModel) {
    m_model.itemChildRemoved(getSelf());
  }
}

void AbstractNamespaceItem::clearLoader() {
  if (m_childItems.empty()) {
    return;
  }

  auto lastItem = m_childItems.last();

  if (!lastItem || lastItem->type() != "loader") return;

  m_model.beforeItemChildRemoved(getSelf(), m_childItems.size() - 1);
  m_childItems.removeLast();
  m_model.itemChildRemoved(lastItem.toWeakRef());
}

void AbstractNamespaceItem::showLoadingError(const QString &err) {
  emit m_model.itemChanged(getSelf());
  emit m_model.error(err);
}

void AbstractNamespaceItem::cancelCurrentOperation() {
  if (m_runningOperation) {
    m_runningOperation->future().cancel();
    m_operations->resetConnection();
    unlock();
  }
}

bool compareChilds(QSharedPointer<TreeItem> first,
                   QSharedPointer<TreeItem> second) {
  auto firstMemoryItem = first.dynamicCast<MemoryUsage>();
  auto secondMemoryItem = second.dynamicCast<MemoryUsage>();

  if (!firstMemoryItem)
    qDebug() << "Invalid tree item:" << first->getDisplayName();
  if (!secondMemoryItem)
    qDebug() << "Invalid tree item:" << second->getDisplayName();

  return (firstMemoryItem ? firstMemoryItem->usedMemory() : 0) >
         (secondMemoryItem ? secondMemoryItem->usedMemory() : 0);
}

void AbstractNamespaceItem::sortChilds() {
  m_model.beforeItemLayoutChanged(getSelf());
  std::sort(m_childItems.begin(), m_childItems.end(), compareChilds);
  m_model.itemLayoutChanged(getSelf());
  emit m_model.itemChanged(getSelf());
}

void AbstractNamespaceItem::renderRawKeys(
    const RedisClient::Connection::RawKeysList &keylist, QRegExp filter,
    QSharedPointer<RenderRawKeysCallback> callback, bool appendNewItems,
    bool checkPreRenderedItems, int maxChildItems) {
  if (!m_operations) {
    return;
  }

  uint renderingLimit =
      qMax(static_cast<uint>(m_childItems.size()), keysRenderingLimit());

  if (maxChildItems > 0) {
    renderingLimit = static_cast<uint>(maxChildItems);
  } 

  auto settings = ConnectionsTree::KeysTreeRenderer::RenderingSettigns{
      filter,         m_operations->getNamespaceSeparator(),
      getDbIndex(),   renderingLimit,
      appendNewItems, checkPreRenderedItems, keysShortNameRendering()};

  auto future = QtConcurrent::run(
      [](QList<QByteArray> keylist) {
        std::sort(keylist.begin(), keylist.end());
        return keylist;
      },
      keylist);

  auto selfWPtr = getSelf();

  AsyncFuture::observe(future).subscribe([selfWPtr, this, settings, callback, future]() {

    auto self = selfWPtr.toStrongRef();

    if (!self)
      return;

    ConnectionsTree::KeysTreeRenderer::renderKeys(
        m_operations, future.result(),
        qSharedPointerDynamicCast<AbstractNamespaceItem>(self), settings,
        m_model.expandedNamespaces);
    if (callback)
      callback->call();
  });
}

void AbstractNamespaceItem::ensureLoaderIsCreated() {
  if (m_rawChildKeys.empty() || m_childItems.empty()) {
    return;
  }

  auto lastItem = m_childItems.last();

  if (lastItem->type() == "loader") return;

  m_model.beforeChildLoaded(getSelf(), 1);
  m_childItems.append(
      QSharedPointer<TreeItem>(new LoadMoreItem(getSelf(), m_model)));
  m_model.childLoaded(getSelf());
}

QHash<QString, std::function<bool ()> > AbstractNamespaceItem::eventHandlers() {
  auto events = TreeItem::eventHandlers();

  events.insert("analyze_memory_usage", [this]() {
    if (m_usedMemory > 0) return true;

    auto future = m_operations->connectionSupportsMemoryOperations();

    auto selfWPtr = getSelf();

    AsyncFuture::observe(future).subscribe([selfWPtr, this](bool isSupported) {
      auto self = selfWPtr.toStrongRef();

      if (!self)
        return;

      if (!isSupported) {
        emit m_model.error(QCoreApplication::translate(
            "RESP",
            "Your redis-server doesn't support <a "
            "href='https://redis.io/commands/memory-usage'><b>MEMORY</b></a> "
            "commands."));
        unlock();
        return;
      }

      getMemoryUsage([selfWPtr, this](qlonglong) {
        QTimer::singleShot(0, this, [this]() {
          sortChilds();
          unlock();
          m_runningOperation.clear();
        });
      });
    });
    return false;
  });

  return events;
}

void AbstractNamespaceItem::getMemoryUsage(
    std::function<void(qlonglong)> callback) {
  m_usedMemory = 0;

  m_runningOperation = QSharedPointer<AsyncFuture::Deferred<qlonglong>>(
      new AsyncFuture::Deferred<qlonglong>());

  QtConcurrent::run(this, &AbstractNamespaceItem::calculateUsedMemory,
                    m_runningOperation, callback);

  return;
}

void AbstractNamespaceItem::fetchMore() {
  if (m_rawChildKeys.empty()) {
    return;
  }

  clearLoader();

  int childsCount = m_childItems.size();
  auto rawKeys = m_rawChildKeys;

  emit m_model.itemChanged(getSelf());

  m_rawChildKeys.clear();

  auto callback = QSharedPointer<RenderRawKeysCallback>(
      new RenderRawKeysCallback(getSelf(), [this]() {
        ensureLoaderIsCreated();

        unlock();
      }));

  return renderRawKeys(
      rawKeys, m_filter, callback,
      true, false, static_cast<uint>(childsCount) + keysRenderingLimit());
}

void AbstractNamespaceItem::calculateUsedMemory(
    QSharedPointer<AsyncFuture::Deferred<qlonglong>> parentDeffered,
    std::function<void(qlonglong)> callback) {
  if (parentDeffered && parentDeffered->future().isCanceled()) {
    return;
  }

  if (m_rawChildKeys.size() > 0) {
    auto resultCallback = QSharedPointer<Operations::GetUsedMemoryCallback>(
        new Operations::GetUsedMemoryCallback(
            getSelf(), [this, callback](qlonglong result) {
              m_usedMemory = result;
              emit m_model.itemChanged(getSelf());

              if (m_childItems.empty()) callback(result);
            }));

    auto progressCallback = QSharedPointer<Operations::GetUsedMemoryCallback>(
        new Operations::GetUsedMemoryCallback(
            getSelf(), [this](qlonglong progress) {
              m_usedMemory = progress;
              emit m_model.itemChanged(getSelf());
            }));

    operations()->getUsedMemory(m_rawChildKeys, m_dbIndex, resultCallback,
                                progressCallback);
  }

  auto resultsRemaining = QSharedPointer<qlonglong>(new qlonglong(0));

  auto updateUsedMemoryValue = [this, resultsRemaining,
                                callback](qlonglong result) {
    if (!resultsRemaining) return;

    QMutexLocker locker(&m_updateUsedMemoryMutex);
    Q_UNUSED(locker);
    m_usedMemory += result;
    emit m_model.itemChanged(getSelf());

    (*resultsRemaining)--;

    if (*resultsRemaining <= 0) {
      callback(m_usedMemory);
    }
  };

  (*resultsRemaining) += m_childNamespaces.size();

  for (auto childNs : qAsConst(m_childNamespaces)) {
    if (parentDeffered->future().isCanceled()) {
      return;
    }
    childNs->calculateUsedMemory(parentDeffered, updateUsedMemoryValue);
  }

  for (const QSharedPointer<TreeItem> &child : qAsConst(m_childItems)) {
    if (parentDeffered->future().isCanceled()) {
      return;
    }

    if (!child || child->type() != "key") continue;

    auto memoryItem = child.dynamicCast<MemoryUsage>();

    if (!memoryItem) continue;

    QMutexLocker locker(&m_updateUsedMemoryMutex);
    (*resultsRemaining)++;
    memoryItem->getMemoryUsage(updateUsedMemoryValue);
  }
}

void AbstractNamespaceItem::restoreOpenedNamespaces(QSharedPointer<AbstractNamespaceItem> ns) {
  if (ns->type() == "namespace" && !ns->isExpanded()) return;

  if (ns->isExpanded())
      m_model.expandItem(ns.staticCast<TreeItem>().toWeakRef());

  auto childs = ns->getAllChildNamespaces();

  for (auto childNs : childs) {
    restoreOpenedNamespaces(childNs);
  }
}
