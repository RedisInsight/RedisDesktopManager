#include "test_dialogs.h"
#include "app/dialogs/connect.h"
#include "models/connectionsmanager.h"
#include "app/widgets/consoletabs.h"

namespace ValueEditor {
    class ViewModel;
}

void TestDialogs::testConnectionDialog()
{
    //given
    QString configTestFile = "connections.xml";
    ConsoleTabs tabsWidget;
    QSharedPointer<ConnectionsManager> testManager(new ConnectionsManager(configTestFile, tabsWidget,
                                   QSharedPointer<ValueEditor::ViewModel>()));

    ConnectionWindow window(testManager.toWeakRef());
    RedisClient::ConnectionConfig conf("fake_host", "fake_name");
    conf.setSshTunnelSettings("fake_ssh_host", "fake_ssh_user", "fake_ssh_pass");

    //when
    window.setConnectionConfig(conf);
    RedisClient::ConnectionConfig actualResult = window.getConectionConfigFromFormData();

    //then
    QCOMPARE(actualResult.getInternalParameters(), conf.getInternalParameters());
}
