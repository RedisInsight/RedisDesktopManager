#pragma once

#include <QStandardItemModel>
#include "connection.h"
#include "command.h"
#include "response.h"

class KeyModel : public QStandardItemModel
{
    Q_OBJECT

public:
    KeyModel(RedisClient::Connection * db, const QString &keyName, int dbIndex);
    virtual ~KeyModel();    

    virtual void loadValue() = 0;

    void loadTTL();

    QString getKeyName();

    void renameKey(const QString&);

    void deleteKey();

    virtual void updateValue(const QString& value, const QModelIndex *cellIndex) = 0;

    const static int KEY_MODEL_TYPE = 1;

    const static int KEY_VALUE_TYPE_ROLE = 7000;

    inline virtual int getKeyModelType()
    {
        return KEY_MODEL_TYPE;
    }

signals:    
    void valueLoaded();
    void ttlLoaded(RedisClient::Response);
    void keyRenamed();
    void keyRenameError(const QString&);
    void keyDeleted();
    void keyDeleteError(const QString&);
    void valueUpdated();
    void valueUpdateError(const QString&);

protected slots:
    void loadedValue(RedisClient::Response);
    void loadedRenameStatus(RedisClient::Response);
    void loadedDeleteStatus(RedisClient::Response);

protected:    
    RedisClient::Connection * db;
    QString keyName;
    int dbIndex;    

    virtual void initModel(const QVariant &) = 0;
};


