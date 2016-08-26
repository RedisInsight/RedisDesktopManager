#pragma once
#include "abstractkey.h"

class StringKeyModel : public KeyModel<QByteArray>
{    
public:
    StringKeyModel(QSharedPointer<RedisClient::Connection> connection,
                   QByteArray fullPath, int dbIndex, long long ttl);

    QString getType() override;
    QStringList getColumnNames() override;
    QHash<int, QByteArray> getRoles() override;
    QVariant getData(int rowIndex, int dataRole) override;

    void addRow(const QVariantMap&) override;
    virtual void updateRow(int rowIndex, const QVariantMap& row) override;
    void loadRows(unsigned long, unsigned long, std::function<void(const QString&)> callback) override;
    void removeRow(int) override;       

protected:
    void addLoadedRowsToCache(const QVariantList&, int) override {}

private:
    enum Roles { Value = Qt::UserRole + 1 };    
};

