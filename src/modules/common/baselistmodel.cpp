#include "baselistmodel.h"

BaseListModel::BaseListModel(QObject *parent)
    : QAbstractListModel(parent)
{

}

QVariantMap BaseListModel::getRowRaw(int row)
{
    QHash<int,QByteArray> names = roleNames();
    QHashIterator<int, QByteArray> i(names);
    QVariantMap res;

    while (i.hasNext()) {
        i.next();
        QModelIndex idx = index(row, 0);
        QVariant data = idx.data(i.key());
        res[i.value()] = data;
    }
    return res;
}
