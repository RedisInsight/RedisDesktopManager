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
      m_removed(false),
      m_rendering(false)
{
    m_displayName = m_fullPath.mid(m_fullPath.lastIndexOf(settings.nsSeparator) + 1);

    m_eventHandlers.insert("click", [this]() {
        if (m_childItems.size() == 0) {
            m_rendering = true;
            m_model.itemChanged(getSelf());

            renderChilds();

            m_rendering = false;
            m_model.itemChanged(getSelf());
        }
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
    if (m_rendering) return QString("qrc:/images/wait.svg");
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
