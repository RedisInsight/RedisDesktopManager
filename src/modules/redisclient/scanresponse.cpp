#include "scanresponse.h"

int RedisClient::ScanResponse::getCursor() const
{
    return -1; //TBD
}

QVariantList RedisClient::ScanResponse::getCollection() const
{
    return QVariantList(); //TBD
}

bool RedisClient::ScanResponse::isValidScanResponse(RedisClient::Response &r)
{
    return false; //TBD
}
