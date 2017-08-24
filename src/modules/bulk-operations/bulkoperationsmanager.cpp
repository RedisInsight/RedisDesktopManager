#include "bulkoperationsmanager.h"
#include <QDebug>
#include <qredisclient/connection.h>

#include "bulkoperation.h"

BulkOperations::Manager::Manager(QSharedPointer<ConnectionsModel> model)
    : QObject(nullptr),
      m_model(model)
{
    Q_ASSERT(m_model);
}

bool BulkOperations::Manager::hasOperation() const
{
    return !m_operation.isNull();
}

bool BulkOperations::Manager::multiConnectionOperation() const
{
    return m_operation && m_operation->multiConnectionOperation();
}

bool BulkOperations::Manager::clearOperation()
{
    if (!hasOperation())
        return true;

    if (m_operation->m_currentState == CurrentOperation::State::RUNNING) {
        return false;
    }

    m_operation.clear();
    return true;
}

void BulkOperations::Manager::runOperation(int, int)
{
    if (!hasOperation())
        return;

    m_operation->run([this](RedisClient::Response r, QString e){
        if (!e.isEmpty()) {
            emit error(e);
            return;
        }

        if (r.isErrorMessage()) {
            emit error(QString(QObject::tr("Bulk operation error: %1")).arg(r.getValue().toString()));
            return;
        }

        emit operationFinished();
    });
}

void BulkOperations::Manager::getAffectedKeys()
{
    if (!hasOperation())
        return;

    m_operation->getAffectedKeys([this](QVariant r, QString e){
        if (!e.isEmpty()) {
            emit error(e);
            return;
        }

        emit affectedKeys(r);
    });
}

void BulkOperations::Manager::notifyAboutOperationSuccess()
{
    if (hasOperation())
        emit m_operation->notifyCallerAboutSuccess();
}

QVariant BulkOperations::Manager::getTargetConnections()
{
    return QVariant(m_model->getConnections());
}

QString BulkOperations::Manager::connectionName() const
{
    if (!hasOperation())
        return QString();

    return m_operation->m_connection->getConfig().name();
}

int BulkOperations::Manager::dbIndex() const
{
    if (!hasOperation())
        return -1;

    return m_operation->m_dbIndex;
}

QString BulkOperations::Manager::keyPattern() const
{
    if (!hasOperation())
        return QString();

    return m_operation->m_keyPattern.pattern();
}

int BulkOperations::Manager::operationProgress() const
{
    if (!hasOperation())
        return -1;

    return m_operation->m_progress;
}

void BulkOperations::Manager::requestBulkOperation(QSharedPointer<RedisClient::Connection> connection,
                                                   int dbIndex, BulkOperations::Manager::Operation op,
                                                   QRegExp keyPattern, std::function<void()> callback)
{
    if (hasOperation()) {
        qWarning() << "BulkOperationsManager already has bulk operation request";
        return;
    }

    m_operation = QSharedPointer<BulkOperations::CurrentOperation>(
                new BulkOperations::CurrentOperation(connection, dbIndex, op, keyPattern));

    QObject::connect(m_operation.data(), &BulkOperations::CurrentOperation::notifyCallerAboutSuccess, this,
                     [callback]() { callback(); });

    emit connectionNameChanged();
    emit dbIndexChanged();
    emit keyPatternChanged();
    emit operationProgressChanged();
    emit openDialog(m_operation->getTypeName());
}
