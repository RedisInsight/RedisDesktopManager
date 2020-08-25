#pragma once
#include <QAbstractListModel>
#include <functional>
#include "tabmodel.h"

class TabViewModel : public QAbstractListModel {
  Q_OBJECT

 public:
  enum Roles {
    tabName = Qt::UserRole + 1,
    tabIndex,
    tabModel,
  };

  typedef std::function<QSharedPointer<TabModel>(
      QSharedPointer<RedisClient::Connection>, int dbIndex, QList<QByteArray> initCmd)>
      ModelFactory;

 public:
  TabViewModel(const ModelFactory& modelFactory);

  QModelIndex index(int row, int column = 0,
                    const QModelIndex& parent = QModelIndex()) const override;

  int rowCount(const QModelIndex& parent = QModelIndex()) const override;

  QVariant data(const QModelIndex& index,
                int role = Qt::DisplayRole) const override;

  QHash<int, QByteArray> roleNames() const override;

 public:  // methods exported to QML
  Q_INVOKABLE void closeTab(int i);

  Q_INVOKABLE int tabsCount() const;

 signals:
  void changeCurrentTab(int i);

 public slots:
  void openTab(QSharedPointer<RedisClient::Connection> connection,
               int dbIndex = 0, QList<QByteArray> initCmd=QList<QByteArray>());

  void closeAllTabsWithConnection(
      QSharedPointer<RedisClient::Connection> connection);

 private:
  QList<QSharedPointer<TabModel>> m_models;
  ModelFactory m_modelFactory;

  bool isIndexValid(const QModelIndex& index) const;
};

template <class T>
TabViewModel::ModelFactory getTabModelFactory() {
  return TabViewModel::ModelFactory(
      [](QSharedPointer<RedisClient::Connection> c, int dbIndex, QList<QByteArray> initCmd) {
        return QSharedPointer<TabModel>(new T(c, dbIndex, initCmd),
                                        &QObject::deleteLater);
      });
}
