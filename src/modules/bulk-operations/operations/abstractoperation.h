#pragma once
#include <QObject>
#include <QSharedPointer>
#include <QRegExp>

#include <qredisclient/connection.h>

namespace BulkOperations {

    class AbstractOperation : public QObject {
        Q_OBJECT

        public:
            enum class State { READY, RUNNING, FINISHED };

            typedef std::function<void(long processed, const QStringList& errors)> OperationCallback;

        public:
            AbstractOperation(QSharedPointer<RedisClient::Connection> connection, int dbIndex,
                              QRegExp keyPattern=QRegExp("*", Qt::CaseSensitive, QRegExp::Wildcard));

            virtual ~AbstractOperation() {}

            void getAffectedKeys(std::function<void(QVariant, QString)> callback);

            virtual void run(OperationCallback callback,
                             QSharedPointer<RedisClient::Connection> targetConnection = QSharedPointer<RedisClient::Connection>(),
                             int targetDbIndex = 0) = 0;

            virtual QString getTypeName() const = 0;

            virtual bool multiConnectionOperation() const = 0;

            bool isRunning() const;

            QSharedPointer<RedisClient::Connection> getConnection();

            int getDbIndex() const;

            QRegExp getKeyPattern() const;

            unsigned short currentProgress() const;

            void setMetadata(const QVariantMap &meta);

        signals:
            void notifyCallerAboutSuccess();

        protected:
            QSharedPointer<RedisClient::Connection> m_connection;
            int m_dbIndex;
            QRegExp m_keyPattern;
            State m_currentState;
            unsigned short m_progress;
            QStringList m_affectedKeys;
            QVariantMap m_metadata;
    };
}
