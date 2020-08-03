#pragma once
#include <QObject>
#include <QRegExp>
#include <QSharedPointer>

#include <qredisclient/connection.h>

namespace BulkOperations {

class AbstractOperation : public QObject {
  Q_OBJECT

 public:
  enum class State { READY, RUNNING, FINISHED };

  typedef std::function<void(QRegExp affectedKeysFilter, long processed,
                             const QStringList& errors)>
      OperationCallback;

 public:
  AbstractOperation(QSharedPointer<RedisClient::Connection> connection,
                    int dbIndex, OperationCallback callback,
                    QRegExp keyPattern = QRegExp("*", Qt::CaseSensitive,
                                                 QRegExp::Wildcard));

  virtual ~AbstractOperation() {}

  virtual void getAffectedKeys(std::function<void(QVariant, QString)> callback);

  virtual void run(QSharedPointer<RedisClient::Connection> targetConnection =
                       QSharedPointer<RedisClient::Connection>(),
                   int targetDbIndex = 0);

  virtual QString getTypeName() const = 0;

  virtual bool multiConnectionOperation() const = 0;

  bool isRunning() const;

  QSharedPointer<RedisClient::Connection> getConnection();

  int getDbIndex() const;

  QRegExp getKeyPattern() const;

  void setKeyPattern(const QRegExp p);

  int currentProgress() const;

  void setMetadata(const QVariantMap& meta);

 signals:
  void progress(int processed);

 protected:
  virtual bool isMetadataValid() const { return true; }

  virtual void performOperation(
      QSharedPointer<RedisClient::Connection> targetConnection,
      int targetDbIndex) = 0;

  void incrementProgress();

  void processError(const QString& err);

 protected:
  QSharedPointer<RedisClient::Connection> m_connection;
  int m_dbIndex;
  QRegExp m_keyPattern;
  State m_currentState;
  int m_progress;
  QStringList m_affectedKeys;
  QStringList m_keysWithErrors;
  QVariantMap m_metadata;
  OperationCallback m_callback;
  QStringList m_errors;
  QMutex m_errorsMutex;
  QMutex m_processedKeysMutex;
  qint64 m_lastProgressNotification;
  QString m_errorMessagePrefix;
};
}  // namespace BulkOperations
