import QtQuick 2.3
import QtQuick.Layouts 1.13
import QtQuick.Controls 2.13
import QtQuick.Controls 1.4 as LC
import QtQuick.Dialogs 1.2
import QtQuick.Window 2.2
import QtCharts 2.3
import "./../common"
import "./../settings"


Repeater {
    id: root

    BetterTab {
        id: tab

        Component {
            id: serverTabButton

            BetterTabButton {
                icon.source: "qrc:/images/database.svg"

                text: tabName

                onCloseClicked: {
                    serverStatsModel.closeTab(tabIndex)
                }
            }
        }

        Component.onCompleted: {
            var tabButton = serverTabButton.createObject(tab);
            tabButton.self = tabButton;
            tabButton.tabRef = tab;
            tabBar.addItem(tabButton)
            tabBar.activateTabButton(tabButton)
            tabs.activateTab(tab)
        }

        property var model: tabModel

        onModelChanged: {
            if (!model)
                return;

            tab.model.init()
        }

        Rectangle {
            anchors.fill: parent

            color: sysPalette.base

            ColumnLayout {

                anchors.fill: parent
                anchors.margins: 15

                GridLayout {
                    Layout.fillWidth: true

                    rows: 2
                    flow: GridLayout.TopToBottom

                    Text {
                        Layout.preferredWidth: tab.width / 5

                        text: qsTranslate("RDM","Redis Version")
                        font.pointSize: 12
                        color: "grey"
                    }

                    BetterLabel {
                        id: redisVersionLabel
                        text: "N/A"
                        font.pointSize: 12
                        objectName: "rdm_server_info_redis_version"
                    }

                    Text {
                        Layout.preferredWidth: tab.width / 5

                        text: qsTranslate("RDM","Used memory")
                        font.pointSize: 12
                        color: "grey"
                    }

                    BetterLabel {
                        id: usedMemoryLabel;
                        text: "N/A";
                        font.pointSize: 12
                        objectName: "rdm_server_info_used_memory"
                    }

                    Text {
                        Layout.preferredWidth: tab.width / 5

                        text: qsTranslate("RDM","Clients")
                        font.pointSize: 12
                        color: "grey"
                    }

                    BetterLabel {
                        id: connectedClientsLabel;
                        text: "N/A";
                        font.pointSize: 12
                        objectName: "rdm_server_info_clients"
                    }

                    Text {
                        Layout.preferredWidth: tab.width / 5

                        text: qsTranslate("RDM","Commands Processed")
                        font.pointSize: 12
                        color: "grey"
                        wrapMode: Text.WordWrap
                    }

                    BetterLabel {
                        id: totalCommandsProcessedLabel;
                        text: "N/A";
                        font.pointSize: 12
                        objectName: "rdm_server_info_cmd_processed"
                    }


                    Text {
                        Layout.preferredWidth: tab.width / 5

                        text: qsTranslate("RDM","Uptime")
                        font.pointSize: 12
                        color: "grey"
                    }

                    BetterLabel {
                        id: uptimeLabel;
                        text: "N/A";
                        font.pointSize: 12
                        objectName: "rdm_server_info_uptime"
                    }

                    Connections {
                        target: tab.model? tab.model : null

                        onServerInfoChanged: {
                            usedMemoryLabel.text = getValue("memory", "used_memory_human")
                            redisVersionLabel.text = getValue("server", "redis_version")
                            connectedClientsLabel.text = getValue("clients", "connected_clients")
                            totalCommandsProcessedLabel.text = getValue("stats", "total_commands_processed")
                            uptimeLabel.text = getValue("server", "uptime_in_days") + qsTranslate("RDM"," day(s)")
                        }

                        function getValue(cat, prop) {
                            try {
                                return tab.model.serverInfo[cat][prop]
                            } catch(e) {
                                console.error("Cannot get server info '" + prop + "' from " + cat)
                                return ""
                            }
                        }
                    }
                }

                TabBar {
                    id: serverInfoTabBar
                    Layout.fillWidth: true
                    Layout.preferredHeight: 30

                    TabButton {
                        text: qsTranslate("RDM","Memory Usage")
                    }

                    TabButton {
                        text: qsTranslate("RDM","Server Info")
                    }

                    TabButton {
                        text: qsTranslate("RDM","Slowlog")
                    }

                    TabButton {
                        text: qsTranslate("RDM","Clients")
                    }

                    TabButton {
                        objectName: "rdm_server_info_tab_btn_pub_sub_channels"
                        text: qsTranslate("RDM","Pub/Sub Channels")
                    }
                }

                StackLayout {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    Layout.rightMargin: 50

                    currentIndex: serverInfoTabBar.currentIndex

                    ChartView {
                        objectName: "rdm_server_info_tab_memory_usage"
                        id: view

                        theme: {
                            if (sysPalette.base.hslLightness < 0.4) {
                                return ChartView.ChartThemeDark
                            } else {
                                return ChartView.ChartThemeLight
                            }
                        }

                        backgroundColor: sysPalette.base

                        title: qsTranslate("RDM","Memory Usage")
                        antialiasing: true

                        DateTimeAxis {
                            id: axisX
                            min: new Date()
                            format: "HH:mm:ss"
                        }

                        ValueAxis {
                            id: axisY
                            min: 0
                            titleText: qsTranslate("RDM","Mb")
                        }

                        function toMsecsSinceEpoch(date) {
                            var msecs = date.getTime();
                            return msecs;
                        }

                        SplineSeries {
                            id: used_memory_series
                            name: "used_memory"
                            axisX: axisX
                            axisY: axisY
                        }

                        SplineSeries {
                            id: used_memory_rss_series
                            name: "used_memory_rss"
                            axisX: axisX
                            axisY: axisY
                        }

                        SplineSeries {
                            id: used_memory_lua_series
                            name: "used_memory_lua"
                            axisX: axisX
                            axisY: axisY
                        }

                        SplineSeries {
                            id: used_memory_peak_series
                            name: "used_memory_peak"
                            axisX: axisX
                            axisY: axisY
                        }

                        Connections {
                            target: tab.model? tab.model : null

                            onServerInfoChanged: {
                                if (uiBlocker.visible) {
                                    uiBlocker.visible = false
                                }

                                var getValue = function (name) {
                                    return Math.round(parseFloat(tab.model.serverInfo["memory"][name] ) / (1024 * 1024)  * 100) / 100;
                                }

                                qmlUtils.addNewValueToDynamicChart(used_memory_series, getValue("used_memory"))
                                qmlUtils.addNewValueToDynamicChart(used_memory_rss_series, getValue("used_memory_rss"))
                                qmlUtils.addNewValueToDynamicChart(used_memory_lua_series, getValue("used_memory_lua"))
                                qmlUtils.addNewValueToDynamicChart(used_memory_peak_series, getValue("used_memory_peak"))

                                axisY.max = Math.max(getValue("used_memory_peak"),
                                                     getValue("used_memory"),
                                                     getValue("used_memory_rss"),
                                                     getValue("used_memory_lua")) + 2;
                            }
                        }
                    }


                    Item {

                        ColumnLayout {

                            anchors.fill: parent
                            anchors.margins: 10

                            BoolOption {
                                id: autorefreshSwitch

                                Layout.preferredWidth: 200
                                Layout.preferredHeight: 40

                                value: true
                                label: qsTranslate("RDM","Auto Refresh")
                            }

                            TabBar {
                                id: serverInfoDetailsTabBar
                                Layout.fillWidth: true
                                Layout.preferredHeight: 30

                                currentIndex: 0

                                Repeater {
                                    id: serverInfoBuilderTabButtons
                                    TabButton {
                                        text: modelData['name']
                                    }
                                }
                            }

                            StackLayout {
                                id: serverInfoTabs

                                Layout.fillWidth: true
                                Layout.fillHeight: true
                                currentIndex: serverInfoDetailsTabBar.currentIndex

                                Repeater {
                                    id: serverInfoBuilder

                                    LC.TableView {

                                        model: modelData['section_data']

                                        LC.TableViewColumn {
                                            role: "name"
                                            title: qsTranslate("RDM","Property")
                                            width: 250
                                        }

                                        LC.TableViewColumn {
                                            role: "value"
                                            title: qsTranslate("RDM","Value")
                                            width: 350
                                        }
                                    }

                                }

                                Connections {
                                    target: tab.model? tab.model : null

                                    onServerInfoChanged: {
                                        if (autorefreshSwitch.value === false)
                                            return;

                                        loadServerInfo();
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

                        Rectangle {
                            id: serverInfoUIBlocker
                            visible: !serverInfoBuilder.model
                            anchors.fill: parent
                            color: Qt.rgba(0, 0, 0, 0.1)

                            Item {
                                anchors.fill: parent
                                BusyIndicator { anchors.centerIn: parent; running: true }
                            }

                            MouseArea {
                                anchors.fill: parent
                            }
                        }
                    }

                    Item {

                        ColumnLayout {

                            anchors.fill: parent
                            anchors.margins: 10

                            BoolOption {
                                Layout.preferredWidth: 200
                                Layout.preferredHeight: 40

                                value: true
                                label: qsTranslate("RDM","Auto Refresh")

                                onValueChanged: {
                                    tab.model.refreshSlowLog = value
                                }
                            }

                            LC.TableView {
                                Layout.fillHeight: true
                                Layout.fillWidth: true

                                model: tab.model.slowLog ? tab.model.slowLog : []

                                LC.TableViewColumn {
                                    role: "cmd"
                                    title: qsTranslate("RDM","Command")
                                    width: 600

                                    delegate: Text {
                                        text: {
                                            var result = "";
                                            for (var index in modelData['cmd']) {
                                                result += modelData['cmd'][index] + " ";
                                            }
                                            return result;
                                        }
                                        elide: styleData.elideMode
                                    }
                                }

                                LC.TableViewColumn {
                                    role: "time"
                                    title: qsTranslate("RDM","Processed at")
                                    width: 150
                                }

                                LC.TableViewColumn {
                                    role: "exec_time"
                                    title: qsTranslate("RDM","Execution Time (μs)")
                                    width: 150
                                }
                            }
                        }
                    }

                    Item {

                        ColumnLayout {

                            anchors.fill: parent
                            anchors.margins: 10

                            BoolOption {
                                Layout.preferredWidth: 200
                                Layout.preferredHeight: 40

                                value: true
                                label: qsTranslate("RDM","Auto Refresh")

                                onValueChanged: {
                                    tab.model.refreshClients = value
                                }
                            }

                            LC.TableView {
                                Layout.fillHeight: true
                                Layout.fillWidth: true

                                model: tab.model.clients ? tab.model.clients : []

                                LC.TableViewColumn {
                                    role: "addr"
                                    title: qsTranslate("RDM","Client Address")
                                    width: 200
                                }

                                LC.TableViewColumn {
                                    role: "age"
                                    title: qsTranslate("RDM","Age (sec)")
                                    width: 75
                                }

                                LC.TableViewColumn {
                                    role: "idle"
                                    title: qsTranslate("RDM","Idle")
                                    width: 75
                                }

                                LC.TableViewColumn {
                                    role: "flags"
                                    title: qsTranslate("RDM","Flags")
                                    width: 75
                                }

                                LC.TableViewColumn {
                                    role: "db"
                                    title: qsTranslate("RDM","Current Database")
                                    width: 120
                                }
                            }
                        }
                    }

                    Item {

                        ColumnLayout {

                            anchors.fill: parent
                            anchors.margins: 10

                            BoolOption {
                                Layout.preferredWidth: 200
                                Layout.preferredHeight: 40

                                value: true
                                label: qsTranslate("RDM","Enable")

                                onValueChanged: {
                                    tab.model.refreshPubSubMonitor = value
                                }
                            }

                            LC.TableView {
                                Layout.fillHeight: true
                                Layout.fillWidth: true

                                model: tab.model.pubSubChannels ? tab.model.pubSubChannels : []                                

                                rowDelegate: Item {
                                    height: 50
                                }

                                LC.TableViewColumn {
                                    role: "addr"
                                    title: qsTranslate("RDM","Channel Name")
                                    width: 200
                                }

                                LC.TableViewColumn {
                                    role: "addr"
                                    width: 200
                                    delegate: Item {
                                        BetterButton {
                                            objectName: "rdm_server_info_pub_sub_subscribe_to_channel_btn"
                                            anchors.centerIn: parent
                                            text: qsTranslate("RDM","Subscribe in Console")
                                            onClicked: {
                                                console.log(styleData.value)
                                                tab.model.subscribeToChannel(styleData.value)
                                            }
                                        }
                                    }
                                }

                            }
                        }
                    }
                }
            }

            Component.onCompleted: {
                uiBlocker.visible = true
            }

            Rectangle {
                id: uiBlocker
                visible: false
                anchors.fill: parent
                color: Qt.rgba(0, 0, 0, 0.1)

                Item {
                    anchors.fill: parent
                    BusyIndicator { anchors.centerIn: parent; running: true }
                }

                MouseArea {
                    anchors.fill: parent
                }
            }
        }
    }
}
