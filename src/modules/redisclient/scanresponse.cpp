#include "scanresponse.h"

int RedisClient::ScanResponse::getCursor()
{
    QVariant value = getValue();

    if (!value.canConvert(QMetaType::QVariantList))
        return -1;

    QVariantList result = value.toList();

    return result.at(0).toInt();
}

QVariantList RedisClient::ScanResponse::getCollection()
{
    QVariant value = getValue();

    if (!value.canConvert(QMetaType::QVariantList))
        return QVariantList();

    QVariantList result = value.toList();

    return result.at(1).toList();
}

bool RedisClient::ScanResponse::isValidScanResponse(Response r)
{
    QVariant value = r.getValue();

    if (!value.canConvert(QMetaType::QVariantList))
        return false;

    QVariantList result = value.toList();

    return result.size() == 2
            && result.at(0).canConvert(QMetaType::QString)
            && result.at(1).canConvert(QMetaType::QVariantList);
}
