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

    QByteArray getName() const override;

    QByteArray getFullPath() const;

    QString getIconUrl() const override;

    QString getType() const override { return "namespace"; }

    int itemDepth() const override { return m_fullPath.count(m_renderingSettings.nsSeparator.toUtf8()) + 2; }

    bool isLocked() const override;

    bool isEnabled() const override;    

    int getDbIndex() const;

    void setRemoved();

private:
    QByteArray m_fullPath;    
    QByteArray m_displayName;
    bool m_removed;
    bool m_rendering;
};
}
