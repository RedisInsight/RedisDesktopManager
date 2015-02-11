#pragma once
#include "response.h"

namespace RedisClient {

class ScanResponse : public Response
{
public:
    ScanResponse(const QByteArray &resp): Response(resp) {}

    int getCursor();
    QVariantList getCollection();

    static bool isValidScanResponse(Response r);
};

}
