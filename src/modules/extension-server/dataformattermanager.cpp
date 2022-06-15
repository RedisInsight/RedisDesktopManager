#include "dataformattermanager.h"

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSettings>

#include "app/models/configmanager.h"
#include "client/OAIDefaultApi.h"

RespExtServer::DataFormattersManager::DataFormattersManager(QQmlApplicationEngine &engine)
    : m_engine(engine), m_api(new RespExtServer::OAIDefaultApi()) {
  QSettings settings;

  int requestTimeout =
      settings.value("app/extensionServerRequestTimeout", 10).toInt() * 1000;

  m_api->setTimeOut(requestTimeout);
  QObject::connect(m_api.data(), &OAIDefaultApi::dataFormattersGetSignal, this,
                   &DataFormattersManager::onLoaded);

  QObject::connect(m_api.data(), &OAIDefaultApi::dataFormattersGetSignalE, this,
                   &DataFormattersManager::onLoadingError);

  QObject::connect(m_api.data(),
                   &OAIDefaultApi::dataFormattersIdDecodePostSignalFull, this,
                   &DataFormattersManager::onDecoded);

  QObject::connect(m_api.data(),
                   &OAIDefaultApi::dataFormattersIdEncodePostSignalFull, this,
                   &DataFormattersManager::onEncoded);

  QObject::connect(m_api.data(),
                   &OAIDefaultApi::dataFormattersIdDecodePostSignalEFull, this,
                   &DataFormattersManager::onDecodeError);

  QObject::connect(m_api.data(),
                   &OAIDefaultApi::dataFormattersIdEncodePostSignalEFull, this,
                   &DataFormattersManager::onEncodeError);
}

void RespExtServer::DataFormattersManager::loadFormatters() {
  m_api->setNewServerForAllOperations(extServerUrl());

  QSettings settings;

  QString user =
      settings.value("app/extensionServerUser", QString()).toString();
  QString password =
      settings.value("app/extensionServerPassword", QString()).toString();

  if (!user.isEmpty() && !password.isEmpty()) {
    m_api->setUsername(user);
    m_api->setPassword(password);
  }

  m_api->dataFormattersGet();
}

int RespExtServer::DataFormattersManager::rowCount(const QModelIndex &) const {
  return m_formattersData.size();
}

QVariant RespExtServer::DataFormattersManager::data(const QModelIndex &index,
                                                    int role) const {
  if (!(0 <= index.row() && index.row() < rowCount())) {
    return QVariant();
  }

  auto data = m_formattersData[index.row()];

  if (role == name) {
    return data.getName();
  } else if (role == id) {
    return data.getId();
  } else if (role == keyTypes) {
    return data.getKeyTypes();
  } else if (role == magicHeader) {
    return data.getMagicHeader();
  } else if (role == readOnly) {
    return data.isReadOnly();
  }

  return QVariant();
}

QHash<int, QByteArray> RespExtServer::DataFormattersManager::roleNames() const {
  QHash<int, QByteArray> roles;
  roles[id] = "id";
  roles[name] = "name";
  roles[keyTypes] = "keyTypes";
  roles[magicHeader] = "magicHeader";
  roles[readOnly] = "readOnly";
  return roles;
}

void RespExtServer::DataFormattersManager::setUrl(const QString &path) {
  m_extServerUrl = path;
}

void RespExtServer::DataFormattersManager::decode(const QString &formatterId,
                                                  const QByteArray &data,
                                                  QVariant context,
                                                  QJSValue jsCallback) {
  if (!m_mapping.contains(formatterId)) {
    emit error(QCoreApplication::translate("RESP", "Can't find formatter: %1")
                   .arg(formatterId));
    return;
  }

  if (!jsCallback.isCallable() || !context.canConvert<QVariantMap>()) {
    emit error(QCoreApplication::translate("RESP", "Invalid callback"));
    return;
  }

  auto requestContext = context.toMap();

  m_context = FormatterContext{jsCallback, formatterId};

  OAIDecodePayload payload;
  payload.setData(data.toBase64());
  payload.setRedisKeyName(requestContext["redis-key-name"].toByteArray());
  payload.setRedisKeyType(requestContext["redis-key-type"].toString());

  m_api->dataFormattersIdDecodePost(formatterId, payload);
}

