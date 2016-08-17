#include "abstractnamespaceitem.h"
#include "connections-tree/operations.h"
#include "connections-tree/model.h"
#include "keyitem.h"
#include "namespaceitem.h"
#include <QtConcurrent>

using namespace ConnectionsTree;


void KeysTreeRenderer::renderKeys(QSharedPointer<Operations> operations,
                                  RedisClient::Connection::RawKeysList keys,
                                  QSharedPointer<AbstractNamespaceItem> parent,
                                  RenderingSettigns settings)
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

        //if filter enabled - skip keys
        if (!settings.filter.isEmpty()) {
            QString key = QString::fromUtf8(rawKey); // UTF filtering
            if (!key.contains(settings.filter))
                continue;
        }      

        renderLazily(parent, rawKey.mid(unprocessedPartStart), rawKey, operations, settings);
    }
    qDebug() << "Tree builded in: " << timer.elapsed() << " ms";

    parent->notifyModel();
}

void KeysTreeRenderer::renderLazily(
        QSharedPointer<AbstractNamespaceItem> parent,
        const QByteArray &notProcessedKeyPart,
        const QByteArray &fullKey,
        QSharedPointer<Operations> m_operations,
        const RenderingSettigns& settings,
        unsigned long level)
{
    Q_ASSERT(parent);

    if (level > 1) {
        parent->append(fullKey);
        return;
    }

    QWeakPointer<TreeItem> currentParent = parent.staticCast<TreeItem>().toWeakRef();

    int indexOfNaspaceSeparator = (settings.nsSeparator.isEmpty())? -1 : notProcessedKeyPart.indexOf(settings.nsSeparator);

    if (indexOfNaspaceSeparator == -1) {
        QSharedPointer<KeyItem> newKey(new KeyItem(fullKey, settings.dbIndex, m_operations,
                                                   currentParent, parent->model()));
        parent->append(newKey);
        return;
    }

    QString firstNamespaceName = notProcessedKeyPart.mid(0, indexOfNaspaceSeparator);

    QSharedPointer<AbstractNamespaceItem> namespaceItem = parent->findChildNamespace(firstNamespaceName);

    if (namespaceItem.isNull()) {
        QString namespaceFullPath = fullKey.mid(0, fullKey.indexOf(notProcessedKeyPart) + firstNamespaceName.size());
        namespaceItem = QSharedPointer<NamespaceItem>(new NamespaceItem(namespaceFullPath,
                                                                        m_operations, currentParent,
                                                                        parent->model(), settings));
        parent->appendNamespace(namespaceItem);
    }

    renderLazily(namespaceItem, notProcessedKeyPart.mid(indexOfNaspaceSeparator + settings.nsSeparator.length()),
                 fullKey, m_operations, settings, level + 1);
}

AbstractNamespaceItem::AbstractNamespaceItem(Model& model,
                                             QWeakPointer<TreeItem> parent,
                                             QSharedPointer<Operations> operations,
                                             const KeysTreeRenderer::RenderingSettigns &rSettings)
    : TreeItem(model),
      m_parent(parent),
      m_operations(operations),
      m_renderingSettings(rSettings)
{

}

QList<QSharedPointer<TreeItem> > AbstractNamespaceItem::getAllChilds() const
{
    return m_childItems;
}

QSharedPointer<TreeItem> AbstractNamespaceItem::child(uint row) const
{
    if (row < m_childItems.size())
        return m_childItems.at(row);

    return QSharedPointer<TreeItem>();
}

QWeakPointer<TreeItem> AbstractNamespaceItem::parent() const
{
    return m_parent;
}

uint AbstractNamespaceItem::childCount(bool recursive) const
{
    if (!recursive)
        return m_childItems.size();

    if (m_rawChilds.size() > 0)
        return m_rawChilds.size();

    uint count = 0;
    for (auto item : m_childItems) {
        if (item->supportChildItems()) {
            count += item->childCount(true);
        } else {
            count += 1;
        }
    }
    return count;
}

void AbstractNamespaceItem::clear(bool removeRawKeys)
{
    if (removeRawKeys)
        m_rawChilds.clear();

    emit m_model.itemChildsUnloaded(getSelf());
    m_childItems.clear();
    m_childNamespaces.clear();    
}

void AbstractNamespaceItem::notifyModel()
{
    qDebug() << "Notify model about loaded childs";
    emit m_model.itemChanged(getSelf());
    emit m_model.itemChildsLoaded(getSelf());
}

void AbstractNamespaceItem::renderChilds()
{
    qDebug() << "Render keys: " << m_rawChilds.size();

    QSharedPointer<TreeItem> self = getSelf().toStrongRef();

    if (!self) {
        qDebug() << "Cannot render keys: invalid parent item";
        return;
    }

    QtConcurrent::run(&KeysTreeRenderer::renderKeys, m_operations, m_rawChilds,
                       self.dynamicCast<AbstractNamespaceItem>(), m_renderingSettings);
}
