#include "KeysTreeRenderer.h"
#include "RedisKeyItem.h"
#include "RedisKeyNamespace.h"


QList<QStandardItem*> KeysTreeRenderer::renderKeys(RedisServerDbItem * db, QStringList rawKeys, const QRegExp & filter, const RedisServerDbItem::Icons & icons)
{
    rawKeys.sort();

    QList<QStandardItem*> result;

    for (QString rawKey : rawKeys) {

        //if filter enabled - skip keys
        if (!filter.isEmpty() && !rawKey.contains(filter)) {
            continue;
        }

        renderNamaspacedKey(result, nullptr, rawKey, rawKey, icons.namespaceIcon, icons.keyIcon, db);                        
    }    

    return result;
}

void KeysTreeRenderer::renderNamaspacedKey(QList<QStandardItem*> & root, QStandardItem * currItem, 
                                            QString notProcessedKeyPart, QString fullKey, const QIcon & namespaceIcon, 
                                            const QIcon & keyIcon, RedisServerDbItem * db)
{
    if (!notProcessedKeyPart.contains(":")) {
        RedisKeyItem * newKey = new RedisKeyItem();
        newKey->init(fullKey, db, keyIcon);    

        if (currItem == nullptr) {
            root.push_back(newKey);
        } else {
            currItem->appendRow(newKey);    
        }
        return;
    }

    int indexOfNaspaceSeparator = notProcessedKeyPart.indexOf(":");

    QString firstNamespaceName = notProcessedKeyPart.mid(0, indexOfNaspaceSeparator);

    QStandardItem * namespaceItem = nullptr;

    if (currItem == nullptr) {
        for (int i=0; i < root.size(); ++i)
        {
            QStandardItem * child = root[i];

            if (child->type() == RedisKeyNamespace::TYPE 
                && child->text() == firstNamespaceName) {
                    namespaceItem = child;
                    break;
            }
        }
    } else {
        for (int i=0; i < currItem->rowCount(); ++i)
        {
            QStandardItem * child = currItem->child(i);

            if (child->type() == RedisKeyNamespace::TYPE 
                && child->text() == firstNamespaceName) {
                    namespaceItem = child;
                    break;
            }
        }
    }



    if (namespaceItem == nullptr) {
        namespaceItem = new RedisKeyNamespace(firstNamespaceName, namespaceIcon);        

        if (currItem == nullptr) {
            root.push_back(namespaceItem);
        } else {
            currItem->appendRow(namespaceItem);    
        }
    }

    renderNamaspacedKey(root, namespaceItem, notProcessedKeyPart.mid(indexOfNaspaceSeparator+1), fullKey, namespaceIcon, keyIcon, db);        
}