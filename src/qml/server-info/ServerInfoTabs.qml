import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.1
import QtQuick.Dialogs 1.2
import QtQuick.Window 2.2
import QtCharts 2.0
import "./../common"
import "./../settings"


Repeater {
    id: root

    BetterTab {
        id: tab
        title: tabName
        icon: "qrc:/images/console.svg"
        tabType: "server_info"

        property var model: serverStatsModel.getValue(tabIndex)

        onModelChanged: {
            if (!model)
                return;

            tab.model.init()
        }

        onClose: {
            serverStatsModel.closeTab(tabIndex)
        }

        Rectangle {
            anchors.fill: parent

            color: "white"

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

                    Label {
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

                    Label {
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

                    Label {
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

                    Label {
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

                    Label {
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

                TabView {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    Layout.rightMargin: 50

                    Tab {
                        title: qsTranslate("RDM","Memory Usage")

                        ChartView {
                            id: view

                            anchors.fill: parent

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
                                        return parseFloat(tab.model.serverInfo["memory"][name] ) / (1024 * 1024)
                                    }

                                    qmlUtils.addNewValueToDynamicChart(used_memory_series, getValue("used_memory"))
                                    qmlUtils.addNewValueToDynamicChart(used_memory_rss_series, getValue("used_memory_rss"))
                                    qmlUtils.addNewValueToDynamicChart(used_memory_lua_series, getValue("used_memory_lua"))
                                    qmlUtils.addNewValueToDynamicChart(used_memory_peak_series, getValue("used_memory_peak"))

                                    axisY.max = getValue("used_memory_peak") + 1
                                }
                            }
                        }
                    }

                    Tab {
                        title: qsTranslate("RDM","Server Info")

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

                            TabView {
                                id: serverInfoTabs

                                Layout.fillWidth: true
                                Layout.fillHeight: true

                                Repeater {
                                    id: serverInfoBuilder

                                    Tab {
                                        anchors.fill: parent
                                        title: modelData["name"]

                                        TableView {
                                            anchors.fill: parent

                                            model: modelData["section_data"]

                                            TableViewColumn {
                                                role: "name"
                                                title: qsTranslate("RDM","Property")
                                                width: 250
                                            }

                                            TableViewColumn {
                                                role: "value"
                                                title: qsTranslate("RDM","Value")
                                                width: 350
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
                                        serverInfoTabs.currentIndex = currentTab
                                    }
                                }
                            }
                        }
                    }

                    Tab {
                        title: qsTranslate("RDM","Slowlog")

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

                            TableView {
                                Layout.fillHeight: true
                                Layout.fillWidth: true

                                model: tab.model.slowLog ? tab.model.slowLog : []

                                TableViewColumn {
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

                                TableViewColumn {
                                    role: "time"
                                    title: qsTranslate("RDM","Processed at")
                                    width: 150
                                }

                                TableViewColumn {
                                    role: "exec_time"
                                    title: qsTranslate("RDM","Execution Time (μs)")
                                    width: 150
                                }
                            }
                        }
                    }

                    Tab {
                        title: qsTranslate("RDM","Clients")

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

                            TableView {
                                Layout.fillHeight: true
                                Layout.fillWidth: true

                                model: tab.model.clients ? tab.model.clients : []

                                TableViewColumn {
                                    role: "addr"
                                    title: qsTranslate("RDM","Client Address")
                                    width: 200
                                }

                                TableViewColumn {
                                    role: "age"
                                    title: qsTranslate("RDM","Age (sec)")
                                    width: 75
                                }

                                TableViewColumn {
                                    role: "idle"
                                    title: qsTranslate("RDM","Idle")
                                    width: 75
                                }

                                TableViewColumn {
                                    role: "flags"
                                    title: qsTranslate("RDM","Flags")
                                    width: 75
                                }

                                TableViewColumn {
                                    role: "db"
                                    title: qsTranslate("RDM","Current Database")
                                    width: 120
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
