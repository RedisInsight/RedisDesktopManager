#include "consoleviewmodel.h"
#include <QDebug>
#include <QTimer>
#include <qredisclient/connection.h>
#include <qredisclient/utils/text.h>
#include <googlemp.h>

Console::ViewModel::ViewModel()
    : m_currentTabIndex(0)
{
}

void Console::ViewModel::openConsole(QSharedPointer<RedisClient::Connection> connection)
{
    beginInsertRows(QModelIndex(), m_models.count(), m_models.count());

    QSharedPointer<Model> model(new Model(connection));
    m_models.append(model);
    QTimer::singleShot(100, this, [model]() { model->init(); });

    endInsertRows();
}

void Console::ViewModel::closeAllTabsWithConnection(QSharedPointer<RedisClient::Connection> connection)
{    
    for (int index = 0; 0 <= index && index < m_models.size(); index++) {
        auto model = m_models.at(index);

        if (model->getConnection() == connection) {
            beginRemoveRows(QModelIndex(), index, index);
            m_models.removeAt(index);
            endRemoveRows();
            index--;
        }
    }
}

QModelIndex Console::ViewModel::index(int row, int, const QModelIndex&) const
{    
    return createIndex(row, 0);
}

int Console::ViewModel::rowCount(const QModelIndex&) const
{
    return m_models.count();
}

QVariant Console::ViewModel::data(const QModelIndex &index, int role) const
{
    if (!isIndexValid(index))
        return QVariant();

    QSharedPointer<Model> model = m_models.at(index.row());

    if (model.isNull())
        return QVariant();

    switch (role) {
        case consoleIndex: return index.row();
        case consoleName: return model->getName();
        case consoleState: return "tbd"; // TODO: add state in console model
    }

    return QVariant();
}

QHash<int, QByteArray> Console::ViewModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[consoleIndex] = "consoleIndex";
    roles[consoleName] = "consoleName";
    roles[consoleState] = "consoleState";
    return roles;
}

void Console::ViewModel::closeTab(int i)
{
    if (!isIndexValid(index(i, 0)))
        return;

    beginRemoveRows(QModelIndex(), i, i);
    m_models.removeAt(i);
    endRemoveRows();
}

void Console::ViewModel::setCurrentTab(int i)
{
    m_currentTabIndex = i;
}

QObject* Console::ViewModel::getValue(int i)
{
    if (!isIndexValid(index(i, 0)))
        return nullptr;

    return qobject_cast<QObject* >(m_models.at(i).data());
}

bool Console::ViewModel::isIndexValid(const QModelIndex &index) const
{
    return 0 <= index.row() && index.row() < rowCount();
}
