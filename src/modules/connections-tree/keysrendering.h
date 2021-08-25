#pragma once
#include <QRegExp>
#include <QString>
#include <QSharedPointer>
#include <QtConcurrent>
#include <qredisclient/connection.h>

namespace ConnectionsTree {

    class Operations;
    class AbstractNamespaceItem;
    class Model;

    QSharedPointer<AbstractNamespaceItem> resolveRootItem(QSharedPointer<AbstractNamespaceItem> item);

    class KeysTreeRenderer
    {
    public:
        struct RenderingSettigns {
            QRegExp filter;
            QString nsSeparator;
            uint dbIndex;            
            uint renderLimit;            
            bool appendNewItems;
            bool checkPreRenderedItems;
            bool shortKeysRendering;
        };

    public:
        static void renderKeys(QSharedPointer<Operations> operations,
                               RedisClient::Connection::RawKeysList keys,
                               QSharedPointer<AbstractNamespaceItem> parent,
                               RenderingSettigns settings,
                               const QSet<QByteArray> &expandedNamespaces);                

    private:
        static void renderLazily(QSharedPointer<AbstractNamespaceItem> root,
                                 QSharedPointer<AbstractNamespaceItem> parent,
                                 const QByteArray &notProcessedKeyPart,
                                 const QByteArray &fullKey,
                                 QSharedPointer<Operations> operations,
                                 const RenderingSettigns& settings,
                                 const QSet<QByteArray> &expandedNamespaces,
                                 unsigned long level=0,
                                 const QByteArray &nextKey=QByteArray());
    };

}
