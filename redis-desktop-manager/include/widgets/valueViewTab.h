#pragma once

#include <QWidget>
#include <QSharedPointer>
#include "RedisKeyItem.h"
#include "KeyModel.h"

class ValueTabView;
class Response;

class ValueTab : public QWidget
{
    Q_OBJECT

public:
    ValueTab(RedisKeyItem * key);

    bool close();

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

protected slots:
    void keyTypeLoaded(Response type);

    void valueLoaded();

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
