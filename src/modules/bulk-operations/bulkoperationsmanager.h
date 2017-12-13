#pragma once
#include <functional>
#include <QObject>
#include <QSharedPointer>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QRegExp>
#include <qredisclient/response.h>

#include "connections.h"

namespace BulkOperations {

    class CurrentOperation;

    class Manager : public QObject
    {
        Q_OBJECT

        Q_PROPERTY(QString connectionName READ connectionName NOTIFY connectionNameChanged)
        Q_PROPERTY(int dbIndex READ dbIndex NOTIFY dbIndexChanged)
        Q_PROPERTY(QString keyPattern READ keyPattern NOTIFY keyPatternChanged)
        Q_PROPERTY(int operationProgress READ operationProgress NOTIFY operationProgressChanged)
    public:
        enum class Operation { DELETE_KEYS, COPY_KEYS };

    public:
        Manager(QSharedPointer<ConnectionsModel> model);

        Q_INVOKABLE bool hasOperation() const;
        Q_INVOKABLE bool multiConnectionOperation() const;
        Q_INVOKABLE bool clearOperation();
        Q_INVOKABLE void runOperation(int targetConnection = -1, int targetDb = -1);
        Q_INVOKABLE void getAffectedKeys();
        Q_INVOKABLE void notifyAboutOperationSuccess();
        Q_INVOKABLE QVariant getTargetConnections();

        // Property getters
        QString connectionName() const;
        int dbIndex() const;
        QString keyPattern() const;
        int operationProgress() const;

    signals:
        void openDialog(const QString& operationName);
        void affectedKeys(QVariant r);
        void operationFinished();
        void error(QString e);

        // Property notifiers
        void connectionNameChanged();
        void dbIndexChanged();
        void keyPatternChanged();
        void operationProgressChanged();

    public slots:
        void requestBulkOperation(QSharedPointer<RedisClient::Connection> connection, int dbIndex,
                                  Operation op, QRegExp keyPattern, std::function<void()> callback);

    private:                
        QSharedPointer<CurrentOperation> m_operation;
        QSharedPointer<ConnectionsModel> m_model;
    };
}
