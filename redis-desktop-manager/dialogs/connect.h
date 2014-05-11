#pragma once

#include <QDialog>
#include "ui_connection.h"

class RedisServerItem;

class MainWin;
namespace RedisClient {
class ConnectionConfig;
class Connection;
}

class ConnectionWindow : public QDialog
{
    Q_OBJECT

public:
    ConnectionWindow(QWidget *parent = 0, RedisServerItem * c = nullptr);

private:
    Ui::connectionDialog ui;
    MainWin * mainForm;
    RedisServerItem * server;
    bool inEditMode;

    bool isFormDataValid();
    bool isConnectionSettingsValid();
    bool isSshSettingsValid();
    bool isAdvancedSettingsValid();
    bool isSshTunnelUsed();
    RedisClient::ConnectionConfig getConectionConfigFromFormData();
    void loadValuesFromConnection(RedisClient::Connection *);

    private slots:
        void OnOkButtonClick();
        void OnShowPasswordCheckboxChanged(int);
        void OnBrowseSshKeyClick();
        void OnTestConnectionButtonClick();
};
