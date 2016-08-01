#include "valueviewmodel.h"
#include <QDebug>

const int PAGE_SIZE = 1000;

ValueEditor::ValueViewModel::ValueViewModel(QSharedPointer<ValueEditor::Model> model)
    : QAbstractListModel((QObject*)model->getConnector().data()),
      m_model(model),
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
        emit rowsLoaded(start, count);
        emit layoutChanged();
        return;
    }

    QString msg = QString("Cannot load key value: %1");

    try {
        // NOTE(u_glide): Do so for proper rendering of QML table
        m_lastLoadedRowFrameSize = totalRowCount() - start;
        m_model->loadRows(start, count, [this, start, count, msg](const QString& err)
        {
            if (!err.isEmpty()) {
                emit error(msg.arg(err));
                return;
            }

            int loaded = totalRowCount() - start;
            loaded = (loaded > count) ? count : loaded;

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

    QVariantMap res = getRowRaw(i);
    // check original value to avoid maintaining state
    bool updated = false;

    if (ValueEditor::Compression::isCompressed(res["value"].toByteArray())) {
        QByteArray output;
        if (!ValueEditor::Compression::compress(row["value"].toByteArray(), output)) {
            qDebug() << "Compression failed";
        } else {
            qDebug() << "Compression succeeded";
            QVariantMap compressedRow;
            compressedRow["value"] = QVariant(output);
            
            try {
                m_model->updateRow(targetRow, compressedRow);
                updated = true;
            } catch(const Model::Exception& e) {
                emit error(QString(e.what()));
            }
        }
    }

    if (!updated) {
        try {
            m_model->updateRow(targetRow, row);
        } catch(const Model::Exception& e) {
            emit error(QString(e.what()));
        }
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
        emit beginRemoveRows(QModelIndex(), targetRow, targetRow);
        emit endRemoveRows();

        if (targetRow < m_model->rowsCount())
            emit dataChanged(index(targetRow, 0), index(m_model->rowsCount() - 1, 0));
    } catch(const Model::Exception& e) {
        emit error(QString(e.what()));     
    }
}

int ValueEditor::ValueViewModel::totalRowCount()
{
    return m_model->rowsCount();
}

int ValueEditor::ValueViewModel::pageSize()
{
    return PAGE_SIZE;
}

QVariantMap ValueEditor::ValueViewModel::getRow(int row)
{
    int targetRow = mapRowIndex(row);
    if (targetRow < 0 || !m_model->isRowLoaded(targetRow))
        return QVariantMap();

    QVariantMap res = getRowRaw(row);
    bool compressed = ValueEditor::Compression::isCompressed(res["value"].toByteArray());
    qDebug() << "Is compressed: " << compressed;
    if (compressed) {
        QByteArray output;
        if (!ValueEditor::Compression::decompress(res["value"].toByteArray(), output)) {
            qDebug() << "Decompression failed";
        } else {
            qDebug() << "Decompression succeeded";
            res["value"] = QVariant(output);
        }
    }
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
