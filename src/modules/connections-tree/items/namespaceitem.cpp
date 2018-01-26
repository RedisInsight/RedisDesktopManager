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

        if (m_childItems.size() == 0) {
            QString nsFilter = QString("%1%2*").arg(QString::fromUtf8(m_fullPath)).arg(m_operations->getNamespaceSeparator());

            qDebug() << "NS Filter" << nsFilter;

            lock();
            emit m_model.itemChanged(getSelf());

            m_operations->loadNamespaceItems(qSharedPointerDynamicCast<AbstractNamespaceItem>(getSelf()),
                                             nsFilter, [this](const QString& err) {
                unlock();
                if (!err.isEmpty())
                    return showLoadingError(err);

                setExpanded(true);
                emit m_model.itemChanged(getSelf());
                emit m_model.expandItem(getSelf());
            });
        } else {
            setExpanded(true);
            emit m_model.itemChanged(getSelf());
            emit m_model.expandItem(getSelf());
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
    if (isLocked()) return QString("qrc:/images/wait.svg");
    return QString("qrc:/images/namespace.svg");
}

int NamespaceItem::itemDepth() const {
    return m_fullPath.count(m_operations->getNamespaceSeparator().toUtf8()) + 2;
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
