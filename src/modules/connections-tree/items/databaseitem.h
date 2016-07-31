#pragma once
#include "abstractnamespaceitem.h"

namespace ConnectionsTree {

class ServerItem;

class DatabaseItem : public QObject, public AbstractNamespaceItem
{
    Q_OBJECT
public:
    DatabaseItem(unsigned int index, int keysCount,
                 QSharedPointer<Operations> operations,
                 QWeakPointer<TreeItem> parent,
                 Model& model);

    ~DatabaseItem();

    QString getName() const override;
    QByteArray getFullPath() const override;
    QString getDisplayName() const override;
    QIcon getIcon() const override;              

    bool onClick(ParentView& treeView) override;   
    QSharedPointer<QMenu> getContextMenu(ParentView& treeView) override;

    bool isLocked() const override;
    bool isEnabled() const override;

    void notifyModel() override;

    void loadKeys();
    void unload(bool removeRawKeys=true);
    int getIndex() const;

protected:
    void reload();
    void filterKeys(const QRegExp& filter);
    void resetFilter();    

private:
    unsigned short int m_index;
    unsigned int m_keysCount;
    bool m_locked;            
    ParentView* m_parentView;    
};

}
