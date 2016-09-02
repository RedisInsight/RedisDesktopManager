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
                             QWeakPointer<TreeItem> parent, Model &model,
                             const KeysTreeRenderer::RenderingSettigns& settings)
    : AbstractNamespaceItem(model, parent, operations, settings),
      m_fullPath(fullPath),
      m_removed(false)
{
    m_displayName = m_fullPath.mid(m_fullPath.lastIndexOf(settings.nsSeparator) + 1);

    m_eventHandlers.insert("click", [this]() {
       if (m_childItems.size() == 0 && m_rawChilds.size() > 0) {
        renderChilds();
       }
    });


    m_eventHandlers.insert("delete", [this]() {
        m_operations->deleteDbNamespace(*this);
    });

}

QString NamespaceItem::getDisplayName() const
{    
    return QString("%1 (%2)").arg(m_displayName).arg(childCount(true));
}

QString NamespaceItem::getDisplayPart() const
{
    return m_displayName;
}

QByteArray NamespaceItem::getName() const
{
    return m_fullPath;
}

QString NamespaceItem::getIconUrl() const
{    
    return QString("qrc:/images/namespace.svg");
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
    return m_fullPath;
}

int NamespaceItem::getDbIndex() const
{
    return m_renderingSettings.dbIndex;
}

void NamespaceItem::setRemoved()
{
   m_removed = true;
}
