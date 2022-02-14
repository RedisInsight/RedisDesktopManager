#pragma once
#include <qredisclient/connection.h>
#include <QObject>
#include <QSharedPointer>
#include <functional>
#include <QJSValue>
#include "modules/connections-tree/operations.h"

class NewKeyRequest {
  Q_GADGET

  Q_PROPERTY(QString dbIdString READ dbIdString)
  Q_PROPERTY(QString keyName READ keyName WRITE setKeyName)
  Q_PROPERTY(QString keyType READ keyType WRITE setKeyType)
  Q_PROPERTY(QVariantMap value READ value WRITE setValue)
  Q_PROPERTY(QString valueFilePath READ valueFilePath WRITE setValueFilePath)  

 public:
  NewKeyRequest(QSharedPointer<RedisClient::Connection> connection, int dbIndex,
                QSharedPointer<ConnectionsTree::Operations::OpenNewKeyDialogCallback> callback,
                QString keyPrefix = QString());

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

  QString valueFilePath() const { return m_valueFilePath; }

  void setValueFilePath(const QString& path) { m_valueFilePath = path; }

  QSharedPointer<RedisClient::Connection> connection() { return m_connection; }

  void callback() const {
    if (m_callback) m_callback->call();
  }

  Q_INVOKABLE void loadAdditionalKeyTypesInfo(QJSValue jsCallback);

 private:
  QSharedPointer<RedisClient::Connection> m_connection = nullptr;
  int m_dbIndex = -1;
  QSharedPointer<ConnectionsTree::Operations::OpenNewKeyDialogCallback> m_callback;
  QJSValue m_jsCallback;
  QString m_keyName;
  QString m_keyType;
  QVariantMap m_value;
  QString m_valueFilePath;
};

Q_DECLARE_METATYPE(NewKeyRequest)
