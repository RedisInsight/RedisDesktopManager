#pragma once

#include <QDialog>
#include <QWeakPointer>
#include "ui_connection.h"
#include "modules/redisclient/connectionconfig.h"

class ConnectionsManager;

class ConnectionWindow : public QDialog
{
    Q_OBJECT

public:
    ConnectionWindow(QWeakPointer<ConnectionsManager> manager, QWidget *parent = nullptr);
    void setConnectionConfig(const RedisClient::ConnectionConfig& config);
    RedisClient::ConnectionConfig getConectionConfigFromFormData();
    bool isFormDataValid();

private:
    Ui::connectionDialog ui;        
    bool m_inEditMode;
    QWeakPointer<ConnectionsManager> m_manager;
    RedisClient::ConnectionConfig m_config;

    bool isConnectionSettingsValid();
    bool isSshSettingsValid();
    bool isAdvancedSettingsValid();
    bool isSshTunnelUsed();
    bool isSslUsed();

    void loadValuesFromConfig(const RedisClient::ConnectionConfig& config);

    void markFieldInvalid(QWidget *w);
    void markFieldValid(QWidget *w);

private slots:
    void OnOkButtonClick();
    void OnShowPasswordCheckboxChanged(int);
    void OnBrowseSshKeyClick();
    void OnBrowsePemFileClick(QLineEdit *target);
    void OnTestConnectionButtonClick();
};
