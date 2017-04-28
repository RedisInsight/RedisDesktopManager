#pragma once
#include <functional>
#include <QAbstractListModel>
#include "tabmodel.h"

class TabViewModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles {
        tabName = Qt::UserRole + 1,
        tabIndex,
    };

    typedef std::function<QSharedPointer<TabModel>(QSharedPointer<RedisClient::Connection>)> ModelFactory;

public:
    TabViewModel(const ModelFactory& modelFactory);

    QModelIndex index(int row, int column = 0, const QModelIndex& parent = QModelIndex()) const;

    int rowCount(const QModelIndex& parent = QModelIndex()) const;

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

    QHash<int, QByteArray> roleNames() const override;

public: // methods exported to QML
    Q_INVOKABLE void closeTab(int i);

    Q_INVOKABLE void setCurrentTab(int i);

    Q_INVOKABLE QObject* getValue(int i);

    Q_INVOKABLE int tabsCount() const;

signals:
    void changeCurrentTab(int i);

public slots:
    void openTab(QSharedPointer<RedisClient::Connection> connection);

    void closeAllTabsWithConnection(QSharedPointer<RedisClient::Connection> connection);

private:
    QList<QSharedPointer<TabModel>> m_models;
    int m_currentTabIndex;
    ModelFactory m_modelFactory;

    bool isIndexValid(const QModelIndex &index) const;

};

template <class T> TabViewModel::ModelFactory getTabModelFactory()
{
    return TabViewModel::ModelFactory([](QSharedPointer<RedisClient::Connection> c) {
        return QSharedPointer<TabModel>(new T(c));
    });
}
