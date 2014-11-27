#include "viewmodel.h"
#include <QDebug>

ValueEditor::ViewModel::ViewModel(QSharedPointer<AbstractKeyFactory> keyFactory)
    : m_keyFactory(keyFactory), m_currentTabIndex(0)
{
}

void ValueEditor::ViewModel::openTab(QSharedPointer<RedisClient::Connection> connection,
                                     const QString &keyFullPath, int dbIndex, bool inNewTab)
{
    m_keyFactory->loadKey(connection, keyFullPath, dbIndex,
                        [this, inNewTab](QSharedPointer<Model> keyModel) {
                            if (keyModel.isNull())
                                return;

                            loadModel(keyModel, inNewTab);
                            qDebug() << "Key model loaded:"
                                     << keyModel->getKeyName()
                                     << keyModel->getType();

                        });
    // TODO: add empty key model for loading
}


QModelIndex ValueEditor::ViewModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(column);
    Q_UNUSED(parent);
    return createIndex(row, 0);
}

int ValueEditor::ViewModel::rowCount(const QModelIndex&) const
{
    return m_valueModels.count();
}

QVariant ValueEditor::ViewModel::data(const QModelIndex &index, int role) const
{
    if (!isIndexValid(index))
        return QVariant();

    QSharedPointer<Model> model = m_valueModels.at(index.row());

    if (model.isNull())
        return QVariant();

    switch (role) {
        case keyIndex: return index.row();
        case keyNameRole: return model->getKeyName();
        case keyTTL: return model->getTTL();
        case keyType: return model->getType();
        case state: return model->getState();
        case showValueNavigation: return model->isMultiRow();
        case columnNames: return QVariant(model->getColumnNames()).toList();
        case count: return static_cast<qulonglong>(model->rowsCount());
    }

    return QVariant();
}

bool ValueEditor::ViewModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!isIndexValid(index))
        return false;

    return false; //TBD
}

QHash<int, QByteArray> ValueEditor::ViewModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[keyIndex] = "keyIndex";
    roles[keyNameRole] = "keyName";
    roles[keyTTL] = "keyTtl";
    roles[keyType] = "keyType";
    roles[state] = "keyState";
    roles[showValueNavigation] = "showValueNavigation";
    roles[columnNames] = "columnNames";
    roles[count] = "valuesCount";
    return roles;
}

void ValueEditor::ViewModel::renameKey(int i, const QString& newKeyName)
{
    if (!isIndexValid(index(i, 0)))
        return;

    qDebug() << "Rename key:" << newKeyName;

    auto value = m_valueModels.at(i);

    try {
        value->setKeyName(newKeyName);
        emit dataChanged(index(i, 0), index(i, 0));
    } catch (const Model::Exception& e) {
        emit keyError(i, "Can't rename key: " + QString(e.what()));
    }
}

void ValueEditor::ViewModel::removeKey(int i)
{
    if (!isIndexValid(index(i, 0)))
        return;

    qDebug() << "Remove key:" << i;

    auto value = m_valueModels.at(i);

    try {
        value->removeKey();
        beginRemoveRows(QModelIndex(), i, i);
        m_valueModels.removeAt(i);
        endRemoveRows();
    } catch (const Model::Exception& e) {
        emit keyError(i, "Can't remove key: " + QString(e.what()));
    }
}

void ValueEditor::ViewModel::closeTab(int i)
{
    if (!isIndexValid(index(i, 0)))
        return;

    try {
        beginRemoveRows(QModelIndex(), i, i);
        m_valueModels.removeAt(i);
        endRemoveRows();
    } catch (const Model::Exception& e) {
        emit keyError(i, "Can't remove key: " + QString(e.what()));
    }
}

void ValueEditor::ViewModel::setCurrentTab(int i)
{
    m_currentTabIndex = i;
}

bool ValueEditor::ViewModel::isIndexValid(const QModelIndex &index) const
{
    return 0 <= index.row() && index.row() < rowCount();
}

void ValueEditor::ViewModel::loadModel(QSharedPointer<ValueEditor::Model> model, bool openNewTab)
{
    if (m_valueModels.count() == 0)
        emit closeWelcomeTab();

    if (openNewTab || m_valueModels.count() == 0) {
        beginInsertRows(QModelIndex(), m_valueModels.count(), m_valueModels.count());
        m_valueModels.append(model);
        endInsertRows();
    } else {
        m_valueModels.insert(m_currentTabIndex, model);
        m_valueModels.removeAt(m_currentTabIndex+1);
        emit dataChanged(index(m_currentTabIndex, 0), index(m_currentTabIndex, 0));
    }
}

//void ValueEditor::ViewModel::loadModel(QSharedPointer<ValueEditor::Model> m, bool loadLargeKeysInLegacy)
//{
//    m_model = m;

//    connect(m_model.data(), &Model::dataLoaded, this, &ViewModel::onDataLoaded);

//    if (!m_model->isMultiRow()) // all data already loaded
//        return;

//    std::function<void()> callback = [this]() {
//        rowsLoaded();
//    };

//    if (m_model->isPartialLoadingSupported()) {
//        m_model->loadRows(0, getPageLimit(), callback);
//    } else {
//        // TBD: show warning in UI: "(!) Partial loading not supported by current redis-server"
//        if (m_model->rowsCount() > getPageLimit() && !loadLargeKeysInLegacy) {
//            // TBD: show confirmation dialog in UI
//        } else {
//            m_model->loadRows(0, m_model->rowsCount(), callback);
//        }
//    }
//}


