#include "test_model.h"
#include <modeltest.h>
#include "connections-tree/model.h"

void TestModel::testLoadImplementation()
{
    // Given
   ConnectionsTree::Model m;

   // When
   ModelTest test((QAbstractItemModel *)&m);

   // Then
   // No assertions
   Q_UNUSED(test);
}
