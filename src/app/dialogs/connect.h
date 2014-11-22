#pragma once

#include <QDialog>
#include <QWeakPointer>
#include "ui_connection.h"

namespace RedisClient {
class ConnectionConfig;
}
class ConnectionsManager;

class ConnectionWindow : public QDialog
{
    Q_OBJECT

public:
    ConnectionWindow(QWeakPointer<ConnectionsManager> manager, QWidget *parent = nullptr);

private:
    Ui::connectionDialog ui;        
    bool inEditMode;
    QWeakPointer<ConnectionsManager> m_manager;

    bool isFormDataValid();
    bool isConnectionSettingsValid();
    bool isSshSettingsValid();
    bool isAdvancedSettingsValid();
    bool isSshTunnelUsed();
    RedisClient::ConnectionConfig getConectionConfigFromFormData();
    void loadValuesFromConfig(const RedisClient::ConnectionConfig& config);

    private slots:
        void OnOkButtonClick();
        void OnShowPasswordCheckboxChanged(int);
        void OnBrowseSshKeyClick();
        void OnTestConnectionButtonClick();
};
