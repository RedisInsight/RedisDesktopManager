#pragma once
#include "listlikekey.h"

class SetKeyModel : public ListLikeKeyModel
{
public:
    SetKeyModel(QSharedPointer<RedisClient::Connection> connection,
                QByteArray fullPath, int dbIndex, long long ttl);

    QString type() override;

    void addRow(const QVariantMap&, Callback) override;
    virtual void updateRow(int rowIndex, const QVariantMap &, Callback) override;
    void removeRow(int, Callback) override;

private:    
    void addSetRow(const QByteArray &value);
    RedisClient::Response deleteSetRow(const QByteArray &value);    
};

