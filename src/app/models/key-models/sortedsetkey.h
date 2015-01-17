#ifndef SORTEDSETKEYMODEL_H
#define SORTEDSETKEYMODEL_H

#include "abstractkey.h"

class SortedSetKeyModel : public KeyModel
{
    Q_OBJECT

public:
    SortedSetKeyModel(QSharedPointer<RedisClient::Connection> connection, QString fullPath, int dbIndex, int ttl);

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
    enum Roles { Value = Qt::UserRole + 1, BinaryValue, Score, RowNumber};

    QList<QPair<QByteArray, double>> m_rowsCache;

    void loadRowCount();
    void addSortedSetRow(const QString& value, double score);
    void deleteSortedSetRow(const QString& value);
};

#endif // SORTEDSETKEYMODEL_H
