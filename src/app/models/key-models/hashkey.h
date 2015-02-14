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
    enum Roles { Key = Qt::UserRole + 1, Value, BinaryValue, RowNumber};

    QList<QPair<QByteArray, QByteArray>> m_rowsCache;

    void loadRowCount();

    void setHashRow(const QByteArray &hashKey, const QByteArray &hashValue, bool updateIfNotExist = true);
    void deleteHashRow(const QByteArray& hashKey);

    void addLoadedRowsToCache(const QVariantList& list, int rowStart);
};

#endif // HASHKEYMODEL_H
