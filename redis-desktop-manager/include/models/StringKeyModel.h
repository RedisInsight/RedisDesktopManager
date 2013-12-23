#pragma once

#include <QObject>
#include "KeyModel.h"

class StringKeyModel : public KeyModel
{
    Q_OBJECT

public:
    StringKeyModel(ConnectionBridge * db, const QString &keyName, int dbIndex);

    void loadValue();

    QString getValue();

    static const int KEY_MODEL_TYPE = 2;

    inline int getKeyModelType()
    {
        return StringKeyModel::KEY_MODEL_TYPE;
    }

    void updateValue(const QString& value, const QModelIndex *cellIndex);

protected:
    QString plainData;

    void initModel(const QVariant &);

protected slots:
    void loadedUpdateStatus(Response);

};

