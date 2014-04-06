#pragma once

#include <QSharedPointer>
#include "RedisKeyItem.h"
#include "KeyModel.h"
#include "BaseTab.h"

class ValueTabView;
class Response;

class ValueTab : public BaseTab
{
    Q_OBJECT

public:
    ValueTab(RedisKeyItem * key);

    void close();

protected:    
    RedisKeyItem * key; //we don't control this object
    QSharedPointer<KeyModel> keyModel;
    QSharedPointer<ValueTabView> ui;

    bool isInitialized;

    bool tabMustBeDestroyed;

    bool operationInProgress;

    ~ValueTab();

    void destroy();

    bool isOperationsAborted();

    bool shouldBeReplaced();

protected slots:
    void keyTypeLoaded(Response type);
    void valueLoaded();
    void ttlLoaded(Response r);
    void deleteKey();
    void renameKey();
    void keyRenamed();
    void keyDeleted();
    void updateValue(const QString&, const QModelIndex *);
    void valueUpdated();
    void errorOccurred(const QString&);
    void OnClose();

signals:
    void error(const QString&);
    void keyDeleted(QWidget *, RedisKeyItem *);
};
