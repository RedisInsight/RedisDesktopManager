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
                             uint dbIndex)
    : AbstractNamespaceItem(model, parent, operations, dbIndex),
      m_fullPath(fullPath),
      m_removed(false),
      m_combinator(nullptr) {
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

  m_eventHandlers.insert("analyze_memory_usage", [this]() {
    lock();

    getMemoryUsage();

    m_currentOperation = m_combinator->future();

    AsyncFuture::observe(m_currentOperation)
        .subscribe(
            [this]() {
              qDebug() << "Unlocking on success";
              unlock();
            },
            [this]() {
              qDebug() << "Trying to cancel memory usage analysis ";
              m_operations->resetConnection();
              unlock();
            });
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
  QString title = QString("%1 (%2)")
                      .arg(printableString(m_displayName, true))
                      .arg(childCount(true));

  if (m_usedMemory > 0) {
    title.append(QString(" <b>[%1]</b>").arg(humanReadableSize(m_usedMemory)));
  }

  return title;
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

QFuture<qlonglong> NamespaceItem::getMemoryUsage() {
  auto d = QSharedPointer<AsyncFuture::Deferred<qlonglong>>(
      new AsyncFuture::Deferred<qlonglong>());

  if (!m_combinator)
    m_combinator = QSharedPointer<AsyncFuture::Combinator>(
        new AsyncFuture::Combinator(AsyncFuture::AllSettled));

  m_combinator->subscribe([d, this]() { return d->complete(m_usedMemory); });

  for (QSharedPointer<TreeItem> child : m_childItems) {
    if (m_combinator->future().isCanceled()) break;

    if (!child) continue;

    auto memoryItem = child.dynamicCast<MemoryUsage>();

    if (!memoryItem) continue;

    auto future = memoryItem->getMemoryUsage();

    AsyncFuture::observe(future).subscribe([this](qlonglong result) {
      QMutexLocker locker(&m_updateUsedMemoryMutex);
      Q_UNUSED(locker);

      m_usedMemory += result;
      emit m_model.itemChanged(getSelf());
    });

    *m_combinator << future;
  }

  return d->future();
}
