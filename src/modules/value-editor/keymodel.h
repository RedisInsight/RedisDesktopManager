#pragma once
#include <functional>
#include "exception.h"
#include <QString>
#include <QHash>
#include <QObject>
#include <qredisclient/connection.h>

namespace ValueEditor {

class ModelSignals : public QObject
{
    Q_OBJECT
public:
    ModelSignals(){}
signals:
    void dataLoaded();
    void removed();
};

class Model
{    
    ADD_EXCEPTION
public:
    Model() {}
    virtual QString getKeyName() = 0;
    virtual QString getKeyTitle() = 0;

    virtual QString getType() = 0;
    virtual long long getTTL() = 0;
    virtual QStringList getColumnNames() = 0;
    virtual QHash<int, QByteArray> getRoles() = 0;
    virtual QVariant getData(int rowIndex, int dataRole) = 0;    

    virtual void setKeyName(const QByteArray&) = 0; // async
    virtual void setTTL(const long long) = 0; // async
    virtual void removeKey() = 0;

    //rows operations	
    virtual void addRow(const QVariantMap&) = 0;
    virtual void updateRow(int rowIndex, const QVariantMap&) = 0; // async
    virtual unsigned long rowsCount() = 0;
    virtual void loadRows(unsigned long rowStart, unsigned long count, std::function<void(const QString&)> callback) = 0; //async
    virtual void clearRowCache() = 0;
    virtual void removeRow(int) = 0; // async
    virtual bool isRowLoaded(int) = 0;
    virtual bool isMultiRow() const = 0;

    virtual QSharedPointer<ModelSignals> getConnector() const = 0;
    virtual QSharedPointer<RedisClient::Connection> getConnection() const = 0;
    virtual unsigned int dbIndex() const = 0;

    virtual ~Model() {}
};

}
