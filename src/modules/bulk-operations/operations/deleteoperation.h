#pragma once
#include <QObject>
#include <QSharedPointer>
#include <QRegExp>
#include "abstractoperation.h"

namespace BulkOperations {

    class DeleteOperation : public AbstractOperation {
        Q_OBJECT

        public:
            DeleteOperation(QSharedPointer<RedisClient::Connection> connection, int dbIndex,
                          QRegExp keyPattern=QRegExp("*", Qt::CaseSensitive, QRegExp::Wildcard));

            void run(OperationCallback callback,
                     QSharedPointer<RedisClient::Connection> targetConnection = QSharedPointer<RedisClient::Connection>(),
                     int targetDbIndex = 0) override;

            QString getTypeName() const override;

            bool multiConnectionOperation() const override;
    };
}
