#pragma once
#include "abstractnamespaceitem.h"
#include "memoryusage.h"

namespace ConnectionsTree {

class NamespaceItem : public QObject,
                      public AbstractNamespaceItem,
                      public MemoryUsage {
  Q_OBJECT

 public:
  NamespaceItem(const QByteArray& fullPath,
                QSharedPointer<Operations> operations,
                QWeakPointer<TreeItem> parent, Model& model, uint dbIndex);

 public:
  NamespaceItem(const QByteArray& fullPath,
                QSharedPointer<Operations> operations,
                QWeakPointer<TreeItem> parent, Model& model, uint dbIndex,
                QRegExp filter);

  QString getDisplayName() const override;

  QByteArray getName() const override;

  QByteArray getFullPath() const override;

  QString type() const override { return "namespace"; }

  bool isEnabled() const override;

  void setRemoved();

  QFuture<qlonglong> getMemoryUsage() override;

 protected:
  void load();

  void sortChilds();

  QHash<QString, std::function<void()>> eventHandlers() override;

 private:
  QByteArray m_fullPath;
  bool m_removed;
  QSharedPointer<AsyncFuture::Combinator> m_combinator;
};
}  // namespace ConnectionsTree
