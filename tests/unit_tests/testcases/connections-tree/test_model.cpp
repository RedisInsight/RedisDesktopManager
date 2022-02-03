#include "test_model.h"
#include "connections-tree/model.h"

#include <QAbstractItemModelTester>

void TestModel::testLoadImplementation() {
  // Given
  ConnectionsTree::Model m;

  // When
  auto test =
      QScopedPointer<QAbstractItemModelTester>(new QAbstractItemModelTester(
          (QAbstractItemModel *)&m,
          QAbstractItemModelTester::FailureReportingMode::Fatal, this));

  // Then
  // No assertions
  Q_UNUSED(test);
}