void RespExtServer::DataFormattersManager::isValid(const QString &formatterId,
                                                   const QByteArray &data,
                                                   QVariant context,
                                                   QJSValue jsCallback) {
  // TODO: Check magic header if any
  Q_UNUSED(context);
  Q_UNUSED(formatterId);
  Q_UNUSED(data);
  jsCallback.call(QJSValueList{true});
}

void RespExtServer::DataFormattersManager::encode(const QString &formatterId,
                                                  const QByteArray &data,
                                                  QVariant context,
                                                  QJSValue jsCallback) {
  if (!m_mapping.contains(formatterId)) {
    emit error(QCoreApplication::translate("RESP", "Can't find formatter: %1")
                   .arg(formatterId));
    return;
  }

  m_context = FormatterContext{jsCallback, formatterId};

  OAIEncodePayload payload;
  payload.setData(data.toBase64());

  auto requestContext = QJsonDocument::fromVariant(context);

  if (requestContext.isObject()) {
    OAIObject metadata;
    metadata.fromJsonObject(requestContext.object());
    payload.setMetadata(metadata);
  }

  m_api->dataFormattersIdEncodePost(formatterId, payload);
}

QVariantList RespExtServer::DataFormattersManager::getPlainList() {
  QList<QVariant> r;
  foreach (auto v, m_formattersData) {
    r.append(v.asJsonObject().toVariantMap());
  }
  return r;
}

QString RespExtServer::DataFormattersManager::extServerUrl() {
  if (!m_extServerUrl.isEmpty()) {
    return m_extServerUrl;
  }

  QSettings settings;
  return settings.value("app/extensionServerUrl", QString()).toString();
}

bool RespExtServer::DataFormattersManager::isInstalled(const QString &name) {
  return m_mapping.contains(name);
}

void RespExtServer::DataFormattersManager::onLoaded(
    QList<RespExtServer::OAIDataFormatter> summary) {
  qDebug() << "Formatters loaded from extension server" << summary.size();

  emit layoutAboutToBeChanged();
  m_formattersData = summary;
  fillMapping();
  emit layoutChanged();
  emit loaded();
}

void RespExtServer::DataFormattersManager::onLoadingError(
    QList<RespExtServer::OAIDataFormatter>, QNetworkReply::NetworkError,
    QString error_str) {
  emit error(
      QCoreApplication::translate(
          "RESP",
          "Can't load list of available formatters from extension server: %1")
          .arg(error_str));
}

void RespExtServer::DataFormattersManager::onDecoded(
    OAIHttpRequestWorker *worker, QString) {
  if (!worker || !m_context.isValid()) return;

  auto headers = worker->getResponseHeaders();

  QString format{"plain"};
  auto decoded = QString::fromUtf8(worker->response);

  if (headers.contains("Content-Type")) {
    if (headers["Content-Type"].toLower() == "application/json") {
      format = "json";
    } else if (headers["Content-Type"].toLower().startsWith("image")) {
      format = "image";
      decoded = QString("data:%1;base64,%2")
                    .arg(headers["Content-Type"])
                    .arg(QString(worker->response.toBase64()));
    }
  }

  auto formatter = m_formattersData[m_mapping[m_context.formatterId]];

  m_context.jsCallback.call(
      QJSValueList{QString(), decoded, formatter.isReadOnly(), format});
}

void RespExtServer::DataFormattersManager::onEncoded(
    OAIHttpRequestWorker *worker, QString) {
  if (!worker || !m_context.isValid()) return;

  auto encoded = m_engine.toScriptValue(worker->response);

  m_context.jsCallback.call(QJSValueList{QString(), encoded});
}

void RespExtServer::DataFormattersManager::onDecodeError(
    OAIHttpRequestWorker *worker, QNetworkReply::NetworkError,
    QString error_str) {
  if (!worker || !m_context.isValid()) return;

  auto formatter = m_formattersData[m_mapping[m_context.formatterId]];

  m_context.jsCallback.call(QJSValueList{error_str, QString(), true, "plain"});
}

void RespExtServer::DataFormattersManager::onEncodeError(
    OAIHttpRequestWorker *worker, QNetworkReply::NetworkError,
    QString error_str) {
  if (!worker || !m_context.isValid()) return;

  emit error(QCoreApplication::translate("RESP", "Can't encode value: %1")
                 .arg(error_str));
}

void RespExtServer::DataFormattersManager::fillMapping() {
  int index = 0;

  for (const auto &f : qAsConst(m_formattersData)) {
    m_mapping[f.getId()] = index;
    index++;
  }
}
