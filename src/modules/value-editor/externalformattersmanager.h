#pragma once
#include <QAbstractListModel>
#include <QJSValue>
#include <QProcess>

namespace ValueEditor {

class ExternalFormattersManager : public QAbstractListModel {
  Q_OBJECT

 public:
  enum Roles { name = Qt::UserRole + 1, version, description, cmd };

 public:
  ExternalFormattersManager();

  void loadFormatters();  // TODO make async with callback & invokable

  int rowCount(const QModelIndex& parent = QModelIndex()) const override;

  QVariant data(const QModelIndex& index, int role) const override;

  QHash<int, QByteArray> roleNames() const override;

  void setPath(const QString& path);

 signals:
  void error(const QString& msg);

 public:
  Q_INVOKABLE void decode(const QString& formatterName, const QByteArray& data,
                          QJSValue jsCallback);

  Q_INVOKABLE void isValid(const QString& formatterName, const QByteArray& data,
                           QJSValue jsCallback);

  Q_INVOKABLE void encode(const QString& formatterName, const QByteArray& data,
                          QJSValue jsCallback);

  Q_INVOKABLE QVariantList getPlainList();

  Q_INVOKABLE QString formattersPath();

  Q_INVOKABLE bool isInstalled(const QString& name);

 private:
  void fillMapping();

  QSharedPointer<QProcess> createProcess();

  QPair<QByteArray, QByteArray> readOutputFromExternalProcess(
      const QStringList& cmd, const QByteArray& processInput,
      const QString& wd);

  QJsonObject readJsonFromExternalProcess(const QStringList& cmd,
                                          const QByteArray& processInput,
                                          const QString& wd);

 private:
  QList<QVariantMap> m_formattersData;
  QHash<QString, int> m_mapping;
  QString m_formattersPath;
};

}  // namespace ValueEditor
