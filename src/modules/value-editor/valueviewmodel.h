#pragma once
#include <QAbstractListModel>
#include <QSharedPointer>
#include <QVariantMap>
#include "keymodel.h"

namespace ValueEditor {

class LargeTextWrappingModel : public QAbstractListModel
{
    Q_OBJECT
public:
    LargeTextWrappingModel(const QString& text=QString(), uint chunkSize=1000)
        : m_text(text), m_chunkSize(chunkSize)
    {
    }

    ~LargeTextWrappingModel() {
        qDebug() << "{!!DEL!!} Remove text wrapping model";
    }

    QHash<int, QByteArray> roleNames() const
    {
        QHash<int, QByteArray> roles;
        roles[Qt::UserRole + 1] = "value";
        return roles;
    }

    int rowCount(const QModelIndex &parent = QModelIndex()) const override
    {
        return m_text.size() / m_chunkSize + 1;
    }

    QVariant data(const QModelIndex &index, int role) const
    {
        if (!isIndexValid(index))
            return QVariant();

        if (role == Qt::UserRole + 1) {
            return m_text.mid(index.row() * m_chunkSize, m_chunkSize);
        }

        return QVariant();
    }

public slots:
    void cleanUp()
    {
        emit beginRemoveRows(QModelIndex(), 0, rowCount());
        m_text.clear();
        emit endRemoveRows();        
    }

private:
    bool isIndexValid(const QModelIndex &index) const {
         return 0 <= index.row() && index.row() < rowCount();
    }

private:
    QString m_text;
    uint m_chunkSize;
};

class ValueViewModel : public QAbstractListModel
{
    Q_OBJECT

public:
    ValueViewModel(Model &model);
    ~ValueViewModel()
    {
            qDebug() << "{!!DEL!!} Remove ValueViewModel model";
    }

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
    Q_INVOKABLE void loadRows(int start, int count);
    Q_INVOKABLE bool isMultiRow();
    Q_INVOKABLE void reload();
    Q_INVOKABLE int totalRowCount();
    Q_INVOKABLE int pageSize();

    // general operations    
    Q_INVOKABLE QVariantList getColumnNames();

    Q_INVOKABLE QObject* wrapLargeText(const QByteArray &text)
    {
        auto w = new LargeTextWrappingModel(QString::fromUtf8(text));        
        w->setParent(this);
        return w;
    }

signals:
    void rowsLoaded(int start, int count);
    void error(QString error);

private:
    ValueEditor::Model& m_model;

protected:
    bool isIndexValid(const QModelIndex &index) const;
    int mapRowIndex(int i);
    QVariantMap getRowRaw(int row);

private:
    int m_startFramePosition;
    int m_lastLoadedRowFrameSize;

};

}
