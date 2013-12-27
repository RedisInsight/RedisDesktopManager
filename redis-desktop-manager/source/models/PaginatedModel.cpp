#include "PaginatedModel.h"

PaginatedModel::PaginatedModel(ConnectionBridge * db, const QString &keyName, int dbIndex)
    : KeyModel(db, keyName, dbIndex), currentPage(0), rawData(nullptr)
{
}

void PaginatedModel::initModel(const QVariant & val)
{
    rawData = QSharedPointer<QVector<QString>>(new QVector<QString>);

    if (val.canConvert<QStringList>())
        *rawData = val.value<QStringList>().toVector();
    
    setCurrentPage(1);
}

int PaginatedModel::getCurrentPage()
{
    return currentPage;
}

int PaginatedModel::itemsCount()
{
    return rawData->size();
}

int PaginatedModel::getPagesCount()
{
    int pages = itemsCount() / itemsOnPageLimit;

    if (itemsCount() % itemsOnPageLimit > 0) {
        pages++;
    }

    return pages;
}

PaginatedModel::~PaginatedModel()
{
}
