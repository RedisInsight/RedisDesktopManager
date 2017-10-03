#include "namespaceitem.h"
#include <QMenu>
#include <QMessageBox>
#include "connections-tree/utils.h"
#include "connections-tree/model.h"
#include "databaseitem.h"
#include "keyitem.h"

using namespace ConnectionsTree;

NamespaceItem::NamespaceItem(const QByteArray &fullPath,
                             QSharedPointer<Operations> operations,
                             QWeakPointer<TreeItem> parent,
                             Model &model, uint dbIndex)
    : AbstractNamespaceItem(model, parent, operations, dbIndex),
      m_fullPath(fullPath),
      m_removed(false)
{
    m_displayName = m_fullPath.mid(m_fullPath.lastIndexOf(m_operations->getNamespaceSeparator()) + 1);

    m_eventHandlers.insert("click", [this]() {
        setExpanded(true);
        notifyModel();
    });

    m_eventHandlers.insert("delete", [this]() {
        m_operations->deleteDbNamespace(*this);
    });
}

QString NamespaceItem::getDisplayName() const
{    
    return QString("%1 (%2)").arg(QString::fromUtf8(m_displayName)).arg(childCount(true));
}

QByteArray NamespaceItem::getName() const
{
    return m_displayName;
}

QString NamespaceItem::getIconUrl() const
{    
    return QString("qrc:/images/namespace.svg");
}

int NamespaceItem::itemDepth() const {
    return m_fullPath.count(m_operations->getNamespaceSeparator().toUtf8()) + 2;
}

bool NamespaceItem::isLocked() const
{
    return false;
}

bool NamespaceItem::isEnabled() const
{
    return m_removed == false;
}

QByteArray NamespaceItem::getFullPath() const
{
    return m_fullPath;
}

void NamespaceItem::setRemoved()
{
   m_removed = true;
}
