#include "textexportoperation.h"
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QMutex>
#include <QMutexLocker>

#include "app/models/key-models/keyfactory.h"

BulkOperations::TextExportOperation::TextExportOperation(
        QSharedPointer<RedisClient::Connection> connection,
        int dbIndex, QRegExp keyPattern)
    : BulkOperations::AbstractOperation(connection, dbIndex, keyPattern)
{

}

void BulkOperations::TextExportOperation::run(OperationCallback callback,
                                              QSharedPointer<RedisClient::Connection>, int)
{       
    if (!m_metadata.contains("path")) {
        qWarning() << "Required Metadata for TextExportOperation is absent";
        return;
    }

    QString filePath = m_metadata["path"].toString();
    QFile outFile(filePath);
    long processedKeys = 0;
    QStringList errors;

    if (!outFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        errors << QString("Cannot open file %1 for writing").arg(filePath);
        callback(processedKeys, errors);
        return;
    }

    QTextStream stream(&outFile);
    QMutex mutex;    
    KeyFactory keyFactory;
    int callbacksCount = 0;

    for (QString k: m_affectedKeys) {
        QByteArray keyFullPath = k.toUtf8();

        try {
            keyFactory.loadKey(m_connection, keyFullPath, m_dbIndex,
                               [this, &errors, &stream, &mutex, &processedKeys](QSharedPointer<ValueEditor::Model> model, const QString& err) {
                processedKeys++;

                if (!err.isEmpty()) {
                    errors.append(err);
                    return;
                }

                QMutexLocker l(&mutex);

                QStringList commands = model->exportAsCommands();

                for(QString cmd : commands) {
                    stream << cmd << endl;
                }
            });
            callbacksCount++;
        } catch (const KeyFactory::Exception& e) {
            errors.append(QString(e.what()));
            break;
        }    
    }

    if (callbacksCount > 0) {
        while (true) {
            if (!m_connection->waitForIdle(1000*5) || callbacksCount < processedKeys)
                continue;
            break;
        }
        qDebug() << "All callbacks processed";
    }

    stream.flush();
    outFile.flush();
    outFile.close();
    callback(processedKeys, errors);
}

QString BulkOperations::TextExportOperation::getTypeName() const
{
    return QString("text_export");
}

bool BulkOperations::TextExportOperation::multiConnectionOperation() const
{
    return false;
}
