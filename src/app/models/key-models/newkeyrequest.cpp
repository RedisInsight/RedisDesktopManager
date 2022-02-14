#include "newkeyrequest.h"

NewKeyRequest::NewKeyRequest(QSharedPointer<RedisClient::Connection> connection, int dbIndex, QSharedPointer<ConnectionsTree::Operations::OpenNewKeyDialogCallback> callback, QString keyPrefix)
    : m_connection(connection),
      m_dbIndex(dbIndex),
      m_callback(callback),
      m_keyName(keyPrefix),
      m_valueFilePath(QString()){
}

void NewKeyRequest::loadAdditionalKeyTypesInfo(QJSValue jsCallback) {
    if (!m_connection) {
        qWarning() << "Invalid connection";
        return;
    }

    m_jsCallback = jsCallback;

    qDebug() << m_jsCallback.isCallable();

    m_connection->refreshServerInfo([this](){
        if (!m_jsCallback.isCallable()) {
            qDebug() << "JS callback is not callable";
            return;
        }

        if (!m_connection) {
            m_jsCallback.call(QJSValueList{});
            return;
        }

        auto loadedModules = m_connection->getEnabledModules().keys();

        QJSValueList supportedKeyTypesExposedByModules;

        for (QString mod : loadedModules) {
            if (mod == "ReJSON")
                supportedKeyTypesExposedByModules.append(QJSValue(mod));
        }

        m_jsCallback.call(supportedKeyTypesExposedByModules);
    });
}
