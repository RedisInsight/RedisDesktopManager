#pragma once
#include <QAbstractListModel>

class BaseListModel : public QAbstractListModel
{
    Q_OBJECT

    public:
        BaseListModel(QObject *parent = Q_NULLPTR);
        virtual ~BaseListModel() {}

    protected:
        QVariantMap getRowRaw(int row);

        inline bool isIndexValid(const QModelIndex &index) const
        {
            return 0 <= index.row() && index.row() < rowCount();
        }

        inline bool isRowIndexValid(int row) const
        {
            return 0 <= row && row < rowCount();
        }
};
