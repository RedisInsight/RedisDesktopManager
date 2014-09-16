#pragma once

#include <QObject>
#include "abstractkey.h"

class SetKeyModel : public KeyModel
{
    Q_OBJECT

public:
    SetKeyModel(QSharedPointer<RedisClient::Connection> connection, QString fullPath, int dbIndex, int ttl);

    QString getType() override;
    QStringList getColumnNames() override;
    QHash<int, QByteArray> getRoles() override;
    QString getData(int rowIndex, int dataRole) override;
    virtual void setData(int rowIndex, int dataRole, QString value) override;

    void addRow(/* ??? */) override;
    unsigned long rowsCount() override;
    void loadRows(unsigned long rowStart, unsigned long count, std::function<void()> callback) override;
    void clearRowCache() override;
    void removeRow(int) override;
    bool isRowLoaded(int) override;
    bool isMultiRow() const override;
};

