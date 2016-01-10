#pragma once
#include <functional>
#include <QAbstractListModel>
#include <QString>
#include <QPair>
#include <QByteArray>
#include <QSharedPointer>
#include <QJSValue>
#include "consolemodel.h"

namespace Console {

class ViewModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles {
        consoleName = Qt::UserRole + 1,
        consoleState,
        consoleIndex,
    };

public:
    ViewModel();

    QModelIndex index(int row, int column = 0, const QModelIndex& parent = QModelIndex()) const;
    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;    
    QHash<int, QByteArray> roleNames() const override;

public: // methods exported to QML    
    Q_INVOKABLE void closeTab(int i);
    Q_INVOKABLE void setCurrentTab(int i);
    Q_INVOKABLE QObject* getValue(int i);    

public slots:    
    void openConsole(QSharedPointer<RedisClient::Connection> connection);
    void closeAllTabsWithConnection(QSharedPointer<RedisClient::Connection> connection);

private:
    QList<QSharedPointer<Model>> m_models;
    int m_currentTabIndex;    

    bool isIndexValid(const QModelIndex &index) const;        
};

}
