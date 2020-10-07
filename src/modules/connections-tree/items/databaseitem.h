#pragma once
#include "abstractnamespaceitem.h"

namespace ConnectionsTree {

class ServerItem;

class DatabaseItem : public AbstractNamespaceItem {
 public:
  DatabaseItem(unsigned int index, int keysCount,
               QSharedPointer<Operations> operations,
               QWeakPointer<TreeItem> parent, Model& model);

  ~DatabaseItem();

  QByteArray getName() const override;

  QByteArray getFullPath() const override;

  QString getDisplayName() const override;

  QString type() const override { return "database"; }

  bool isEnabled() const override;

  void notifyModel() override;

  QVariantMap metadata() const override;

  void setMetadata(const QString&, QVariant) override;

  void getMemoryUsage(std::function<void(qlonglong)> callback) override;

  void reload(std::function<void()> callback = std::function<void()>());

 protected:
  void loadKeys(std::function<void()> callback = std::function<void()>());
  void unload(bool notify = true);
  void performLiveUpdate();
  void filterKeys(const QRegExp& filter);
  void resetFilter();

  QHash<QString, std::function<void()>> eventHandlers() override;

 private:
  QSharedPointer<QTimer> liveUpdateTimer();
  bool isLiveUpdateEnabled() const;
  QVariantList filterHistoryTop10() const;

 private:
  unsigned int m_keysCount;
  QSharedPointer<QTimer> m_liveUpdateTimer;
};

}  // namespace ConnectionsTree
