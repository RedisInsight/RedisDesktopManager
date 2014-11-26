#pragma once

#include <QSharedPointer>
#include "redisclient/connection.h"
#include "connections-tree/model.h"

class ConsoleTabs;

namespace ValueEditor {
    class ViewModel;
}

class ConnectionsManager : public ConnectionsTree::Model
{
    Q_OBJECT    

public:
    ConnectionsManager(const QString& configPath, ConsoleTabs& tabs,
                       QSharedPointer<ValueEditor::ViewModel> values);
    ~ConnectionsManager(void);

    void addConnection(QSharedPointer<RedisClient::Connection> connection);
    bool importConnections(const QString &);
    bool saveConnectionsConfigToFile(const QString&);

    int size();

private:
    QString configPath;
    bool connectionSettingsChanged;
    QList<QSharedPointer<RedisClient::Connection>> connections;
    ConsoleTabs& m_tabs;
    QSharedPointer<ValueEditor::ViewModel> m_values;

protected:
    bool loadConnectionsConfigFromFile(const QString& config, bool saveChangesToFile = false);
};

