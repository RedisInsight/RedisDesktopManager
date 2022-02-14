#pragma once
#include <QJSValue>
#include "exception.h"
#include "modules/value-editor/abstractkeyfactory.h"
#include "newkeyrequest.h"
#include "modules/connections-tree/operations.h"

class KeyFactory : public QObject, public ValueEditor::AbstractKeyFactory {
  Q_OBJECT
 public:
  KeyFactory();

  void loadKey(
      QSharedPointer<RedisClient::Connection> connection,
      QByteArray keyFullPath, int dbIndex,
      std::function<void(QSharedPointer<ValueEditor::Model>, const QString&)>
          callback) override;

 public slots:
  void createNewKeyRequest(
      QSharedPointer<RedisClient::Connection> connection,
      QSharedPointer<ConnectionsTree::Operations::OpenNewKeyDialogCallback>
          callback,
      int dbIndex, QString keyPrefix);

  void submitNewKeyRequest(NewKeyRequest r);

 signals:
  void newKeyDialog(NewKeyRequest r);
  void keyAdded();
  void error(const QString& err);

 private:
  QSharedPointer<ValueEditor::Model> createModel(
      QString type, QSharedPointer<RedisClient::Connection> connection,
      QByteArray keyFullPath, int dbIndex, long long ttl);
};
