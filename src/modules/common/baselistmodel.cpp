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

        if (i.value() == "display")
            continue;

        QModelIndex idx = index(row);
        QVariant d = data(idx, i.key());
        res[i.value()] = d;
    }
    return res;
}
