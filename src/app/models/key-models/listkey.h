#pragma once

#include "abstractkey.h"
#include <QByteArray>

class ListKeyModel : public ListLikeKeyModel
{
    Q_OBJECT

public:
    ListKeyModel(QSharedPointer<RedisClient::Connection> connection, QString fullPath, int dbIndex, int ttl);

    QString getType() override;            
    virtual void setData(int rowIndex, int dataRole, QString value) override;

    void addRow(const QVariantMap&) override;    
    void loadRows(unsigned long rowStart, unsigned long count, std::function<void()> callback) override;    
    void removeRow(int) override;

private:   
    void loadRowCount();
};
