#pragma once
#include "exception.h"
#include <QString>
#include <QHash>

namespace ValueEditor {

class Model
{
    ADD_EXCEPTION
public:
    virtual QString getKeyName() = 0;
    virtual int getTTL() = 0;
    virtual QString getType() = 0;
    virtual QStringList getColumnNames() = 0;
    virtual QHash<int, QByteArray> getRoles() = 0;
    virtual QString getData(int rowIndex, int dataRole) = 0;
    virtual unsigned long rowsCount() = 0;

    virtual bool isMultiRow() const = 0;
    virtual bool isPartialLoadingSupported() = 0;

    virtual void setKeyName(const QString&) = 0;
    virtual void setTTL(int) = 0;
    virtual void setData(int rowIndex, int dataRole, QString value) = 0;

    virtual void removeKey() = 0;
    virtual void removeRow(int) = 0;

    virtual ~Model() {}
};

}
