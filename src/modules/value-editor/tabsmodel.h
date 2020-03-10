#pragma once
#include <QAbstractListModel>
#include <QByteArray>
#include <QPair>
#include <QSharedPointer>
#include <QString>
#include <functional>
#include "abstractkeyfactory.h"
#include "valueviewmodel.h"

class Events;

namespace ConnectionsTree {
class KeyItem;
}

namespace ValueEditor {

class TabsModel : public QAbstractListModel {
  Q_OBJECT

 public:
  enum Roles {
    keyNameRole = Qt::UserRole + 1,
    keyIndex,
    keyTTL,
    keyType,
    isMultiRow,
    rowsCount,
    keyModel,
    showLoader,
    tabName,
  };

 public:
  TabsModel(QSharedPointer<AbstractKeyFactory> keyFactory,
            QSharedPointer<Events> events);

  ~TabsModel() override;

  QModelIndex index(int row, int column = 0,
                    const QModelIndex& parent = QModelIndex()) const override;
  int rowCount(const QModelIndex& parent = QModelIndex()) const override;
  QVariant data(const QModelIndex& index,
                int role = Qt::DisplayRole) const override;
  QHash<int, QByteArray> roleNames() const override;

 public:  // methods exported to QML
  Q_INVOKABLE void closeTab(int i);
  Q_INVOKABLE void setCurrentTab(int i);

 signals:
  void tabError(int index, const QString& error);
  void replaceTab(int index);

 public slots:
  void openTab(QSharedPointer<RedisClient::Connection> connection,
               QSharedPointer<ConnectionsTree::KeyItem> key, bool inNewTab);
  void closeDbKeys(QSharedPointer<RedisClient::Connection> connection,
                   int dbIndex, const QRegExp& filter);

 private:
  QList<QSharedPointer<ValueViewModel>> m_viewModels;
  QSharedPointer<AbstractKeyFactory> m_keyFactory;
  QSharedPointer<Events> m_events;
  int m_currentTabIndex;

  bool isIndexValid(const QModelIndex& index) const;
  QSharedPointer<ValueViewModel> loadModel(const QString& loadingBanner,
                 QWeakPointer<ConnectionsTree::KeyItem> key,
                 bool openNewTab = false);
  void tabChanged(QSharedPointer<ValueViewModel> m);
  void tabRemoved(QSharedPointer<ValueViewModel> m);
};

}  // namespace ValueEditor
