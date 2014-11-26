#pragma once

#include <QAbstractListModel>
#include <QString>
#include <QByteArray>
#include <QSharedPointer>
#include "abstractkeyfactory.h"
#include "keymodel.h"

namespace ValueEditor {

class ViewModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles {
        keyNameRole = Qt::UserRole + 1,
        keyTTL, keyType, state, showValueNavigation, columnNames, count
    };

public:
    ViewModel(QSharedPointer<AbstractKeyFactory> keyFactory);

    Q_INVOKABLE Model* getValueModel(int index);
    Q_INVOKABLE void closeKey(int index);

    QModelIndex index(int row, int column = 0, const QModelIndex& parent = QModelIndex()) const;
    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex& index, const QVariant& value, int role);
    QHash<int, QByteArray> roleNames() const override;

public slots:
    void openTab(QSharedPointer<RedisClient::Connection> connection,
                 const QString& keyFullPath, int dbIndex, bool inNewTab);

private:
    QList<QSharedPointer<Model>> m_valueModels;
    QSharedPointer<AbstractKeyFactory> m_keyFactory;

    bool isIndexValid(const QModelIndex &index) const;
    void loadModel(QSharedPointer<Model> model);

};

}
