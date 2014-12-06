#pragma once

#include <QObject>
#include "abstractkey.h"

class StringKeyModel : public KeyModel
{
    Q_OBJECT

public:
    StringKeyModel(QSharedPointer<RedisClient::Connection> connection, QString fullPath, int dbIndex, int ttl);

    QString getType() override;
    QStringList getColumnNames() override;
    QHash<int, QByteArray> getRoles() override;
    QVariant getData(int rowIndex, int dataRole) override;
    virtual void setData(int rowIndex, int dataRole, QString value) override;

    void addRow(const QVariantMap&) override;
    unsigned long rowsCount() override;
    void loadRows(unsigned long rowStart, unsigned long count, std::function<void()> callback) override;
    void clearRowCache() override;
    void removeRow(int) override;
    bool isRowLoaded(int) override;
    bool isMultiRow() const override;

private:
    enum Roles { Value = Qt::UserRole + 1};

    bool loadValue();

    QByteArray m_value;
};

