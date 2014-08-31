#include "iconproxy.h"

using namespace ConnectionsTree;

QSharedPointer<IconProxy> IconProxy::m_instance = QSharedPointer<IconProxy>();

IconProxy::IconProxy()
{
}

QSharedPointer<IconProxy> IconProxy::instance()
{
    if (m_instance.isNull())
        m_instance = QSharedPointer<IconProxy>(new IconProxy());

    return m_instance;
}

QIcon IconProxy::get(const QString &qrcPath)
{
    if (!m_cache.contains(qrcPath)) {
        m_cache[qrcPath] = QIcon(qrcPath);
    }

    return m_cache[qrcPath];
}
