#include "namespaceitem.h"

#include <QMenu>
#include "connections-tree/iconproxy.h"
#include "connections-tree/utils.h"
#include "connections-tree/model.h"
#include "databaseitem.h"
#include "keyitem.h"

using namespace ConnectionsTree;

NamespaceItem::NamespaceItem(const QString& fullPath,
                             QSharedPointer<Operations> operations,
                             QWeakPointer<TreeItem> parent, Model &model,
                             const KeysTreeRenderer::RenderingSettigns& settings)
    : AbstractNamespaceItem(model, parent, operations, settings),
      m_fullPath(fullPath),
      m_removed(false)
{
    m_displayName = m_fullPath.mid(m_fullPath.lastIndexOf(settings.nsSeparator)+1);   
}

QString NamespaceItem::getDisplayName() const
{    
    return QString("%1 (%2)").arg(m_displayName).arg(childCount(true));
}

QString NamespaceItem::getName() const
{
    return m_displayName;
}

QIcon NamespaceItem::getIcon() const
{    
    return IconProxy::instance()->get(":/images/namespace.png");
}

bool NamespaceItem::onClick(TreeItem::ParentView&)
{    
    if (m_childItems.size() == 0 && m_rawChilds.size() > 0) {
        renderChilds();
    }
    return true;
}

QSharedPointer<QMenu> NamespaceItem::getContextMenu(TreeItem::ParentView&)
{
    QSharedPointer<QMenu> menu(new QMenu());

    if (!isEnabled())
        return menu;

    menu->addAction(createMenuAction(":/images/delete.png", "Delete namespace",
                                     menu.data(), menu.data(),
                                     [this](){ m_operations->deleteDbNamespace(*this); }));

    return menu;
}

bool NamespaceItem::isLocked() const
{
    return false;
}

bool NamespaceItem::isEnabled() const
{
    return m_removed == false;
}

void NamespaceItem::notifyModel()
{
    m_rawChilds.clear();
    AbstractNamespaceItem::notifyModel();
}

QByteArray NamespaceItem::getFullPath() const
{
    return m_fullPath.toUtf8();
}

int NamespaceItem::getDbIndex() const
{
    return m_renderingSettings.dbIndex;
}

void NamespaceItem::setRemoved()
{
    m_removed = true;
}

