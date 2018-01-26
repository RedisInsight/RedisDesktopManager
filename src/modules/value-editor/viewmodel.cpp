#include "viewmodel.h"
#include <QDebug>
#include <qredisclient/connection.h>
#include <qredisclient/utils/text.h>
#include "connections-tree/items/keyitem.h"
#include "value-editor/valueviewmodel.h"

ValueEditor::ViewModel::ViewModel(QSharedPointer<AbstractKeyFactory> keyFactory)
    : m_keyFactory(keyFactory), m_currentTabIndex(0)
{
}

void ValueEditor::ViewModel::openTab(QSharedPointer<RedisClient::Connection> connection,
                                     ConnectionsTree::KeyItem& key, bool inNewTab)
{
    try {
        m_keyFactory->loadKey(connection, key.getFullPath(), key.getDbIndex(),
                            [this, inNewTab, &key](QSharedPointer<Model> keyModel, const QString& error)
        {
            if (keyModel.isNull() || !error.isEmpty()) {               
                emit keyError(-1, QString("<b>%1</b>:\n%2").arg(QObject::tr("Cannot open value tab")).arg(error));
                return;
            }

            loadModel(keyModel, inNewTab);

            auto weakKeyModel = keyModel.toWeakRef();

            QObject::connect(keyModel->getConnector().data(), &ModelSignals::removed,
                             this, [this, weakKeyModel, &key]()
            {
                auto keyModel = weakKeyModel.toStrongRef();

                if (!keyModel)
                    return;

                removeModel(keyModel);
                key.setRemoved(); //Disable key in connections tree
            });            
        });
        // TODO: add empty key model for loading
    } catch (...) {
        emit keyError(-1, QObject::tr("Connection error. Can't open value tab. "));
    }
}

void ValueEditor::ViewModel::closeDbKeys(QSharedPointer<RedisClient::Connection> connection, int dbIndex,
                                         const QRegExp& filter)
{
    for (int index = 0; 0 <= index && index < m_valueModels.size(); index++) {
        auto model = m_valueModels.at(index);

        if (model->getConnection() == connection && model->dbIndex() == dbIndex) {
            if (model->getKeyName().contains(filter)) {
                beginRemoveRows(QModelIndex(), index, index);
                m_valueModels.removeAt(index);
                endRemoveRows();
                index--;
            }
        }
    }
}

QModelIndex ValueEditor::ViewModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    if (row < 0 || column < 0)
        return QModelIndex();

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
        case keyDisplayName: return model->getKeyTitle();
        case keyTTL: return model->getTTL();
        case keyType: return model->getType();        
        case showValueNavigation: return model->isMultiRow();
        case columnNames: return QVariant(model->getColumnNames()).toList();
        case count: return static_cast<qulonglong>(model->rowsCount());
    }

    return QVariant();
}

QHash<int, QByteArray> ValueEditor::ViewModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[keyIndex] = "keyIndex";
    roles[keyNameRole] = "keyName";
    roles[keyDisplayName] = "keyTitle";
    roles[keyTTL] = "keyTtl";
    roles[keyType] = "keyType";    
    roles[showValueNavigation] = "showValueNavigation";
    roles[columnNames] = "columnNames";
    roles[count] = "valuesCount";    
    return roles;
}

void ValueEditor::ViewModel::addKey(QString keyName, QString keyType,
                                    const QVariantMap &row, QJSValue jsCallback)
{
    if (m_newKeyRequest.first.isNull()) {
        qDebug() << "Invalid new key request";
        return;
    }

    auto connection = m_newKeyRequest.first.toStrongRef();

    if (!connection) {
        qDebug() << "Invalid new key request";
        return;
    }

    try {
        m_keyFactory->addKey(connection, keyName.toUtf8(),
                             m_newKeyRequest.second, keyType, row);
        m_newKeyCallback();

        if (jsCallback.isCallable())
            jsCallback.call(QJSValueList {});

        m_newKeyRequest = NewKeyRequest();
    } catch (const Model::Exception& e) {
        if (jsCallback.isCallable())
            jsCallback.call(QJSValueList { QObject::tr("Can't add new key: ") + QString(e.what()) });
    }
}

