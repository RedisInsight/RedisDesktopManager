#pragma once
#include <functional>
#include "exception.h"
#include <QString>
#include <QHash>
#include <QObject>

namespace ValueEditor {

class Model : public QObject
{
    Q_OBJECT	
    ADD_EXCEPTION
public:
    virtual QString getKeyName() = 0; // sync

    virtual QString getType() = 0; // sync
    virtual int getTTL() = 0; // sync
    virtual QStringList getColumnNames() = 0; // sync
    virtual QHash<int, QByteArray> getRoles() = 0; // sync
    virtual QString getData(int rowIndex, int dataRole) = 0; // sync

    virtual bool isPartialLoadingSupported() = 0; // sync

    virtual void setKeyName(const QString&) = 0; // async
    virtual void setTTL(int) = 0; // async
    virtual void setData(int rowIndex, int dataRole, QString value) = 0; // async
    virtual void removeKey() = 0; // sync

    //rows operations	
    virtual void addRow(/* ??? */) = 0;
    virtual unsigned long rowsCount() = 0; // sync
    virtual void loadRows(unsigned long rowStart, unsigned long count, std::function<void()> callback) = 0; //async
    virtual void clearRowCache() = 0;
    virtual void removeRow(int) = 0; // async
    virtual bool isRowLoaded(int) = 0; // sync
    virtual bool isMultiRow() const = 0; // sync

    virtual ~Model() {}

signals:
    void dataLoaded();
};

}
