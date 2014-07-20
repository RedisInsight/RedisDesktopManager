#ifndef ICONPROXY_H
#define ICONPROXY_H

#include <QIcon>
#include <QString>
#include <QSharedPointer>
#include <QHash>

namespace ConnectionsTree {

    class IconProxy
    {
    public:
        static QSharedPointer<IconProxy> instance();

        QIcon get(const QString& qrcPath);

    private:
        static QSharedPointer<IconProxy> m_instance;
        QHash<QString, QIcon> m_cache;

    private:
        IconProxy();
    };

}

#endif // ICONPROXY_H
