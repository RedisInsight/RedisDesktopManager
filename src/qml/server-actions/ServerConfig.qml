import QtQuick 2.3
import QtQuick.Layouts 1.13
import QtQuick.Controls 2.13
import QtQuick.Controls 1.4 as LC
import QtQuick.Window 2.2
import QtCharts 2.3
import "./../common"
import "./../common/platformutils.js" as PlatformUtils
import "./../settings"

ServerAction {
    id: tab

    uiBlocked: !serverInfoBuilder.model

    ColumnLayout {

        anchors.fill: parent
        anchors.margins: 10

        BoolOption {
            id: autorefreshSwitch

            Layout.preferredWidth: 200
            Layout.preferredHeight: 40

            value: true
            label: qsTranslate("RESP","Auto Refresh")
        }

        TabBar {
            id: serverInfoDetailsTabBar
            Layout.fillWidth: true
            Layout.preferredHeight: 30
            visible: !uiBlocked

            currentIndex: 0

            Repeater {
                id: serverInfoBuilderTabButtons
                TabButton {
                    text: modelData['name']
                    implicitWidth: 100
                }
            }
        }

        StackLayout {
            id: serverInfoTabs

            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.topMargin: 15

            currentIndex: serverInfoDetailsTabBar.currentIndex

            Repeater {
                id: serverInfoBuilder

                LegacyTableView {

                    model: modelData['section_data']

                    LC.TableViewColumn {
                        role: "name"
                        title: qsTranslate("RESP","Property")
                        width: 250
                    }

                    LC.TableViewColumn {
                        role: "value"
                        title: qsTranslate("RESP","Value")
                        width: 350
                    }
                }

            }

            Connections {
                target: tab.model? tab.model : null

                function onServerInfoChanged() {
                    if (autorefreshSwitch.value === false)
                        return;

                    loadServerInfo();

                    if (uiBlocked)
                        uiBlocked = false;
                }

                function loadServerInfo() {
                    var sections = []

                    for (var section in tab.model.serverInfo) {
                        var section_data = []
                        for (var key in tab.model.serverInfo[section])
                        {
                            var property = {"name": key, "value": tab.model.serverInfo[section][key]}
                            section_data.push(property)
                        }
                        sections.push({"name": section, "section_data": section_data})
                    }

                    var currentTab = serverInfoTabs.currentIndex
                    serverInfoBuilder.model = sections
                    serverInfoBuilderTabButtons.model = sections
                    serverInfoDetailsTabBar.currentIndex = currentTab
                }
            }
        }
    }
}
