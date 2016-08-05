#include "bulkoperationsmanager.h"
#include <QDebug>
#include <qredisclient/connection.h>

#include "operations/deleteoperation.h"
#include "operations/textexportoperation.h"

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

    if (m_operation->isRunning()) {
        return false;
    }

    m_operation.clear();
    return true;
}

void BulkOperations::Manager::runOperation(int, int)
{
    if (!hasOperation())
        return;

    m_operation->run([this](long processed, const QStringList& e){
        if (e.size() > 0) {
            emit error(e);            
        }

        if (processed > 0)
            emit operationFinished();
    });
}

void BulkOperations::Manager::getAffectedKeys()
{
    if (!hasOperation())
        return;

    m_operation->getAffectedKeys([this](QVariant r, QString e){
        if (!e.isEmpty()) {
            QStringList errors;
            errors << e;
            emit error(errors);
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

void BulkOperations::Manager::setOperationMetadata(const QVariantMap &meta)
{
    if (hasOperation())
        m_operation->setMetadata(meta);
}

QString BulkOperations::Manager::operationName() const
{
    if (!hasOperation())
        return QString();

    return m_operation->getTypeName();
}

QString BulkOperations::Manager::connectionName() const
{
    if (!hasOperation())
        return QString();

    return m_operation->getConnection()->getConfig().name();
}

int BulkOperations::Manager::dbIndex() const
{
    if (!hasOperation())
        return -1;

    return m_operation->getDbIndex();
}

QString BulkOperations::Manager::keyPattern() const
{
    if (!hasOperation())
        return QString();

    return m_operation->getKeyPattern().pattern();
}

int BulkOperations::Manager::operationProgress() const
{
    if (!hasOperation())
        return -1;

    return m_operation->currentProgress();
}

void BulkOperations::Manager::requestBulkOperation(QSharedPointer<RedisClient::Connection> connection,
                                                   int dbIndex, BulkOperations::Manager::Operation op,
                                                   QRegExp keyPattern, std::function<void()> callback)
{
    if (hasOperation()) {
        qWarning() << "BulkOperationsManager already has bulk operation request";
        return;
    }

    if (op == Operation::DELETE_KEYS) {
        m_operation = QSharedPointer<BulkOperations::AbstractOperation>(
                    new BulkOperations::DeleteOperation(connection, dbIndex, keyPattern));
    } else if (op == Operation::TEXT_EXPORT) {
        m_operation = QSharedPointer<BulkOperations::AbstractOperation>(
                    new BulkOperations::TextExportOperation(connection, dbIndex, keyPattern));
    }

    QObject::connect(m_operation.data(), &BulkOperations::AbstractOperation::notifyCallerAboutSuccess, this,
                     [callback]() { if (callback) callback(); });

    emit operationNameChanged();
    emit connectionNameChanged();
    emit dbIndexChanged();
    emit keyPatternChanged();
    emit operationProgressChanged();
    emit openDialog(m_operation->getTypeName());
}
