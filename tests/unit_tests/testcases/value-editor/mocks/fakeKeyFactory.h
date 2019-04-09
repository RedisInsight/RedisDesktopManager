#include <QJSValue>
#include <QTest>
#include "app/models/key-models/newkeyrequest.h"
#include "modules/value-editor/abstractkeyfactory.h"
#include "modules/value-editor/keymodel.h"

class FakeKeyModel : public ValueEditor::Model {
 public:
  FakeKeyModel() : m_notifier(new ValueEditor::ModelSignals()) {}

  QString getKeyName() override { return QString("fake"); }

  QString getKeyTitle() override { return QString("fake"); }

  QString type() override { return QString("fake"); }

  long long getTTL() override { return -1; }

  QStringList getColumnNames() override { return QStringList() << "value"; }

  QHash<int, QByteArray> getRoles() override {
    QHash<int, QByteArray> result;
    result[Qt::UserRole + 1] = "value";
    return result;
  }

  QVariant getData(int rowIndex, int) override {
    if (rowIndex != 0) return QVariant();

    return QVariant(QString("fake value"));
  }

  void setKeyName(const QByteArray &, Callback) override {}

  void setTTL(const long long, Callback) override {}

  void removeKey(Callback) override {}

  void addRow(const QVariantMap &, Callback) override {}

  void updateRow(int, const QVariantMap &, Callback) override {}

  unsigned long rowsCount() override { return 1; }

  void loadRows(QVariant, unsigned long,
                std::function<void(const QString &, unsigned long)>) override {}

  void clearRowCache() override {}

  void removeRow(int, Callback) override {}

  bool isRowLoaded(int) override { return true; }

  bool isMultiRow() const override { return true; }

  QSharedPointer<ValueEditor::ModelSignals> getConnector() const override {
    return m_notifier;
  }

  QSharedPointer<RedisClient::Connection> getConnection() const override {
    return QSharedPointer<RedisClient::Connection>();
  }

  unsigned int dbIndex() const override { return 0u; }

  void loadRowsCount(Callback) override {}

 private:
  QSharedPointer<ValueEditor::ModelSignals> m_notifier;
};

class FakeKeyFactory : public ValueEditor::AbstractKeyFactory {
 public:
  void loadKey(
      QSharedPointer<RedisClient::Connection> connection,
      QByteArray keyFullPath, int dbIndex,
      std::function<void(QSharedPointer<ValueEditor::Model>, const QString &)>
          callback) override {
    loadKeyCalled++;
    QVERIFY(connection.isNull() == false);
    QVERIFY(keyFullPath.isEmpty() == false);
    QVERIFY(dbIndex >= 0);
    callback(QSharedPointer<ValueEditor::Model>(new FakeKeyModel()), QString());
  }

  void submitNewKeyRequest(NewKeyRequest r, QJSValue jsCallback) {
    submitNewKeyRequestCalled++;
  }

  uint loadKeyCalled;
  uint submitNewKeyRequestCalled;
};
