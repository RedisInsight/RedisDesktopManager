import QtQuick 2.3
import QtQuick.Layouts 1.13
import QtQuick.Controls 2.13
import QtQuick.Controls 1.4 as LC
import QtQuick.Dialogs 1.2
import QtQuick.Window 2.2
import QtCharts 2.3
import "./../common"
import "./../common/platformutils.js" as PlatformUtils
import "./../settings"


Repeater {
    id: root

    BetterTab {
        id: tab

        property int colWidth: tab.width / 7

        function getValue(cat, prop) {
            try {
                return tab.model.serverInfo[cat][prop]
            } catch(e) {
                console.error("Cannot get server info '" + prop + "' from " + cat)
                return ""
            }
        }

        function getIntValue(cat, prop) {
            var val = getValue(cat, prop)
            if (val !== "") return parseInt(val)
            return 0
        }

        function getHitRatio() {
            var hits = getIntValue("stats", "keyspace_hits")
            var misses = getIntValue("stats", "keyspace_misses")
            var total = hits + misses
            if (total === 0) {
                return 0
            }
            return hits / total * 100
        }

        function getTotalKeysValue() {
            var total = 0;

            for (var key in tab.model.serverInfo["keyspace"]) {
                var line = tab.model.serverInfo["keyspace"][key]
                var parts = line.split(/[\:\=\,]/);
                var count = parseInt(parts[1])
                total += count
            }
            return total
        }

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
                        Layout.preferredWidth: tab.colWidth

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
                        Layout.preferredWidth: tab.colWidth

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
                        Layout.preferredWidth: tab.colWidth

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
                        Layout.preferredWidth: tab.colWidth

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
                        Layout.preferredWidth: tab.colWidth

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

                    Text {
                        Layout.preferredWidth: tab.colWidth

                        text: qsTranslate("RDM","Total Keys")
                        font.pointSize: 12
                        color: "grey"
                    }

                    BetterLabel {
                        id: totalKeysLabel;
                        text: "N/A";
                        font.pointSize: 12
                        objectName: "rdm_server_info_total_keys"
                    }

                    Text {
                        Layout.preferredWidth: tab.colWidth

                        text: qsTranslate("RDM","Hit Ratio")
                        font.pointSize: 12
                        color: "grey"
                    }

                    BetterLabel {
                        id: hitRatioLabel;
                        text: "N/A";
                        font.pointSize: 12
                        objectName: "rdm_server_info_hit_ratio"
                    }

                    Connections {
                        target: tab.model? tab.model : null

                        function onServerInfoChanged() {
                            usedMemoryLabel.text = tab.getValue("memory", "used_memory_human")
                            redisVersionLabel.text = tab.getValue("server", "redis_version")
                            connectedClientsLabel.text = tab.getValue("clients", "connected_clients")
                            totalCommandsProcessedLabel.text = tab.getValue("stats", "total_commands_processed")
                            uptimeLabel.text = tab.getValue("server", "uptime_in_days") + qsTranslate("RDM"," day(s)")
                            totalKeysLabel.text = tab.getTotalKeysValue()
                            hitRatioLabel.text = tab.getHitRatio() + "%"
                        }

                    }
                }

                TabBar {
                    id: serverInfoTabBar
                    Layout.fillWidth: true
                    Layout.preferredHeight: 30

                    TabButton {
                        text: qsTranslate("RDM","Info")
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

                    Item {
                        id: infoCharts

                        GridLayout {
                            id: infoChartsGrid
                            columns: 3
                            columnSpacing: 0
                            rowSpacing: 0

                            Layout.fillWidth: true
                            Layout.fillHeight: true

                            property int chartWidth: tab.width / 3
                            property int chartHeight: (tab.height - 60) / 2

                            function chartTheme() {
                                if (sysPalette.base.hslLightness < 0.4) {
                                    return ChartView.ChartThemeDark
                                } else {
                                    return ChartView.ChartThemeLight
                                }
                            }

                            ChartView {
                                id: chartCommandsPerSec

                                Layout.preferredWidth: parent.chartWidth
                                Layout.preferredHeight: parent.chartHeight

                                legend.visible: false
                                backgroundRoundness: 0

                                theme: parent.chartTheme()

                                backgroundColor: sysPalette.base

                                title: qsTranslate("RDM","Commands Per Second")
                                antialiasing: true

                                DateTimeAxis {
                                    id: axisXCommandsPerSec
                                    min: new Date()
                                    format: "HH:mm:ss"
                                }

                                ValueAxis {
                                    id: axisYCommandsPerSec
                                    min: 0
                                    max: 100
                                    labelFormat: "%d"
                                    titleText: qsTranslate("RDM","Ops/s")
                                }

                                LineSeries {
                                    id: commands_per_sec_series
                                    name: "commands_per_sec"
                                    axisX: axisXCommandsPerSec
                                    axisY: axisYCommandsPerSec
                                }
                            }

                            ChartView {
                                id: chartConnectedClients

                                Layout.preferredWidth: parent.chartWidth
                                Layout.preferredHeight: parent.chartHeight

                                legend.visible: false
                                backgroundRoundness: 0

                                theme: parent.chartTheme()

                                backgroundColor: sysPalette.base

                                title: qsTranslate("RDM","Connected Clients")
                                antialiasing: true

                                DateTimeAxis {
                                    id: axisXConnectedClients
                                    min: new Date()
                                    format: "HH:mm:ss"
                                }

                                ValueAxis {
                                    id: axisYConnectedClients
                                    min: 0
                                    max: 100
                                    labelFormat: "%d"
                                    titleText: qsTranslate("RDM","Clients")
                                }

                                SplineSeries {
                                    id: connected_clients_series
                                    name: "connected_clients"
                                    axisX: axisXConnectedClients
                                    axisY: axisYConnectedClients
                                }
                            }

                            ChartView {
                                id: chartMemoryUsage
                                objectName: "rdm_server_info_tab_memory_usage"

                                Layout.preferredWidth: parent.chartWidth
                                Layout.preferredHeight: parent.chartHeight

                                legend.visible: false
                                backgroundRoundness: 0

                                theme: parent.chartTheme()
                                backgroundColor: sysPalette.base

                                title: qsTranslate("RDM","Memory Usage")
                                antialiasing: true

                                DateTimeAxis {
                                    id: axisXMemoryUsage
                                    min: new Date()
                                    format: "HH:mm:ss"
                                }

                                ValueAxis {
                                    id: axisYMemoryUsage
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
                                    axisX: axisXMemoryUsage
                                    axisY: axisYMemoryUsage
                                }
                            }

                            ChartView {
                                id: chartNetworkInput

                                Layout.preferredWidth: parent.chartWidth
                                Layout.preferredHeight: parent.chartHeight

                                legend.visible: false
                                backgroundRoundness: 0

                                theme: parent.chartTheme()

                                backgroundColor: sysPalette.base

                                title: qsTranslate("RDM","Network Input")
                                antialiasing: true

                                DateTimeAxis {
                                    id: axisXNetworkInput
                                    min: new Date()
                                    format: "HH:mm:ss"
                                }

                                ValueAxis {
                                    id: axisYNetworkInput
                                    min: 0
                                    titleText: qsTranslate("RDM","Kb/s")
                                }

                                LineSeries {
                                    id: network_input_series
                                    name: "network_input"
                                    axisX: axisXNetworkInput
                                    axisY: axisYNetworkInput
                                }
                            }

                            ChartView {
                                id: chartNetworkOutput

                                Layout.preferredWidth: parent.chartWidth
                                Layout.preferredHeight: parent.chartHeight

                                legend.visible: false
                                backgroundRoundness: 0

                                theme: parent.chartTheme()

                                backgroundColor: sysPalette.base

                                title: qsTranslate("RDM","Network Output")
                                antialiasing: true

                                DateTimeAxis {
                                    id: axisXNetworkOutput
                                    min: new Date()
                                    format: "HH:mm:ss"
                                }

                                ValueAxis {
                                    id: axisYNetworkOutput
                                    min: 0
                                    titleText: qsTranslate("RDM","Kb/s")
                                }

                                LineSeries {
                                    id: network_output_series
                                    name: "network_output"
                                    axisX: axisXNetworkOutput
                                    axisY: axisYNetworkOutput
                                }
                            }

                            ChartView {
                                id: chartTotalKeys

                                Layout.preferredWidth: parent.chartWidth
                                Layout.preferredHeight: parent.chartHeight

                                legend.visible: false
                                backgroundRoundness: 0

                                theme: parent.chartTheme()

                                backgroundColor: sysPalette.base

                                title: qsTranslate("RDM","Total Keys")
                                antialiasing: true

                                DateTimeAxis {
                                    id: axisXTotalKeys
                                    min: new Date()
                                    format: "HH:mm:ss"
                                }

                                ValueAxis {
                                    id: axisYTotalKeys
                                    min: 0
                                    max: 100
                                    labelFormat: "%d"
                                    titleText: qsTranslate("RDM","Keys")
                                }

                                LineSeries {
                                    id: total_keys_series
                                    name: "total_keys"
                                    axisX: axisXTotalKeys
                                    axisY: axisYTotalKeys
                                }
                            }
                        }

                        Connections {
                            target: tab.model? tab.model : null

                            onServerInfoChanged: {
                                if (uiBlocker.visible) {
                                    uiBlocker.visible = false
                                }

                                var getUsedMemory = function (name) {
                                    return Math.round(parseFloat(tab.model.serverInfo["memory"][name] ) / (1024 * 1024)  * 100) / 100;
                                }

                                // Commands per second
                                var commandsPerSec = parseInt(tab.getValue("stats", "instantaneous_ops_per_sec"))
                                var commandsPerSecMax = commandsPerSec + (10 - commandsPerSec % 10)
                                if (commandsPerSecMax > axisYCommandsPerSec.max)
                                    axisYCommandsPerSec.max = commandsPerSecMax
                                qmlUtils.addNewValueToDynamicChart(commands_per_sec_series, commandsPerSec)

                                // Connected clients
                                var connectedClients = parseInt(tab.getValue("clients", "connected_clients"))
                                var connectedClientsMax = connectedClients + (10 - connectedClients % 10)
                                if (connectedClientsMax > axisYConnectedClients.max)
                                    axisYConnectedClients.max = connectedClientsMax
                                qmlUtils.addNewValueToDynamicChart(connected_clients_series, connectedClients)

                                // Memory usage
                                var usedMemory = getUsedMemory("used_memory")
                                var memoryUsageMax = getUsedMemory("used_memory") + (10 - usedMemory % 10)
                                if (memoryUsageMax > axisYMemoryUsage.max)
                                    axisYMemoryUsage.max = memoryUsageMax
                                qmlUtils.addNewValueToDynamicChart(used_memory_series, usedMemory)

                                // Network input
                                var networkInput = parseFloat(tab.getValue("stats", "instantaneous_input_kbps"))
                                var networkInputMax = networkInput + (10 - networkInput % 10)
                                if (networkInputMax > axisYNetworkInput.max)
                                    axisYNetworkInput.max = networkInputMax
                                qmlUtils.addNewValueToDynamicChart(network_input_series, networkInput)

                                // Network output
                                var networkOutput = parseFloat(tab.getValue("stats", "instantaneous_output_kbps"))
                                var networkOutputMax = networkOutput + (10 - networkOutput % 10)
                                if (networkOutputMax > axisYNetworkOutput.max)
                                    axisYNetworkOutput.max = networkOutputMax
                                qmlUtils.addNewValueToDynamicChart(network_output_series, networkOutput)

                                // Total keys
                                var totalKeys = parseInt(tab.getTotalKeysValue())
                                var totalKeysMax = totalKeys + (10 - totalKeys % 10)
                                if (totalKeysMax > axisYTotalKeys.max)
                                    axisYTotalKeys.max = totalKeysMax
                                qmlUtils.addNewValueToDynamicChart(total_keys_series, totalKeys)
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

                                    delegate: Text {
                                        text: {
                                            return new Date(modelData['time']*1000).toLocaleString(
                                                        locale, PlatformUtils.dateTimeFormat);
                                        }
                                        elide: styleData.elideMode
                                    }

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
