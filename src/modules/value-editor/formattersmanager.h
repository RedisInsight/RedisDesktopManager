#pragma once
#include <QAbstractListModel>
#include <QJSValue>

namespace ValueEditor {

class FormattersManager : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles { name = Qt::UserRole + 1, version, cmd };

public:
    FormattersManager();

    void loadFormatters(bool ignoreCache=false); // TODO make async with callback & invokable

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role) const;

    QHash<int, QByteArray> roleNames() const;

signals:
    void error(const QString& msg);

public:
    Q_INVOKABLE void decode(const QString& formatterName,
                                  const QByteArray& data,
                                  QJSValue jsCallback);

    Q_INVOKABLE void isValid(const QString& formatterName,
                                  const QByteArray& data,
                                  QJSValue jsCallback);

    Q_INVOKABLE void encode(const QString& formatterName,
                                  const QByteArray& data,
                                  QJSValue jsCallback);

    Q_INVOKABLE QStringList getPlainList();

    Q_INVOKABLE QString formattersPath();

private:
    void fillMapping();

private:
    QList<QVariantMap> m_formattersData;
    QHash<QString, int> m_mapping;
};

}
