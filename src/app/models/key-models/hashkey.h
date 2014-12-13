#ifndef HASHKEYMODEL_H
#define HASHKEYMODEL_H

#include "abstractkey.h"

class HashKeyModel : public KeyModel
{
    Q_OBJECT

public:
    HashKeyModel(QSharedPointer<RedisClient::Connection> connection, QString fullPath, int dbIndex, int ttl);

    QString getType() override;
    QStringList getColumnNames() override;
    QHash<int, QByteArray> getRoles() override;
    QVariant getData(int rowIndex, int dataRole) override;
    virtual void updateRow(int rowIndex, const QVariantMap &) override;

    void addRow(const QVariantMap&) override;
    unsigned long rowsCount() override;
    void loadRows(unsigned long rowStart, unsigned long count, std::function<void()> callback) override;
    void clearRowCache() override;
    void removeRow(int) override;
    bool isRowLoaded(int) override;
    bool isMultiRow() const override;

private:
    enum Roles { Key = Qt::UserRole + 1, Value, RowNumber};

    QHash<int, QPair<QByteArray, QByteArray>> m_rowsCache;

    void loadRowCount();

    void setHashRow(const QString& hashKey, const QString& hashValue);
    void deleteHashRow(const QString& hashKey);
};

#endif // HASHKEYMODEL_H
