#pragma once

#include <QDialog>
#include <QWeakPointer>
#include "ui_connection.h"
#include "app/models/connectionconf.h"

class ConnectionsManager;
class TestDialogs;

class ConnectionWindow : public QDialog
{
    Q_OBJECT
    friend class TestDialogs;
public:
    ConnectionWindow(QWeakPointer<ConnectionsManager> manager, QWidget *parent = nullptr);
    void setConnectionConfig(const ConnectionConfig& config);
    ConnectionConfig getConectionConfigFromFormData();
    bool isFormDataValid();

private:
    Ui::connectionDialog ui;        
    bool m_inEditMode;
    QWeakPointer<ConnectionsManager> m_manager;
    ConnectionConfig m_config;

    bool isConnectionSettingsValid();
    bool isSshSettingsValid();
    bool isAdvancedSettingsValid();
    bool isSshTunnelUsed();
    bool isSslUsed();
    bool isSslSettingsValid();

    void loadValuesFromConfig(const ConnectionConfig& config);

    void markFieldInvalid(QWidget *w);
    void markFieldValid(QWidget *w);

private slots:
    void OnOkButtonClick();
    void OnShowPasswordCheckboxChanged(int);    
    void OnBrowseFileClick(QLineEdit *target, QString, QString);
    void OnTestConnectionButtonClick();
};
