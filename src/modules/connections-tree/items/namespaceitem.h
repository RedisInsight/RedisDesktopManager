#pragma once
#include "abstractnamespaceitem.h"

namespace ConnectionsTree {        

    class NamespaceItem : public QObject, public AbstractNamespaceItem
    {        
        Q_OBJECT
    public:
        NamespaceItem(const QString& fullPath,
                      QSharedPointer<Operations> operations,
                      QWeakPointer<TreeItem> parent,
                      Model& model,
                      const KeysTreeRenderer::RenderingSettigns& settings);

        QString getDisplayName() const override;
        QString getName() const override;
        QByteArray getFullPath() const override;
        QIcon getIcon() const override;       

        bool onClick(ParentView& treeView) override;
        QSharedPointer<QMenu> getContextMenu(ParentView& treeView) override;

        bool isLocked() const override;
        bool isEnabled() const override;

        void notifyModel() override;

        int getDbIndex() const;

        void setRemoved();

    private:
        QString m_fullPath;        
        QString m_displayName;                           
        bool m_removed;        
    };
}
