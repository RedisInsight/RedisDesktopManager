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
                  const KeysTreeRenderer::RenderingSettigns& settings);

    QString getDisplayName() const override;

    QString getDisplayPart() const;

    QByteArray getName() const override;

    QByteArray getFullPath() const;

    QString getIconUrl() const override;

    QString getType() const override { return "namespace"; }

    bool isLocked() const override;

    bool isEnabled() const override;

    void notifyModel() override;

    int getDbIndex() const;

    void setRemoved();

private:
    QByteArray m_fullPath;    
    QString m_displayName;    
    bool m_removed;
};
}
