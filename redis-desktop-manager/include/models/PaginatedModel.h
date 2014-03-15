#pragma once

#include <QStringList>
#include "KeyModel.h"

class PaginatedModel : public KeyModel
{
    Q_OBJECT

public:
    PaginatedModel(ConnectionBridge * db, const QString &keyName, int dbIndex);

    virtual ~PaginatedModel();

    virtual int getCurrentPage();

    virtual void setCurrentPage(int) = 0;

    virtual int itemsCount();

    virtual int getPagesCount();
protected:
    int currentPage;
    QSharedPointer<QVector<QString>> rawData;

    static const int itemsOnPageLimit = 500;

    void initModel(const QVariant &);    
};

