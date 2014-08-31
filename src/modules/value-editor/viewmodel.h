#pragma once

#include <QAbstractListModel>
#include <QString>
#include "model.h"

namespace ValueEditor {

class ViewModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QString keyName READ keyName WRITE setKeyName NOTIFY keyNameChanged)
    Q_PROPERTY(int keyTTL READ keyTTL WRITE setKeyTTL NOTIFY keyTTLChanged)
    Q_PROPERTY(QString keyType READ keyType)
    Q_PROPERTY(QString state READ state NOTIFY stateChanged)
    Q_PROPERTY(bool showValueNavigation READ showValueNavigation NOTIFY showValueNavigationChanged)
    Q_PROPERTY(QStringList columnNames READ columnNames NOTIFY columnNamesChanged)
    Q_PROPERTY(int currentPage READ currentPage WRITE setCurrentPage NOTIFY currentPageChanged)
    Q_PROPERTY(int pageCount READ pageCount NOTIFY pageCountChanged)

public:
    ViewModel(QSharedPointer<Model>);

    QString keyName();
    int keyTTL();
    QString keyType();
    QString state() const;
    bool showValueNavigation() const;
    QStringList columnNames() const;
    int currentPage() const;
    int pageCount() const;

    void setKeyName(const QString& name);
    void setKeyTTL(int ttl);
    void setCurrentPage(int page);

    QModelIndex index(int row, int column = 0, const QModelIndex& parent = QModelIndex()) const;
    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex& index, const QVariant& value, int role);

public slots:
    void removeKey();
    void removeRow(int index);
    void selectCell(int index, int role);

    void reloadCurrentPage();
    void reloadCurrentValue();

    // ???
    void updateCurrentValue(int role, QString value) {}

signals:
    void keyNameChanged();
    void keyTTLChanged();
    void stateChanged();
    void showValueNavigationChanged();
    void columnNamesChanged();
    void currentPageChanged();
    void pageCountChanged();

private:
    QSharedPointer<Model> m_model;
    int m_currentPage;
    int m_pageCount;
};

}
