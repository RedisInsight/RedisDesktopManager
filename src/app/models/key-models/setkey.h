#pragma once

#include <QObject>
#include "abstractkey.h"

class SetKeyModel : public ListLikeKeyModel
{
    Q_OBJECT

public:
    SetKeyModel(QSharedPointer<RedisClient::Connection> connection,
                QString fullPath, int dbIndex, int ttl);

    QString getType() override;        
    virtual void updateRow(int rowIndex, const QVariantMap &) override;

    void addRow(const QVariantMap&) override;    
    void loadRows(unsigned long rowStart, unsigned long count, std::function<void()> callback) override;
    void removeRow(int) override;

private:
    void loadRowCount();

    void addSetRow(const QString& value);
    void deleteSetRow(const QString& value);
};

