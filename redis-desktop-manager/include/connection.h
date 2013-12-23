#pragma once

#include <QDialog>
#include "ui_connection.h"

class RedisServerItem;
class RedisConnectionConfig;
class MainWin;
class ConnectionBridge;

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
    bool isSshTunnelUsed();
    RedisConnectionConfig getConectionConfigFromFormData();
    void loadValuesFromConnection(ConnectionBridge *);

    private slots:
        void OnOkButtonClick();
};
