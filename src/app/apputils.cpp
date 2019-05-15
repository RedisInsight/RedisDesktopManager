#include "apputils.h"
#include <QStringList>

QString humanReadableSize(long size) {
  double num = size;
  QStringList list;
  list << "KB"
       << "MB"
       << "GB";

  QStringListIterator i(list);
  QString unit("bytes");

  if (size < 1024) {
    return QString().setNum(num) + " " + unit;
  }

  while (num >= 1024.0 && i.hasNext()) {
    unit = i.next();
    num /= 1024.0;
  }

  return QString().setNum(num, 'f', 2) + " " + unit;
}
