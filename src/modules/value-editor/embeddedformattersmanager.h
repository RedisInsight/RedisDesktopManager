#pragma once
#include <QAbstractListModel>
#include <QJSValue>
#include <QSharedPointer>

class QPython;

namespace ValueEditor {

class EmbeddedFormattersManager : public QObject {
  Q_OBJECT

 public:
  enum Roles { name = Qt::UserRole + 1, version, description, cmd };

 public:
  EmbeddedFormattersManager(QSharedPointer<QPython> p);

 signals:
  void error(const QString& msg);

 public:
  Q_INVOKABLE void loadFormatters(QJSValue callback);

  Q_INVOKABLE void decode(const QString& formatterName, const QByteArray& data,
                          QJSValue jsCallback);

  Q_INVOKABLE void isValid(const QString& formatterName, const QByteArray& data,
                           QJSValue jsCallback);

  Q_INVOKABLE void encode(const QString& formatterName, const QByteArray& data,
                          QJSValue jsCallback);

 private:
  QSharedPointer<QPython> m_python;
};

}  // namespace ValueEditor
