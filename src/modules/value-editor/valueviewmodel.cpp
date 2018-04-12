#include "valueviewmodel.h"
#include <QDebug>
#include <QSettings>

ValueEditor::ValueViewModel::ValueViewModel(Model &model)
    : QAbstractListModel((QObject*)model.getConnector().data()),
      m_model(model),
      m_startFramePosition(0),
      m_lastLoadedRowFrameSize(0)
{

}

int ValueEditor::ValueViewModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);    

    return m_lastLoadedRowFrameSize;
}

QVariant ValueEditor::ValueViewModel::data(const QModelIndex &index, int role) const
{
    if (!isIndexValid(index))
        return QVariant();   

    return m_model.getData(m_startFramePosition + index.row(), role);
}

QHash<int, QByteArray> ValueEditor::ValueViewModel::roleNames() const
{
    return m_model.getRoles();
}

bool ValueEditor::ValueViewModel::isIndexValid(const QModelIndex &index) const
{
    return 0 <= index.row() && index.row() < rowCount();
}

int ValueEditor::ValueViewModel::mapRowIndex(int i)
{
    return m_startFramePosition + i;
}

QVariantList ValueEditor::ValueViewModel::getColumnNames()
{
    QVariantList result;

    foreach (QString str, m_model.getColumnNames())
    {
        result.append(QVariant(str));
    }

    return result;
}

bool ValueEditor::ValueViewModel::isMultiRow()
{
    return m_model.isMultiRow();
}

void ValueEditor::ValueViewModel::reload()
{
    m_model.clearRowCache();
    loadRows(m_startFramePosition, m_model.rowsCount() < pageSize() ? m_model.rowsCount() : pageSize());
}

bool ValueEditor::ValueViewModel::isRowLoaded(int i)
{
    return m_model.isRowLoaded(i);
}

void ValueEditor::ValueViewModel::loadRows(int start, int limit)
{
    int rowsLeft = totalRowCount() - start;
    int loaded = (rowsLeft > limit) ? limit : rowsLeft;

    // frame already loaded
    if (m_model.isRowLoaded(start)) {
        m_startFramePosition = start;
        m_lastLoadedRowFrameSize = loaded;

        emit layoutAboutToBeChanged();
        emit rowsLoaded(start, loaded);
        emit layoutChanged();
        return;
    }

    QString msg = QString(QObject::tr("Cannot load key value: %1"));

    try {
        // NOTE(u_glide): Do so for proper rendering of QML table
        m_lastLoadedRowFrameSize = totalRowCount() - start;
        m_model.loadRows(start, limit, [this, start, limit, loaded, msg](const QString& err)
        {
            if (!err.isEmpty()) {
                emit error(msg.arg(err));
                return;
            }                      

            m_lastLoadedRowFrameSize = loaded;
            m_startFramePosition = start;

            emit layoutAboutToBeChanged();
            emit rowsLoaded(start, loaded);
            emit layoutChanged();
        });
    } catch (const ValueEditor::Model::Exception& e) {
        emit error(msg.arg(e.what()));
    }
}

void ValueEditor::ValueViewModel::addRow(const QVariantMap &row)
{
    try {                
        m_model.addRow(row);
        emit layoutChanged();
    } catch (const Model::Exception& e) {
        emit error(QString(e.what()));
    }
}

void ValueEditor::ValueViewModel::updateRow(int i, const QVariantMap &row)
{
    int targetRow = mapRowIndex(i);
    if (targetRow < 0 || !m_model.isRowLoaded(targetRow))
        return;

    try {
        m_model.updateRow(targetRow, row);
    } catch(const Model::Exception& e) {
        emit error(QString(e.what()));
    }

    emit dataChanged(index(i, 0), index(i, 0));
}

void ValueEditor::ValueViewModel::deleteRow(int i)
{
    int targetRow = mapRowIndex(i);

    if (targetRow < 0 || !m_model.isRowLoaded(targetRow))
        return;

    try {
        emit beginRemoveRows(QModelIndex(), i, i);
        m_model.removeRow(targetRow);        
        emit endRemoveRows();

        if (targetRow < m_model.rowsCount())
            emit dataChanged(index(i, 0), index(m_model.rowsCount() - 1, 0));

    } catch(const Model::Exception& e) {
        emit error(QString(e.what()));
    }    
}

int ValueEditor::ValueViewModel::totalRowCount()
{
    return m_model.rowsCount();
}

int ValueEditor::ValueViewModel::pageSize()
{
    QSettings settings;

    return settings.value("app/valueEditorPageSize", 1000).toInt();
}

QVariantMap ValueEditor::ValueViewModel::getRow(int row)
{
    int targetRow = mapRowIndex(row);
    if (targetRow < 0 || !m_model.isRowLoaded(targetRow))
        return QVariantMap();

    QVariantMap res = getRowRaw(row);
    return res;
}

QVariantMap ValueEditor::ValueViewModel::getRowRaw(int row)
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
