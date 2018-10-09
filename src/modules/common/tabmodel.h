#pragma once
#include <qredisclient/connection.h>
#include <QEnableSharedFromThis>
#include <QObject>

class TabModel : public QObject, public QEnableSharedFromThis<TabModel> {
  Q_OBJECT

 public:
  TabModel(QSharedPointer<RedisClient::Connection> connection, int dbIndex);

  virtual ~TabModel();

  virtual QString getName() const = 0;

  Q_INVOKABLE virtual void init();

  virtual QSharedPointer<RedisClient::Connection> getConnection() const;

 signals:
  void error(const QString& error);

  void initialized();

 protected:
  QSharedPointer<RedisClient::Connection> m_connection;
  uint m_dbIndex;
};
