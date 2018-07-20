#pragma once
#include <QAbstractListModel>
#include <QSharedPointer>
#include <QVariantMap>
#include "keymodel.h"
#include "common/baselistmodel.h"

namespace ValueEditor {

class ValueViewModel : public BaseListModel
{
    Q_OBJECT

public:
    ValueViewModel(Model &model);
    ~ValueViewModel() {}

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const;
    QHash<int, QByteArray> roleNames() const;

public:
    // single row operations
    Q_INVOKABLE bool isRowLoaded(int i);
    Q_INVOKABLE void addRow(const QVariantMap& row);
    Q_INVOKABLE void updateRow(int i, const QVariantMap& row);
    Q_INVOKABLE void deleteRow(int i);
    Q_INVOKABLE QVariantMap getRow(int i);
    
    // multi row operations
    Q_INVOKABLE void loadRows(int start, int limit);
    Q_INVOKABLE bool isMultiRow();
    Q_INVOKABLE void reload();
    Q_INVOKABLE int totalRowCount();
    Q_INVOKABLE int pageSize();

    // general operations    
    Q_INVOKABLE QVariantList getColumnNames();    

signals:
    void rowsLoaded(int start, int count);
    void error(QString error);

protected:    
    int mapRowIndex(int i);    

private:
    ValueEditor::Model& m_model;
    int m_startFramePosition;
    int m_lastLoadedRowFrameSize;

};

}
