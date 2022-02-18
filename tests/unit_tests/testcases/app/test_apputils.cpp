#include "test_apputils.h"

#include "app/apputils.h"

void TestAppUtils::testHumanReadableSize() {
  long long size = 3000000000;

  QString result = humanReadableSize(size);

  QCOMPARE(result, "3.00 GB");
}
