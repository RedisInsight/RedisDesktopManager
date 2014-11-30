#pragma once

#include <QAbstractListModel>
#include <QString>
#include <QByteArray>
#include <QSharedPointer>
#include "abstractkeyfactory.h"
#include "keymodel.h"

namespace ConnectionsTree {
    class KeyItem;
}

namespace ValueEditor {

class ViewModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles {
        keyNameRole = Qt::UserRole + 1,
        keyIndex,
        keyTTL,
        keyType,
        state,
        showValueNavigation,
        columnNames,
        count
    };

public:
    ViewModel(QSharedPointer<AbstractKeyFactory> keyFactory);

    QModelIndex index(int row, int column = 0, const QModelIndex& parent = QModelIndex()) const;
    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex& index, const QVariant& value, int role);
    QHash<int, QByteArray> roleNames() const override;


public: // methods exported to QML
    Q_INVOKABLE void renameKey(int index, const QString& newKeyName);
    Q_INVOKABLE void removeKey(int i);
    Q_INVOKABLE void closeTab(int i);
    Q_INVOKABLE void setCurrentTab(int i);

signals:
    void keyError(int index, const QString& error);
    void closeWelcomeTab();

public slots:
    void openTab(QSharedPointer<RedisClient::Connection> connection,
                 ConnectionsTree::KeyItem& key, bool inNewTab);

private:
    QList<QSharedPointer<Model>> m_valueModels;
    QSharedPointer<AbstractKeyFactory> m_keyFactory;

    bool isIndexValid(const QModelIndex &index) const;
    void loadModel(QSharedPointer<Model> model, bool openNewTab = false);
    int m_currentTabIndex;

};

}
