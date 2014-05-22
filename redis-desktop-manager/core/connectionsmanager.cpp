#include <QtXml>
#include "connectionsmanager.h"
#include "connectionconfig.h"
#include "connection.h"
#include "RedisServerItem.h"
#include "RedisServerDbItem.h"
#include "application.h"

RedisConnectionsManager::RedisConnectionsManager(QString config, QObject * parent = nullptr)
    : QStandardItemModel(parent), configPath(config), connectionSettingsChanged(false)
{
    if (!config.isEmpty() && QFile::exists(config)) {
        LoadConnectionsConfigFromFile(config);
    }
}


RedisConnectionsManager::~RedisConnectionsManager(void)
{
    if (connectionSettingsChanged) {
        SaveConnectionsConfigToFile(configPath);
    }
}

void RedisConnectionsManager::AddConnection(RedisClient::Connection * c)
{
    //add connection to internal container
    connections.push_back(c);

    //add connection to view container    
    RedisServerItem * item = new RedisServerItem(c);
    QObject::connect(item, SIGNAL(databasesLoaded()), this, SLOT(updateFilter()));
    MainWin * errorViewForm = (MainWin *) parent();
    QObject::connect(item, SIGNAL(error(QString)), errorViewForm, SLOT(OnError(QString)));
    QObject::connect(c, SIGNAL(log(QString)), errorViewForm, SLOT(OnLogMessage(QString)));
    QObject::connect(item, SIGNAL(unlockUI()), errorViewForm, SLOT(UnlockUi()));
    QObject::connect(item, SIGNAL(statusMessage(QString)), errorViewForm, SLOT(OnStatusMessage(QString)));

    appendRow(item);

    //mark settings as unsaved
    connectionSettingsChanged = true;    
}

bool RedisConnectionsManager::RemoveConnection(RedisServerItem * c)
{
    if (c == nullptr) {
        return false;
    }

    bool removedFromContainer = connections.removeOne(c->getConnection());

    bool removedFromModel = this->removeRow(c->row());

    //mark settings as unsaved
    if (removedFromContainer && removedFromModel) 
        connectionSettingsChanged = true;

    return removedFromContainer && removedFromModel;
}

bool RedisConnectionsManager::ImportConnections(QString &path)
{
    if (LoadConnectionsConfigFromFile(path, true)) {
        return true;
    }

    return false;
}


bool RedisConnectionsManager::LoadConnectionsConfigFromFile(QString& config, bool saveChangesToFile)
{
    QFile conf(config);
    
    if (!conf.open(QIODevice::ReadOnly)) 
        return false;    
    
    QDomDocument xmlConf;

    if (xmlConf.setContent(&conf)) {

        QDomNodeList connectionsList = xmlConf.elementsByTagName("connection");
        
        for (int i = 0; i < connectionsList.size(); ++i) {

            QDomNode connection = connectionsList.at(i);

            if (connection.nodeName() != "connection") continue;

            RedisClient::ConnectionConfig conf = RedisClient::ConnectionConfig::createFromXml(connection);

            if (conf.isNull()) continue;

            AddConnection(new RedisClient::Connection(conf, false));
        }        
    }
    conf.close();

    if (!saveChangesToFile)
        connectionSettingsChanged = false;    

    return true;
}

bool RedisConnectionsManager::SaveConnectionsConfigToFile(QString pathToFile)
{
    QDomDocument config;

    QDomProcessingInstruction xmlProcessingInstruction = config.createProcessingInstruction("xml", "version=\"1.0\"");
    config.appendChild(xmlProcessingInstruction);

    QDomElement connectionsItem = config.createElement("connections");

    config.appendChild(connectionsItem);

    for (RedisClient::Connection * c : connections) {
        connectionsItem.appendChild(c->getConfig().toXml(config));
    }

    QFile confFile(pathToFile);

    if (confFile.open(QIODevice::WriteOnly)) {
        QTextStream(&confFile) << config.toString();
        confFile.close();
        return true;
    }

    return false;
}

void RedisConnectionsManager::setFilter(QRegExp & pattern)
{
    if (pattern.isEmpty()) {
        return;
    }

    filter = pattern;

    updateFilter();
}

void RedisConnectionsManager::resetFilter()
{
    filter.setPattern("");
    updateFilter();
}

void RedisConnectionsManager::updateFilter()
{
    int rowsCount, childRowsCount;
    RedisServerDbItem * db;
    QStandardItem * element;

    rowsCount = rowCount();

    for (int i = 0; i < rowsCount; i++) {
        element = item(i);

        childRowsCount  = element->rowCount();

        for (int childRowIndex = 0; childRowIndex < childRowsCount; childRowIndex++) 
        {
            db = (RedisServerDbItem *) element->child(childRowIndex);

            db->setFilter(filter);
        }
    }
}

void RedisConnectionsManager::connectionChanged()
{
    SaveConnectionsConfigToFile(configPath);
}
