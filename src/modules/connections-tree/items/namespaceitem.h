#pragma once
#include "abstractnamespaceitem.h"

namespace ConnectionsTree {

class NamespaceItem : public AbstractNamespaceItem {
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

 protected:
  void load();

  void reload();

  QHash<QString, std::function<void()>> eventHandlers() override;

 private:
  QByteArray m_fullPath;
  bool m_removed;
};
}  // namespace ConnectionsTree
