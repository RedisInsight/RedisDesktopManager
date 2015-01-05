#include "valueviewmodel.h"
#include <QDebug>

const int PAGE_SIZE = 100;

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

int ValueEditor::ValueViewModel::mapRowIndex(int i)
{
    return m_startFramePosition + i;
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

void ValueEditor::ValueViewModel::reload()
{
    m_model->clearRowCache();
    loadRows(m_startFramePosition, m_model->rowsCount() < PAGE_SIZE ? m_model->rowsCount() : PAGE_SIZE);
    qDebug() << this;
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

void ValueEditor::ValueViewModel::addRow(const QVariantMap &row)
{
    try {                
        m_model->addRow(row);
        emit layoutChanged();
    } catch (const Model::Exception& e) {
        emit error(QString(e.what()));
    }
}

void ValueEditor::ValueViewModel::updateRow(int i, const QVariantMap &row)
{
    int targetRow = mapRowIndex(i);

    if (targetRow < 0 || !m_model->isRowLoaded(targetRow))
        return;

    try {
        m_model->updateRow(targetRow, row);
    } catch(const Model::Exception& e) {
        emit error(QString(e.what()));
    }

    emit dataChanged(index(i, 0), index(i, 0));
}

void ValueEditor::ValueViewModel::deleteRow(int i)
{
    int targetRow = mapRowIndex(i);

    if (targetRow < 0 || !m_model->isRowLoaded(targetRow))
        return;

    try {
        m_model->removeRow(targetRow);
    } catch(const Model::Exception& e) {
        emit error(QString(e.what()));
    }

    emit layoutChanged();
}

int ValueEditor::ValueViewModel::totalRowCount()
{
    return m_model->rowsCount();
}

int ValueEditor::ValueViewModel::pageSize()
{
    return PAGE_SIZE;
}

QVariantMap ValueEditor::ValueViewModel::getRow(int row, bool relative)
{
    QHash<int,QByteArray> names = roleNames();
    QHashIterator<int, QByteArray> i(names);
    QVariantMap res;

    int targetRow = (relative)? mapRowIndex(row) : row;

    while (i.hasNext()) {
        i.next();
        QModelIndex idx = index(targetRow, 0);
        QVariant data = idx.data(i.key());
        res[i.value()] = data;
    }
    return res;
}
