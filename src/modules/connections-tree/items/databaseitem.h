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
                 QWeakPointer<TreeItem> parent, Model& model);

    ~DatabaseItem();

    QByteArray getName() const override;

    QByteArray getFullPath() const override;

    QString getDisplayName() const override;

    QString getIconUrl() const override;

    QString getType() const override { return "database"; }

    int itemDepth() const override { return 1; }

    bool isLocked() const override;

    bool isEnabled() const override;    

    void notifyModel() override;

    void loadKeys();

    void unload(bool removeRawKeys=true);

    int getIndex() const;

    QVariant metadata(const QString&) override;

    void setMetadata(const QString&, QVariant) override;

protected:
    void reload();
    void liveUpdate();
    void filterKeys(const QRegExp& filter);
    void resetFilter();    

private:
    unsigned short int m_index;
    unsigned int m_keysCount;
    bool m_locked;
    QTimer m_liveUpdateTimer;
};

}
