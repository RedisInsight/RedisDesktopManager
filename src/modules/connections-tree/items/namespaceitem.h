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

  QString getDisplayName() const override;

  QByteArray getName() const override;

  QByteArray getFullPath() const;

  QString getType() const override { return "namespace"; }

  bool isEnabled() const override;

  void setRemoved();

  QFuture<qlonglong> getMemoryUsage() override;

 protected:
  void load();

 private:
  QByteArray m_fullPath;
  QByteArray m_displayName;
  bool m_removed;
  QSharedPointer<AsyncFuture::Combinator> m_combinator;
};
}  // namespace ConnectionsTree
