#include "viewmodel.h"

ValueEditor::ViewModel::ViewModel(QSharedPointer<ValueEditor::Model> model)
    : m_model(model)
{

}

QString ValueEditor::ViewModel::keyName()
{
    return m_model->getKeyName();
}

int ValueEditor::ViewModel::keyTTL()
{
    return m_model->getTTL();
}

QString ValueEditor::ViewModel::keyType()
{
    return m_model->getType();
}

QString ValueEditor::ViewModel::state() const
{
    return ""; // TBD
}

bool ValueEditor::ViewModel::showValueNavigation() const
{
    return m_model->isMultiRow();
}

QStringList ValueEditor::ViewModel::columnNames() const
{
    return m_model->getColumnNames();
}

int ValueEditor::ViewModel::currentPage() const
{
    return m_currentPage;
}

int ValueEditor::ViewModel::pageCount() const
{
    return m_pageCount;
}

void ValueEditor::ViewModel::setKeyName(const QString &name)
{
    m_model->setKeyName(name);
}

void ValueEditor::ViewModel::setKeyTTL(int ttl)
{
    m_model->setTTL(ttl);
}

void ValueEditor::ViewModel::setCurrentPage(int page)
{
    //TBD
}

QModelIndex ValueEditor::ViewModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(column);
    Q_UNUSED(parent);
    return createIndex(row, 0);
}

int ValueEditor::ViewModel::rowCount(const QModelIndex &parent) const
{
    return 0; // TBD
}

QVariant ValueEditor::ViewModel::data(const QModelIndex &index, int role) const
{
    return QVariant(); // TBD
}

bool ValueEditor::ViewModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    return false; //TBD
}

void ValueEditor::ViewModel::removeKey()
{
    m_model->removeKey();
}

void ValueEditor::ViewModel::removeRow(int index)
{
    m_model->removeRow(index);
}

void ValueEditor::ViewModel::selectCell(int index, int role)
{
    // TBD
}

void ValueEditor::ViewModel::reloadCurrentPage()
{
    // TBD
}

void ValueEditor::ViewModel::reloadCurrentValue()
{
    // TBD
}
