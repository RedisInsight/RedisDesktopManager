#include "valueviewmodel.h"
#include <QDebug>

ValueEditor::ValueViewModel::ValueViewModel(QSharedPointer<ValueEditor::Model> model)
    : m_model(model), QAbstractListModel((QObject*)model.data()),
      m_startFramePosition(0),
      m_lastLoadedRowFrameSize(0)
{

}

int ValueEditor::ValueViewModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    Q_ASSERT(m_model.isNull() != true);

    return m_lastLoadedRowFrameSize;
}

QVariant ValueEditor::ValueViewModel::data(const QModelIndex &index, int role) const
{
    if (!isIndexValid(index))
        return QVariant();   

    return m_model->getData(m_startFramePosition + index.row(), role);
}

QHash<int, QByteArray> ValueEditor::ValueViewModel::roleNames() const
{
    return m_model->getRoles();
}

bool ValueEditor::ValueViewModel::isIndexValid(const QModelIndex &index) const
{
    return 0 <= index.row() && index.row() < rowCount();
}

bool ValueEditor::ValueViewModel::isPartialLoadingSupported()
{
    return m_model->isPartialLoadingSupported();
}

QVariantList ValueEditor::ValueViewModel::getColumnNames()
{
    QVariantList result;

    foreach (QString str, m_model->getColumnNames())
    {
        result.append(QVariant(str));
    }

    return result;
}

bool ValueEditor::ValueViewModel::isMultiRow()
{
    return m_model->isMultiRow();
}

bool ValueEditor::ValueViewModel::isRowLoaded(int i)
{
    return m_model->isRowLoaded(i);
}

void ValueEditor::ValueViewModel::loadRows(int start, int count)
{
    // frame already loaded
    if (m_model->isRowLoaded(start)) {
        m_startFramePosition = start;
        emit layoutAboutToBeChanged();
        emit layoutChanged();
        return;
    }

    m_model->loadRows(start, count, [this, start, count]()
    {                
        int loaded = totalRowCount() - start;
        loaded = (loaded > count) ? count : loaded;

        m_lastLoadedRowFrameSize = loaded;
        m_startFramePosition = start;

        emit layoutAboutToBeChanged();
        emit rowsLoaded(start, loaded);
        emit layoutChanged();
    });
}

void ValueEditor::ValueViewModel::clearRowCache()
{
    m_model->clearRowCache();
}

void ValueEditor::ValueViewModel::addRow(const QVariantMap &row)
{
    m_model->addRow(row);
}

int ValueEditor::ValueViewModel::totalRowCount()
{
    return m_model->rowsCount();
}

QVariantMap ValueEditor::ValueViewModel::get(int row)
{
    QHash<int,QByteArray> names = roleNames();
    QHashIterator<int, QByteArray> i(names);
    QVariantMap res;
    while (i.hasNext()) {
        i.next();
        QModelIndex idx = index(row, 0);
        QVariant data = idx.data(i.key());
        res[i.value()] = data;
    }
    return res;
}
