#pragma once
#include <QList>
#include <QHash>
#include "common/baselistmodel.h"

namespace Console {

    class AutocompleteModel : public BaseListModel
    {
        Q_OBJECT

    public:
        enum Roles {
            name = Qt::UserRole + 1,
            arguments,
            summary,
            since
        };

        AutocompleteModel();

        QModelIndex index(int row, int column = 0, const QModelIndex& parent = QModelIndex()) const override;

        int rowCount(const QModelIndex& parent = QModelIndex()) const override;

        QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

        QHash<int, QByteArray> roleNames() const override;

        Q_INVOKABLE QVariantMap getRow(int i);

    private:
        struct CommandInfo {
            QString name;
            QString arguments;
            QString summary;
            QString since;
        };

        QList<CommandInfo> m_commands;
    };
}
