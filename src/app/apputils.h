#pragma once
#include <QString>
#include <QLocale>

inline QString humanReadableSize(qint64 size) {
  return QLocale().formattedDataSize(size, 2, QLocale::DataSizeSIFormat);
}
