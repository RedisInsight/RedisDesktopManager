#pragma once
#include <QByteArray>

namespace JSONUtils {

QByteArray prettyPrintJSON(QByteArray val);

bool isJSON(QByteArray val);

QByteArray minifyJSON(const QByteArray& val);

};  // namespace JSONUtils