void ValueEditor::ViewModel::renameKey(int i, const QString& newKeyName)
{
    if (!isIndexValid(index(i, 0)))
        return;

    auto value = m_valueModels.at(i);

    try {
        value->setKeyName(printableStringToBinary(newKeyName));
        emit dataChanged(index(i, 0), index(i, 0));
    } catch (const Model::Exception& e) {
        emit keyError(i, QObject::tr("Can't rename key: ") + QString(e.what()));
    }
}

void ValueEditor::ViewModel::removeKey(int i)
{
    if (!isIndexValid(index(i, 0)))
        return;

    auto value = m_valueModels.at(i);

    try {
        value->removeKey();
    } catch (const Model::Exception& e) {
        emit keyError(i, QObject::tr("Can't remove key: ") + QString(e.what()));
    }
}

void ValueEditor::ViewModel::setTTL(int i, const QString& newTTL)
{
    if (!isIndexValid(index(i, 0)))
        return;

    auto value = m_valueModels.at(i);

    try {
        value->setTTL(newTTL.toLong());
        emit dataChanged(index(i, 0), index(i, 0));
    } catch (const Model::Exception& e) {
        emit keyError(i, QObject::tr("Can't set key ttl: ") + QString(e.what()));
    }
}

void ValueEditor::ViewModel::closeTab(int i)
{
    if (!isIndexValid(index(i, 0)))
        return;

    try {
        beginRemoveRows(QModelIndex(), i, i);
        m_valueModels[i].clear();
        m_valueModels.removeAt(i);
        endRemoveRows();
    } catch (const Model::Exception& e) {
        emit keyError(i, QObject::tr("Can't close key tab: ") + QString(e.what()));
    }
}

void ValueEditor::ViewModel::setCurrentTab(int i)
{
    m_currentTabIndex = i;
}

QObject* ValueEditor::ViewModel::getValue(int i)
{
    if (!isIndexValid(index(i, 0)))
        return nullptr;

    auto model = m_valueModels.at(i);

    QList<QObject *> valueEditors = model->getConnector()->findChildren<QObject *>();


    if (valueEditors.isEmpty())
        return new ValueEditor::ValueViewModel(*model);
    else
        return valueEditors[0];
}

void ValueEditor::ViewModel::openNewKeyDialog(QSharedPointer<RedisClient::Connection> connection,
                                              std::function<void()> callback,
                                              int dbIndex, QString keyPrefix)
{
    if (connection.isNull() || dbIndex < 0)
        return;

    m_newKeyRequest = qMakePair(connection.toWeakRef(), dbIndex);
    m_newKeyCallback = callback;

    QString dbId= QString("%1:db%2")
            .arg(connection->getConfig().name())
            .arg(dbIndex);

    emit newKeyDialog(dbId, keyPrefix);
}

bool ValueEditor::ViewModel::isIndexValid(const QModelIndex &index) const
{
    return 0 <= index.row() && index.row() < rowCount();
}

void ValueEditor::ViewModel::loadModel(QSharedPointer<ValueEditor::Model> model, bool openNewTab)
{
    if (openNewTab || m_valueModels.count() == 0) {
        beginInsertRows(QModelIndex(), m_valueModels.count(), m_valueModels.count());
        m_valueModels.append(model);
        endInsertRows();
    } else {
        emit layoutAboutToBeChanged();
        m_valueModels[m_currentTabIndex].clear();
        m_valueModels.removeAt(m_currentTabIndex);
        m_valueModels.insert(m_currentTabIndex, model);

        emit layoutChanged();
        emit replaceTab(m_currentTabIndex);
    }
}

void ValueEditor::ViewModel::removeModel(QSharedPointer<ValueEditor::Model> model)
{
    int i = m_valueModels.lastIndexOf(model);

    if (i == -1) {
        qDebug() << "[Remove model] Key model not found!";
        return;
    }

    beginRemoveRows(QModelIndex(), i, i);
    m_valueModels.removeAt(i);
    endRemoveRows();
}
