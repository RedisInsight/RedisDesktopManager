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

    bool isEnabled() const override;    

    void notifyModel() override;

    void loadKeys(std::function<void()> callback=std::function<void()>());

    void unload();

    QVariant metadata(const QString&) override;

    void setMetadata(const QString&, QVariant) override;

protected:
    void reload();
    void liveUpdate();
    void filterKeys(const QRegExp& filter);
    void resetFilter();    

private:
    void showLoadingError(const QString& err);

private:    
    unsigned int m_keysCount;    
    QTimer m_liveUpdateTimer;
};

}
