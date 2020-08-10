#pragma once
#include <qredisclient/connection.h>
#include <QObject>
#include <QSharedPointer>
#include <functional>

class NewKeyRequest {
  Q_GADGET

  Q_PROPERTY(QString dbIdString READ dbIdString)
  Q_PROPERTY(QString keyName READ keyName WRITE setKeyName)
  Q_PROPERTY(QString keyType READ keyType WRITE setKeyType)
  Q_PROPERTY(QVariantMap value READ value WRITE setValue)

 public:
  NewKeyRequest(QSharedPointer<RedisClient::Connection> connection, int dbIndex,
                std::function<void()> callback, QString keyPrefix = QString())
      : m_connection(connection),
        m_dbIndex(dbIndex),
        m_callback(callback),
        m_keyName(keyPrefix) {}

  NewKeyRequest() {}

  QString dbIdString() {
    return QString("%1:db%2")
        .arg(m_connection->getConfig().name())
        .arg(m_dbIndex);
  }

  int dbIndex() { return m_dbIndex; }

  QString keyName() { return m_keyName; }

  void setKeyName(QString k) { m_keyName = k; }

  QString keyType() { return m_keyType; }

  void setKeyType(QString k) { m_keyType = k; }

  QVariantMap value() const { return m_value; }

  void setValue(const QVariantMap& v) { m_value = v; }

  QSharedPointer<RedisClient::Connection> connection() { return m_connection; }

  void callback() const {
    if (m_callback) m_callback();
  }

 private:
  QSharedPointer<RedisClient::Connection> m_connection;
  int m_dbIndex;
  std::function<void()> m_callback;
  QString m_keyName;
  QString m_keyType;
  QVariantMap m_value;
};

Q_DECLARE_METATYPE(NewKeyRequest)
