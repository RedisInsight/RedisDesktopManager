#pragma once

#include "ListKeyModel.h"

class SetKeyModel : public ListKeyModel
{
    Q_OBJECT

public:
    SetKeyModel(ConnectionBridge * db, const QString &keyName, int dbIndex);

    void loadValue();

    void updateValue(const QString& value, const QModelIndex *cellIndex);

protected slots:
    void loadedUpdateStatus(Response);
};

