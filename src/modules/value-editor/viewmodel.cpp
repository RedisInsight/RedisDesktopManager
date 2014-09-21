#include "viewmodel.h"

ValueEditor::ViewModel::ViewModel(QString fullKeyPath, int dbIndex, QSharedPointer<AbstractKeyFactory> keyFactory)
    : m_currentState(ValueEditor::ViewModel::State::Init)
{
    keyFactory->loadKey(fullKeyPath, dbIndex, [this](QSharedPointer<Model> keyModel) {
        loadModel(keyModel);
    });

    setCurrentState(State::Loading);
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
    switch (m_currentState) {
        case State::Init: return "init";
        case State::Loading: return "loading";
        case State::Loaded: return "loaded";
        default: return "error";
    }
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

void ValueEditor::ViewModel::onDataLoaded(/* TBD */)
{

}

void ValueEditor::ViewModel::setCurrentState(ValueEditor::ViewModel::State s)
{
    m_currentState = s;
    emit stateChanged();
}

void ValueEditor::ViewModel::loadModel(QSharedPointer<ValueEditor::Model> m, bool loadLargeKeysInLegacy)
{
    m_model = m;

    connect(m_model.data(), &Model::dataLoaded, this, &ViewModel::onDataLoaded);

    if (!m_model->isMultiRow()) // all data already loaded
        return;   

    std::function<void()> callback = [this]() {
        rowsLoaded();
    };

    if (m_model->isPartialLoadingSupported()) {
        m_model->loadRows(0, getPageLimit(), callback);
    } else {
        // TBD: show warning in UI: "(!) Partial loading not supported by current redis-server"
        if (m_model->rowsCount() > getPageLimit() && !loadLargeKeysInLegacy) {
            // TBD: show confirmation dialog in UI
        } else {
            m_model->loadRows(0, m_model->rowsCount(), callback);
        }
    }
}

unsigned long ValueEditor::ViewModel::getPageLimit()
{
    return (unsigned long) 100;
}

void ValueEditor::ViewModel::rowsLoaded()
{

}
