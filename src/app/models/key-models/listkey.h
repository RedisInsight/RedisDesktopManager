#pragma once
#include "listlikekey.h"
#include <QByteArray>

class ListKeyModel : public ListLikeKeyModel
{
public:
    ListKeyModel(QSharedPointer<RedisClient::Connection> connection,
                 QByteArray fullPath, int dbIndex, long long ttl);

    QString getType() override;

    void addRow(const QVariantMap&) override;
    virtual void updateRow(int rowIndex, const QVariantMap &) override;
    void removeRow(int) override;

private:       
    bool isActualPositionChanged(int row);
    void addListRow(const QByteArray &value);
    void setListRow(int pos, const QByteArray &value);
    void deleteListRow(int count, const QByteArray &value);
};
