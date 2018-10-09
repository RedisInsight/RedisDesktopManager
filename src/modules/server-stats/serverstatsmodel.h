#pragma once
#include "common/tabviewmodel.h"
#include "exception.h"

namespace ServerStats {

class Model : public TabModel {
  Q_OBJECT
  ADD_EXCEPTION

  Q_PROPERTY(QVariantMap serverInfo READ serverInfo NOTIFY serverInfoChanged)

 public:
  Model(QSharedPointer<RedisClient::Connection> connection, int dbIndex);

  QString getName() const override;

  QVariantMap serverInfo();

 signals:
  void serverInfoChanged();

 private:
  QTimer m_updateTimer;
  QVariantMap m_serverInfo;
};
}  // namespace ServerStats
