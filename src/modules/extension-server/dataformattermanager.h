#pragma once
#include "client/OAIDataFormatter.h"
#include "client/OAIHttpRequest.h"

#include <QAbstractListModel>
#include <QJSValue>
#include <QNetworkReply>
#include <QSharedPointer>
#include <QQmlApplicationEngine>

namespace RespExtServer {
class OAIDefaultApi;

class DataFormattersManager : public QAbstractListModel {
  Q_OBJECT

 public:
  enum Roles { name = Qt::UserRole + 1, id, keyTypes, magicHeader, readOnly };

 public:
  DataFormattersManager(QQmlApplicationEngine& engine);

  int rowCount(const QModelIndex& parent = QModelIndex()) const override;

  QVariant data(const QModelIndex& index, int role) const override;

  QHash<int, QByteArray> roleNames() const override;

  void setUrl(const QString& path);

 signals:
  void error(const QString& msg);

  void loaded();

 public:
  Q_INVOKABLE void loadFormatters();

  Q_INVOKABLE void decode(const QString& formatterId, const QByteArray& data,
                          QVariant context, QJSValue jsCallback);

  Q_INVOKABLE void isValid(const QString& formatterName, const QByteArray& data,
                          QVariant context, QJSValue jsCallback);

  Q_INVOKABLE void encode(const QString& formatterId, const QByteArray& data,
                          QVariant context, QJSValue jsCallback);

  Q_INVOKABLE QVariantList getPlainList();

  Q_INVOKABLE QString extServerUrl();

  Q_INVOKABLE bool isInstalled(const QString& name);

 protected slots:
  void onLoaded(QList<RespExtServer::OAIDataFormatter> summary);
  void onLoadingError(QList<RespExtServer::OAIDataFormatter> summary,
                      QNetworkReply::NetworkError error_type,
                      QString error_str);

  void onDecoded(OAIHttpRequestWorker *worker, QString summary);
  void onEncoded(OAIHttpRequestWorker *worker, QString summary);

  void onDecodeError(OAIHttpRequestWorker *worker, QNetworkReply::NetworkError error_type, QString error_str);
  void onEncodeError(OAIHttpRequestWorker *, QNetworkReply::NetworkError, QString error_str);

 private:
  void fillMapping();

  struct FormatterContext {
    QJSValue jsCallback = QJSValue();
    QString formatterId = QString();
    bool decodeValidation = false;

    FormatterContext() {}
    FormatterContext(QJSValue c, QString f) : jsCallback(c), formatterId(f) {}

    bool isValid() { return jsCallback.isCallable() && !formatterId.isEmpty(); }
  };

 private:
  QQmlApplicationEngine& m_engine;
  QList<OAIDataFormatter> m_formattersData;
  QHash<QString, int> m_mapping;
  QString m_extServerUrl;
  QSharedPointer<RespExtServer::OAIDefaultApi> m_api;
  FormatterContext m_context;
};

}
