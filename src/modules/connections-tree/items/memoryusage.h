#pragma once
#include <QFuture>
#include <QSharedPointer>
#include <QString>
#include <functional>

#include "treeitem.h"

namespace ConnectionsTree {
class MemoryUsage {
 public:
  MemoryUsage() : m_usedMemory(0) {}
  virtual ~MemoryUsage() {}

  virtual QFuture<qlonglong> getMemoryUsage(
      QSharedPointer<AsyncFuture::Combinator> combinator) = 0;

  qlonglong usedMemory() const { return m_usedMemory; }

 protected:
  qlonglong m_usedMemory;
  QMutex m_updateUsedMemoryMutex;
};
}  // namespace ConnectionsTree
