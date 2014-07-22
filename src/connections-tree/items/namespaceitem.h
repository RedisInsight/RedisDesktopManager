#ifndef NAMESPACEITEM_H
#define NAMESPACEITEM_H

#include "treeitem.h"

namespace ConnectionsTree {

    class NamespaceItem : public TreeItem
    {
    public:
        NamespaceItem();

        void append(QSharedPointer<TreeItem> item);
    };

}
#endif // NAMESPACEITEM_H
