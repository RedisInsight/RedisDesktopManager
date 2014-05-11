#pragma once

#include <QStandardItem>
#include <QtConcurrent>
#include "response.h"

class RedisServerItem;
class RedisKeyItem;

class RedisServerDbItem : public QObject, public QStandardItem
{
    Q_OBJECT

    friend class RedisKeyItem;
public:
    RedisServerDbItem(QString name, int keysCount, RedisServerItem * parent);
    ~RedisServerDbItem();

    bool loadKeys();

    void setFilter(QRegExp &);
    void resetFilter();

    int virtual type() const;

    const static int TYPE = 2100;

    int getDbIndex() const;

    bool operator<(const QStandardItem & other) const;

    struct Icons {

        Icons(QIcon k, QIcon n) 
            : keyIcon(k), namespaceIcon(n)
        {
                
        }

        QIcon keyIcon;
        QIcon namespaceIcon;
    };    

protected:
    void decreaseKeyCounter();
    void setDbText();

private:
    RedisServerItem * server;
    bool isKeysLoaded;
    int dbIndex;
    unsigned int keysCount;
    QString name;
    int currentKeysPoolPosition;
    Icons iconStorage;
    QStringList rawKeys;
    QRegExp filter;
    QFutureWatcher<QList<QStandardItem *>> keysLoadingWatcher;
    QFuture<QList<QStandardItem *>> keysLoadingResult;
    RedisKeyItem * keysPool;
    RedisKeyItem * originalKeyPool;    

    void renderKeys(QStringList &);

    void setNormalIcon();

    void setBusyIcon();

private slots:
    void keysLoaded(RedisClient::Response response);
    void keysLoadingStatusChanged(int);
    void keysLoadingFinished();
};

