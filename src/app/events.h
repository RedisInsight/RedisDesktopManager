#pragma once
#include <qredisclient/connection.h>
#include <QEnableSharedFromThis>
#include <QObject>
#include <QRegExp>
#include <QSharedPointer>
#include <QString>
#include <functional>
#include "modules/bulk-operations/bulkoperationsmanager.h"
#include "common/callbackwithowner.h"

namespace ConnectionsTree {
class KeyItem;
class TreeItem;
}

class Events : public QObject, public QEnableSharedFromThis<Events> {
  Q_OBJECT

 public:
  void registerLoggerForConnection(RedisClient::Connection& c);

 signals:

  // Tabs
  void openValueTab(QSharedPointer<RedisClient::Connection> connection,
                    QSharedPointer<ConnectionsTree::KeyItem> key,
                    bool inNewTab);

  void openConsole(QSharedPointer<RedisClient::Connection> connection,
                   int dbIndex, bool inNewTab,
                   QList<QByteArray> initCmd = QList<QByteArray>());

  void openServerStats(QSharedPointer<RedisClient::Connection> connection);

  void closeDbKeys(QSharedPointer<RedisClient::Connection> connection,
                   int dbIndex,
                   const QRegExp& filter = QRegExp("*", Qt::CaseSensitive,
                                                   QRegExp::Wildcard));

  // Dialogs
  void requestBulkOperation(
      QSharedPointer<RedisClient::Connection> connection, int dbIndex,
      BulkOperations::Manager::Operation op, QRegExp keyPattern,
      BulkOperations::AbstractOperation::OperationCallback callback);

  void newKeyDialog(
      QSharedPointer<RedisClient::Connection> connection,
      QSharedPointer<CallbackWithOwner<ConnectionsTree::TreeItem>> callback,
      int dbIndex, QString keyPrefix);

  // Notifications
  void error(const QString& msg);

  void log(const QString& msg);

  void appRendered();

  void pythonLoaded();

  void externalFormattersLoaded();

};
