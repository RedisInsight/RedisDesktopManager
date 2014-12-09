#pragma once
#include <QAbstractListModel>
#include <QSharedPointer>
#include <QVariantMap>
#include "keymodel.h"

namespace ValueEditor {

class ValueViewModel : public QAbstractListModel
{
    Q_OBJECT

public:
    ValueViewModel(QSharedPointer<ValueEditor::Model> model = QSharedPointer<ValueEditor::Model>());

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role) const;

    QHash<int, QByteArray> roleNames() const;

public:
    Q_INVOKABLE bool isPartialLoadingSupported();
    Q_INVOKABLE QVariantList getColumnNames();
    Q_INVOKABLE bool isMultiRow();
    Q_INVOKABLE bool isRowLoaded(int i);
    Q_INVOKABLE void loadRows(int start, int count);
    Q_INVOKABLE void clearRowCache();
    Q_INVOKABLE void addRow(const QVariantMap&row);
    Q_INVOKABLE int totalRowCount();
    Q_INVOKABLE QVariantMap get(int i, bool relative = false);

signals:
    void rowsLoaded(int start, int count);

private:
    QSharedPointer<ValueEditor::Model> m_model;

protected:
    bool isIndexValid(const QModelIndex &index) const;

private:
    int m_startFramePosition;
    int m_lastLoadedRowFrameSize;

};

}
