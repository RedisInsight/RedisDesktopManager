#ifndef TEST_REDISCONNECTIONSMANAGER_H
#define TEST_REDISCONNECTIONSMANAGER_H

#include <QObject>

class TestConnectionsManager : public QObject
{
	Q_OBJECT

private slots:
	void LoadConnectionsConfigFromFile();
	
};

#endif // TEST_REDISCONNECTIONSMANAGER_H
