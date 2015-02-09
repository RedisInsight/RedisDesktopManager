#pragma once
#include "response.h"

namespace RedisClient {

class ScanResponse : public Response
{
public:
    int getCursor() const;
    QVariantList getCollection() const;

    static bool isValidScanResponse(Response& r);
};

}
