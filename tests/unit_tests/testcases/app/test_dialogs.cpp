#include "test_dialogs.h"
#include "app/dialogs/connect.h"
#include "models/connectionsmanager.h"
#include "app/widgets/consoletabs.h"
#include "value-editor/viewmodel.h"
#include <functional>
#include <QTemporaryFile>
#include <QFileInfo>

namespace ValueEditor {
    class ViewModel;
}

void TestDialogs::init()
{
    QFile::remove("connections.json");
}

void TestDialogs::testConnectionDialog()
{
    //given
    QString configTestFile = "connections.json";
    ConsoleTabs tabsWidget;
    QSharedPointer<ConnectionsManager> testManager(new ConnectionsManager(configTestFile, tabsWidget));
    QTemporaryFile fake_private_key;
    QFileInfo fk(fake_private_key);
    QString fake_file_path = fk.absoluteFilePath();

    //ssh with password
    ConnectionConfig conf("fake_host", "fake_auth", 6379, "fake_name");
    conf.setSshTunnelSettings("fake_ssh_host", "fake_ssh_user", "fake_ssh_pass");

    //ssh with key
    ConnectionConfig conf2("fake_host", "fake_auth", 6379, "fake_name");
    conf2.setSshTunnelSettings("fake_ssh_host", "fake_ssh_user", "", 2222, fake_file_path);

    //ssh with key and pass
    ConnectionConfig conf3("fake_host", "fake_auth", 6379, "fake_name");
    conf3.setSshTunnelSettings("fake_ssh_host", "fake_ssh_user", "fake_ssh_key_pass", 2222, fake_file_path);

    // ssl with auth and custom port
    ConnectionConfig conf4("fake_host", "auth", 6380, "fake_name");
    conf4.setSslSettigns(fake_file_path, fake_file_path, fake_file_path);

    auto verify = [this, testManager](RedisClient::ConnectionConfig& conf) {
        ConnectionWindow window(testManager.toWeakRef());

        //when
        window.setConnectionConfig(conf);
        RedisClient::ConnectionConfig actualResult = window.getConectionConfigFromFormData();

        //then
        QCOMPARE(actualResult.getInternalParameters(), conf.getInternalParameters());
        QCOMPARE(window.isFormDataValid(), true);
    };

    qDebug() << "SSH with pass";
    verify(conf);

    qDebug() << "SSH with key";
    verify(conf2);

    qDebug() << "SSH with key and pass";
    verify(conf3);

    qDebug() << "SSL with auth and custom port";
    verify(conf4);
}

void TestDialogs::testConnectionDialogValidation()
{
    QString configTestFile = "connections.json";
    ConsoleTabs tabsWidget;
    QSharedPointer<ConnectionsManager> testManager(new ConnectionsManager(configTestFile, tabsWidget));
    ConnectionWindow window(testManager.toWeakRef());

    QCOMPARE(window.isAdvancedSettingsValid(), true);

    window.ui.namespaceSeparator->setText("");
    QCOMPARE(window.isAdvancedSettingsValid(), true);

    window.ui.namespaceSeparator->setText(":");
    window.ui.keysPattern->setText("");
    QCOMPARE(window.isAdvancedSettingsValid(), false);
}

void TestDialogs::testOkButtonInvalidSettings()
{
    QString configTestFile = "connections.json";
    ConsoleTabs tabsWidget;
    QSharedPointer<ConnectionsManager> testManager(new ConnectionsManager(configTestFile, tabsWidget));
    ConnectionWindow window(testManager.toWeakRef());

    QCOMPARE(window.isConnectionSettingsValid(), false);
    QCOMPARE(window.isFormDataValid(), false);

    window.OnOkButtonClick();
    QCOMPARE(testManager->size(), 0);
}

void TestDialogs::testOkButton()
{
    using namespace ValueEditor;
    QString configTestFile = "connections.json";
    ConsoleTabs tabsWidget;
    QSharedPointer<ConnectionsManager> testManager(
                new ConnectionsManager(configTestFile, tabsWidget));
    ConnectionWindow window(testManager.toWeakRef());

    window.ui.hostEdit->setText("fake");
    window.ui.nameEdit->setText("fake");

    QCOMPARE(window.isConnectionSettingsValid(), true);
    QCOMPARE(window.isFormDataValid(), true);

    window.OnOkButtonClick();
    QCOMPARE(testManager->size(), 1);
}
