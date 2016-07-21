#include "keyitem.h"
#include <QMenu>
#include <QMessageBox>
#include <qredisclient/utils/text.h>

#include "connections-tree/iconproxy.h"
#include "connections-tree/utils.h"

using namespace ConnectionsTree;

KeyItem::KeyItem(const QByteArray &fullPath, unsigned short dbIndex,
                 QSharedPointer<Operations> operations,
                 QWeakPointer<TreeItem> parent,
                 Model& model)
    : TreeItem(model),
      m_fullPath(fullPath),
      m_dbIndex(dbIndex),
      m_operations(operations),
      m_parent(parent),      
      m_removed(false)
{
}

QString KeyItem::getDisplayName() const
{
    return printableString(m_fullPath);
}

QIcon KeyItem::getIcon() const
{
    return IconProxy::instance()->get(":/images/key.png");
}

QList<QSharedPointer<TreeItem>> KeyItem::getAllChilds() const
{
    return QList<QSharedPointer<TreeItem>>();
}

bool KeyItem::supportChildItems() const
{
    return false;
}

uint KeyItem::childCount(bool) const
{
    return (uint)0;
}

QSharedPointer<TreeItem> KeyItem::child(uint row) const
{
    Q_UNUSED(row);

    return QSharedPointer<TreeItem>();
}

QWeakPointer<TreeItem> KeyItem::parent() const
{
    return m_parent;
}

bool KeyItem::onClick(ParentView&)
{
    if (isEnabled())
        m_operations->openKeyTab(*this, false);

    return false;
}

void KeyItem::onWheelClick(ParentView&)
{    
    if (isEnabled())
        m_operations->openKeyTab(*this, true);
}

QSharedPointer<QMenu> KeyItem::getContextMenu(ParentView& treeview)
{
    QSharedPointer<QMenu> menu(new QMenu());

    if (!isEnabled())
        return menu;

    if (!m_signalReciever) {
        m_signalReciever = QSharedPointer<QObject>(new QObject());
    }

    menu->addAction(createMenuAction(":/images/add.png", "Open key", menu.data(), m_signalReciever.data(),
                                     [this] { m_operations->openKeyTab(*this, false); }));

    menu->addAction(createMenuAction(":/images/add.png", "Open key value in new tab", menu.data(), m_signalReciever.data(),
                                     [this] { m_operations->openKeyTab(*this, true); }));


    std::function<void()> deleteKeyItemCallback = [this, &treeview]()
    {
        confirmAction(treeview.getParentWidget(),
                      QObject::tr("Do you really want to delete this key?"),
                      [this, &treeview]()
        {
            m_operations->deleteDbKey(*this, [&treeview](const QString& error){
                QMessageBox::warning(treeview.getParentWidget(), QObject::tr("Key error"), error);
            });
        });
    };
    menu->addAction(createMenuAction(":/images/delete.png", "Remove key",
                                     menu.data(), m_signalReciever.data(), deleteKeyItemCallback));

    return menu;
}

bool KeyItem::isLocked() const
{
    return false;
}

bool KeyItem::isEnabled() const
{
    if (!m_removed && m_parent) {
        return m_parent.toStrongRef()->isEnabled();
    } else {
        return m_removed == false;
    }
}

QByteArray KeyItem::getFullPath() const
{
    return m_fullPath;
}

int KeyItem::getDbIndex() const
{
    return m_dbIndex;
}

void KeyItem::setRemoved()
{
    m_removed = true;
}
