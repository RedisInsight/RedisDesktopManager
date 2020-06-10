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

    class KeysTreeRenderer
    {
    public:
        struct RenderingSettigns {
            QRegExp filter;
            QString nsSeparator;
            uint dbIndex;
            bool sortKeys;
        };

    public:
        static void renderKeys(QSharedPointer<Operations> operations,
                               RedisClient::Connection::RawKeysList keys,
                               QSharedPointer<AbstractNamespaceItem> parent,
                               RenderingSettigns settings,
                               const QSet<QByteArray> &expandedNamespaces);

    private:
        static void renderLazily(QSharedPointer<AbstractNamespaceItem> parent,
                                 const QByteArray &notProcessedKeyPart,
                                 const QByteArray &fullKey,
                                 QSharedPointer<Operations> operations,
                                 const RenderingSettigns& settings,
                                 const QSet<QByteArray> &expandedNamespaces,
                                 unsigned long level=0,
                                 const QByteArray &nextKey=QByteArray());
    };

}
