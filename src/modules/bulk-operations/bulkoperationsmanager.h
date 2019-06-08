#pragma once
#include <qredisclient/response.h>
#include <QObject>
#include <QRegExp>
#include <QSharedPointer>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <functional>

#include "connections.h"
#include "operations/abstractoperation.h"

class QPython;

namespace BulkOperations {

class Manager : public QObject {
  Q_OBJECT

  Q_PROPERTY(
      QString operationName READ operationName NOTIFY operationNameChanged)
  Q_PROPERTY(
      QString connectionName READ connectionName NOTIFY connectionNameChanged)
  Q_PROPERTY(int dbIndex READ dbIndex NOTIFY dbIndexChanged)
  Q_PROPERTY(QString keyPattern READ keyPattern WRITE setKeyPattern NOTIFY
                 keyPatternChanged)
  Q_PROPERTY(int operationProgress READ operationProgress NOTIFY
                 operationProgressChanged)
 public:
  enum class Operation {
    DELETE_KEYS,
    COPY_KEYS,
    IMPORT_RDB_KEYS,
    TTL,
  };

 public:
  Manager(QSharedPointer<ConnectionsModel> model, QSharedPointer<QPython> p);

  Q_INVOKABLE bool hasOperation() const;
  Q_INVOKABLE bool multiConnectionOperation() const;
  Q_INVOKABLE bool clearOperation();
  Q_INVOKABLE void runOperation(int targetConnection = -1, int targetDb = -1);
  Q_INVOKABLE void getAffectedKeys();
  Q_INVOKABLE QVariant getTargetConnections();

  Q_INVOKABLE void setOperationMetadata(const QVariantMap& meta);

  // Property getters
  QString operationName() const;

  QString connectionName() const;

  int dbIndex() const;

  QString keyPattern() const;
  void setKeyPattern(const QString& p);

  int operationProgress() const;

 signals:
  void openDialog(const QString& operationName);
  void affectedKeys(QVariant r);
  void operationFinished();
  void error(const QString& e, const QString& details);

  // Property notifiers
  void operationNameChanged();
  void connectionNameChanged();
  void dbIndexChanged();
  void keyPatternChanged();
  void operationProgressChanged();

 public slots:
  void requestBulkOperation(QSharedPointer<RedisClient::Connection> connection,
                            int dbIndex, Operation op, QRegExp keyPattern,
                            AbstractOperation::OperationCallback callback);

 private:
  QSharedPointer<AbstractOperation> m_operation;
  QSharedPointer<ConnectionsModel> m_model;
  QSharedPointer<QPython> m_python;
};
}  // namespace BulkOperations
