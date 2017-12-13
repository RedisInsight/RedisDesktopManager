#pragma once
#include "abstractkey.h"

class HashKeyModel : public KeyModel<QPair<QByteArray, QByteArray>>
{
public:
    HashKeyModel(QSharedPointer<RedisClient::Connection> connection,
                 QByteArray fullPath, int dbIndex, long long ttl);

    QString getType() override;
    QStringList getColumnNames() override;
    QHash<int, QByteArray> getRoles() override;

    QVariant getData(int rowIndex, int dataRole) override;
    void addRow(const QVariantMap&) override;    
    virtual void updateRow(int rowIndex, const QVariantMap &) override;
    void removeRow(int) override;    

protected:
    void addLoadedRowsToCache(const QVariantList& list, int rowStart) override;

private:
    enum Roles { RowNumber = Qt::UserRole + 1, Key, Value };

    void setHashRow(const QByteArray &hashKey, const QByteArray &hashValue, bool updateIfNotExist = true);
    void deleteHashRow(const QByteArray& hashKey);
};
