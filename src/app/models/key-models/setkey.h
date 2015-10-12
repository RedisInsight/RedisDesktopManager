#pragma once
#include "listlikekey.h"

class SetKeyModel : public ListLikeKeyModel
{
public:
    SetKeyModel(QSharedPointer<RedisClient::Connection> connection,
                QByteArray fullPath, int dbIndex, long long ttl);

    QString getType() override;

    void addRow(const QVariantMap&) override;
    virtual void updateRow(int rowIndex, const QVariantMap &) override;
    void removeRow(int) override;

private:    
    void addSetRow(const QByteArray &value);
    RedisClient::Response deleteSetRow(const QByteArray &value);    
};

