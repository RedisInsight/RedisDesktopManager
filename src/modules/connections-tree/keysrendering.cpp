#include "keysrendering.h"

#include "items/abstractnamespaceitem.h"
#include "items/keyitem.h"
#include "items/namespaceitem.h"

using namespace ConnectionsTree;

void KeysTreeRenderer::renderKeys(QSharedPointer<Operations> operations,
                                  RedisClient::Connection::RawKeysList keys,
                                  QSharedPointer<AbstractNamespaceItem> parent,
                                  RenderingSettigns settings,
                                  const QSet<QByteArray> &expandedNamespaces) {
  // init
  QElapsedTimer timer;
  timer.start();

  if (settings.sortKeys)
    std::sort(keys.begin(), keys.end());

  qDebug() << "Keys sorted in: " << timer.elapsed() << " ms";

  // render
  timer.restart();

  int unprocessedPartStart = 0;
  if (parent->getFullPath().size() > 0 || parent->type() == "namespace") {
    unprocessedPartStart =
        parent->getFullPath().size() + settings.nsSeparator.length();
  }

  QByteArray rawKey;
  QByteArray nextKey;

  while (!keys.isEmpty()) {
    rawKey = keys.takeFirst();

    if (keys.size() > 0) {
        nextKey = keys[0];
    } else {
        nextKey = QByteArray();
    }

    try {
      renderLazily(parent, rawKey.mid(unprocessedPartStart), rawKey, operations,
                   settings, expandedNamespaces, 0, nextKey);
    } catch (std::bad_alloc &) {
      parent->showLoadingError("Not enough memory to render all keys");
      break;
    }
  }
  qDebug() << "Tree builded in: " << timer.elapsed() << " ms";

  parent->notifyModel();
}

void KeysTreeRenderer::renderLazily(
    QSharedPointer<AbstractNamespaceItem> parent,
    const QByteArray &notProcessedKeyPart, const QByteArray &fullKey,
    QSharedPointer<Operations> m_operations, const RenderingSettigns &settings,
    const QSet<QByteArray> &expandedNamespaces, unsigned long level,
    const QByteArray &nextKey) {
  Q_ASSERT(parent);

  if (level > 0 && parent->isExpanded() == false) {
    parent->appendRawKey(fullKey);
    return;
  }

  QWeakPointer<TreeItem> currentParent =
      parent.staticCast<TreeItem>().toWeakRef();

  int indexOfNaspaceSeparator =
      (settings.nsSeparator.isEmpty())
          ? -1
          : notProcessedKeyPart.indexOf(settings.nsSeparator);  

  if (indexOfNaspaceSeparator == -1) {
    QSharedPointer<KeyItem> newKey(
        new KeyItem(fullKey, currentParent, parent->model()));
    parent->append(newKey);
    return;
  }

  QByteArray firstNamespaceName =
      notProcessedKeyPart.mid(0, indexOfNaspaceSeparator);

  QSharedPointer<AbstractNamespaceItem> namespaceItem =
      parent->findChildNamespace(firstNamespaceName);

  if (namespaceItem.isNull()) {
    long nsPos =
        fullKey.size() - notProcessedKeyPart.size() + firstNamespaceName.size();
    QByteArray namespaceFullPath = fullKey.mid(0, nsPos);

    // Single namespaced key
    if (nextKey.isEmpty() || nextKey.indexOf(namespaceFullPath) == -1) {
        QSharedPointer<KeyItem> newKey(
            new KeyItem(fullKey, currentParent, parent->model()));
        parent->append(newKey);
        return;
    }

    namespaceItem = QSharedPointer<NamespaceItem>(
        new NamespaceItem(namespaceFullPath, m_operations, currentParent,
                          parent->model(), settings.dbIndex, settings.filter));

    if (expandedNamespaces.contains(namespaceFullPath)) {
      namespaceItem->setExpanded(true);
    }

    parent->appendNamespace(namespaceItem);
  }

  renderLazily(namespaceItem,
               notProcessedKeyPart.mid(indexOfNaspaceSeparator +
                                       settings.nsSeparator.length()),
               fullKey, m_operations, settings, expandedNamespaces, level + 1, nextKey);
}
