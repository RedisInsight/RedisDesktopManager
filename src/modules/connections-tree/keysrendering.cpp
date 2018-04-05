#include "keysrendering.h"
#include "items/abstractnamespaceitem.h"
#include "items/keyitem.h"
#include "items/namespaceitem.h"

using namespace ConnectionsTree;


void KeysTreeRenderer::renderKeys(QSharedPointer<Operations> operations,
                                  RedisClient::Connection::RawKeysList keys,
                                  QSharedPointer<AbstractNamespaceItem> parent,
                                  RenderingSettigns settings,
                                  const QSet<QByteArray>& expandedNamespaces)
{
    //init
    QElapsedTimer timer;
    timer.start();
    std::sort(keys.begin(), keys.end());
    qDebug() << "Keys sorted in: " << timer.elapsed() << " ms";

    //render
    timer.restart();

    int unprocessedPartStart = 0;
    if (parent->getFullPath().size() > 0) {
        unprocessedPartStart = parent->getFullPath().size() + settings.nsSeparator.length();
    }

    for (QByteArray rawKey : keys) {
        renderLazily(parent, rawKey.mid(unprocessedPartStart), rawKey, operations, settings,
                     expandedNamespaces);
    }
    qDebug() << "Tree builded in: " << timer.elapsed() << " ms";

    parent->notifyModel();
}

void KeysTreeRenderer::renderNamespaceItems(QSharedPointer<Operations> operations,
                                            RedisClient::Connection::NamespaceItems items,
                                            QSharedPointer<AbstractNamespaceItem> parent,
                                            const QSet<QByteArray> &expandedNamespaces)
{
    QWeakPointer<TreeItem> currentParent = parent.staticCast<TreeItem>().toWeakRef();    

    qDebug() << "loaded ns items:" << items;

    for (QPair<QByteArray, ulong> ns : items.first) {        
        auto namespaceItem = QSharedPointer<NamespaceItem>(new NamespaceItem(ns.first,
                                                                             operations, currentParent,                                                                             parent->model(),
                                                                             parent->getDbIndex()));
        if (expandedNamespaces.contains(ns.first)) {
            namespaceItem->setExpanded(true);
        }

        parent->appendNamespace(namespaceItem);
    }   

    for (QByteArray fullKey : items.second) {
        QSharedPointer<KeyItem> newKey(new KeyItem(fullKey, parent->getDbIndex(), operations,
                                                   currentParent, parent->model()));
        parent->append(newKey);
    }

    parent->notifyModel();
}

void KeysTreeRenderer::renderLazily(
        QSharedPointer<AbstractNamespaceItem> parent,
        const QByteArray &notProcessedKeyPart,
        const QByteArray &fullKey,
        QSharedPointer<Operations> m_operations,
        const RenderingSettigns& settings,
        const QSet<QByteArray> &expandedNamespaces,
        unsigned long level)
{
    Q_ASSERT(parent);

//    if (level > 0 && parent->isExpanded() == false) {
//        parent->append(fullKey);
//        return;
//    }

    QWeakPointer<TreeItem> currentParent = parent.staticCast<TreeItem>().toWeakRef();

    int indexOfNaspaceSeparator = (settings.nsSeparator.isEmpty())? -1 : notProcessedKeyPart.indexOf(settings.nsSeparator);

    if (indexOfNaspaceSeparator == -1) {
        QSharedPointer<KeyItem> newKey(new KeyItem(fullKey, settings.dbIndex, m_operations,
                                                   currentParent, parent->model()));
        parent->append(newKey);
        return;
    }

    QByteArray firstNamespaceName = notProcessedKeyPart.mid(0, indexOfNaspaceSeparator);

    QSharedPointer<AbstractNamespaceItem> namespaceItem = parent->findChildNamespace(firstNamespaceName);

    if (namespaceItem.isNull()) {
        long nsPos = fullKey.size() - notProcessedKeyPart.size() + firstNamespaceName.size();
        QByteArray namespaceFullPath = fullKey.mid(0, nsPos);
        namespaceItem = QSharedPointer<NamespaceItem>(new NamespaceItem(namespaceFullPath,
                                                                        m_operations, currentParent,
                                                                        parent->model(), settings.dbIndex));

        if (expandedNamespaces.contains(namespaceFullPath)) {
            namespaceItem->setExpanded(true);
        }

        parent->appendNamespace(namespaceItem);
    }

    renderLazily(namespaceItem, notProcessedKeyPart.mid(indexOfNaspaceSeparator + settings.nsSeparator.length()),
                 fullKey, m_operations, settings, expandedNamespaces, level + 1);
}
