#include "databaseitem.h"

using namespace ConnectionsTree;

DatabaseItem::DatabaseItem(const QString& displayName, unsigned int index, int keysCount, QSharedPointer<Operations> operations)
{
}

bool DatabaseItem::onClick(QWeakPointer<ParentView> treeView, QWeakPointer<QTabWidget> tabs)
{
//    RedisServerDbItem * db = dynamic_cast<RedisServerDbItem *>(item);

//    db->loadKeys();

//    connections->blockSignals(true);

    return true;
}

void DatabaseItem::loadKeys()
{
   // m_opera
}
