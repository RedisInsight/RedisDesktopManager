#pragma once
#include "abstractnamespaceitem.h"

namespace ConnectionsTree {

class NamespaceItem : public QObject, public AbstractNamespaceItem
{
     Q_OBJECT

public:
    NamespaceItem(const QByteArray& fullPath,                  
                  QSharedPointer<Operations> operations,
                  QWeakPointer<TreeItem> parent,
                  Model& model,
                  uint dbIndex);

    QString getDisplayName() const override;    

    QByteArray getName() const override;

    QByteArray getFullPath() const;

    QString getIconUrl() const override;

    QString getType() const override { return "namespace"; }

    int itemDepth() const override;

    bool isEnabled() const override;        

    void setRemoved();

private:
    QByteArray m_fullPath;    
    QByteArray m_displayName;
    bool m_removed;    
};
}
