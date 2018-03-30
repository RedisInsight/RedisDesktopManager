#include <QTest>
#include "modules/value-editor/abstractkeyfactory.h"
#include "modules/value-editor/keymodel.h"


class FakeKeyModel : public ValueEditor::Model
{
public:
    FakeKeyModel() :
        m_notifier(new ValueEditor::ModelSignals())
    {}

    QString getKeyName() override
    {
        return QString("fake");
    }

    QString getKeyTitle() override
    {
        return QString("fake");
    }

    QString getType() override
    {
        return QString("fake");
    }

    long long getTTL() override
    {
        return -1;
    }

    QStringList getColumnNames() override
    {
        return QStringList() << "value";
    }

    QHash<int, QByteArray> getRoles() override
    {
        QHash<int, QByteArray> result;
        result[Qt::UserRole + 1] = "value";
        return result;
    }

    QVariant getData(int rowIndex, int) override
    {
        if (rowIndex != 0)
            return QVariant();

        return QVariant(QString("fake value"));
    }

    void setKeyName(const QByteArray &) override
    {
    }

    void setTTL(const long long) override
    {
    }

    void removeKey() override
    {
    }

    void addRow(const QVariantMap &) override
    {
    }

    void updateRow(int, const QVariantMap &) override
    {
    }

    unsigned long rowsCount() override
    {
        return 1;
    }

    void loadRows(unsigned long, unsigned long, std::function<void (const QString &)>) override
    {
    }

    void clearRowCache() override
    {
    }

    void removeRow(int) override
    {
    }

    bool isRowLoaded(int) override
    {
        return true;
    }

    bool isMultiRow() const override
    {
        return true;
    }

    QSharedPointer<ValueEditor::ModelSignals> getConnector() const override
    {
        return m_notifier;
    }

    QSharedPointer<RedisClient::Connection> getConnection() const override
    {
        return QSharedPointer<RedisClient::Connection>();
    }

    unsigned int dbIndex() const override
    {
        return 0u;
    }

private:
    QSharedPointer<ValueEditor::ModelSignals> m_notifier;
};


class FakeKeyFactory : public ValueEditor::AbstractKeyFactory
{
public:
    void loadKey(QSharedPointer<RedisClient::Connection> connection,
                 QByteArray keyFullPath, int dbIndex,
                 std::function<void(QSharedPointer<ValueEditor::Model>, const QString&)> callback) override
    {
        loadKeyCalled++;
        QVERIFY(connection.isNull() == false);
        QVERIFY(keyFullPath.isEmpty() == false);
        QVERIFY(dbIndex >= 0);
        callback(QSharedPointer<ValueEditor::Model>(new FakeKeyModel()), QString());
    }

    void addKey(QSharedPointer<RedisClient::Connection> connection,
                 QByteArray keyFullPath, int dbIndex, QString type,
                 const QVariantMap &row) override
    {
        addKeyCalled++;
        QVERIFY(connection.isNull() == false);
        QVERIFY(keyFullPath.isEmpty() == false);
        QVERIFY(dbIndex >= 0);
        QVERIFY(type.isEmpty() == false);
        QVERIFY(row.isEmpty() == false);
    }

    uint loadKeyCalled;
    uint addKeyCalled;
};
