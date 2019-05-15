#include "abstractnamespaceitem.h"
#include "connections-tree/model.h"
#include "connections-tree/operations.h"
#include "keyitem.h"
#include "namespaceitem.h"

#include <QMessageBox>

using namespace ConnectionsTree;

AbstractNamespaceItem::AbstractNamespaceItem(
    Model& model, QWeakPointer<TreeItem> parent,
    QSharedPointer<Operations> operations, uint dbIndex, QRegExp filter)
    : TreeItem(model),
      m_parent(parent),
      m_operations(operations),
      m_filter(filter.isEmpty() ? QRegExp(operations->defaultFilter())
                                : filter),
      m_expanded(false),
      m_dbIndex(dbIndex),
      m_combinator(nullptr) {}

QList<QSharedPointer<TreeItem>> AbstractNamespaceItem::getAllChilds() const {
  return m_childItems;
}

QSharedPointer<TreeItem> AbstractNamespaceItem::child(uint row) const {
  if (row < m_childItems.size()) return m_childItems.at(row);

  return QSharedPointer<TreeItem>();
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
  m_usedMemory = 0;
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

void AbstractNamespaceItem::cancelCurrentOperation() {
  if (m_combinator) {
    m_combinator->future().cancel();
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
  emit m_model.beforeItemLayoutChanged(getSelf());
  std::sort(m_childItems.begin(), m_childItems.end(), compareChilds);
  emit m_model.itemLayoutChanged(getSelf());
  emit m_model.itemChanged(getSelf());
}

QFuture<qlonglong> AbstractNamespaceItem::getMemoryUsage(
    QSharedPointer<AsyncFuture::Combinator> combinator) {
  m_usedMemory = 0;

  lock();

  auto d = QSharedPointer<AsyncFuture::Deferred<qlonglong>>(
      new AsyncFuture::Deferred<qlonglong>());

  auto childsCombinator = QSharedPointer<AsyncFuture::Combinator>(
      new AsyncFuture::Combinator(AsyncFuture::FailFast));

  combinator->onCanceled([this]() { unlock(); });

  for (QSharedPointer<TreeItem> child : m_childItems) {
    if (combinator->future().isCanceled() || d->future().isCanceled()) {
      break;
    }

    if (!child) continue;

    auto memoryItem = child.dynamicCast<MemoryUsage>();

    if (!memoryItem) continue;

    auto future = memoryItem->getMemoryUsage(combinator);

    AsyncFuture::observe(future).subscribe([this](qlonglong result) {
      QMutexLocker locker(&m_updateUsedMemoryMutex);
      Q_UNUSED(locker);

      m_usedMemory += result;
      emit m_model.itemChanged(getSelf());
    });

    combinator->combine(future);
    childsCombinator->combine(future);
  }

  childsCombinator->subscribe(
      [this, d]() {
        d->complete(m_usedMemory);
        unlock();
      },
      [this, d]() {
        d->cancel();
        unlock();
      });

  return d->future();
}

QHash<QString, std::function<void()>> AbstractNamespaceItem::eventHandlers() {
  auto events = TreeItem::eventHandlers();

  events.insert("analyze_memory_usage", [this]() {
    if (m_usedMemory > 0) return;

    lock();

    auto future = m_operations->connectionSupportsMemoryOperations();

    AsyncFuture::observe(future).subscribe([this](bool isSupported) {
      if (!isSupported) {
        emit m_model.error(QCoreApplication::translate(
            "RDM",
            "Your redis-server doesn't support <a "
            "href='https://redis.io/commands/memory-usage'><b>MEMORY</b></a> "
            "commands."));
        unlock();
        return;
      }

      m_combinator = QSharedPointer<AsyncFuture::Combinator>(
          new AsyncFuture::Combinator(AsyncFuture::FailFast));

      getMemoryUsage(m_combinator);

      m_combinator->subscribe(
          [this]() {
            sortChilds();
            unlock();
          },
          [this]() {
            m_operations->resetConnection();
            unlock();
          });
    });
  });

  return events;
}
