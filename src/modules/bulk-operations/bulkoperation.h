#pragma once
#include <QObject>
#include <QSharedPointer>
#include <QRegExp>

#include "bulkoperationsmanager.h"

namespace BulkOperations {

    // FIXME (u_glide): Extract abstract class
    class CurrentOperation : public QObject {
        Q_OBJECT

        public:
            enum class State { READY, RUNNING, FINISHED };
        public:
            CurrentOperation(QSharedPointer<RedisClient::Connection> connection, int dbIndex,
                             Manager::Operation op=Manager::Operation::DELETE_KEYS,
                             QRegExp keyPattern=QRegExp("*", Qt::CaseSensitive, QRegExp::Wildcard));

            void getAffectedKeys(std::function<void(QVariant, QString)> callback);

            void run(std::function<void (RedisClient::Response, QString)> callback,
                    QSharedPointer<RedisClient::Connection> targetConnection = QSharedPointer<RedisClient::Connection>(),
                     int targetDbIndex = 0);

            QString getTypeName() const;

            bool multiConnectionOperation() const;

        signals:
            void notifyCallerAboutSuccess();

        public:
            QSharedPointer<RedisClient::Connection> m_connection;
            int m_dbIndex;
            Manager::Operation m_type;
            QRegExp m_keyPattern;
            State m_currentState;
            unsigned short m_progress;

        private:
            QStringList m_affectedKeys;

    };
}
