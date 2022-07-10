import QtQuick 2.3
import QtQuick.Layouts 1.13
import QtQuick.Controls 2.13
import QtQuick.Controls 1.4 as LC
import QtQuick.Window 2.2
import QtCharts 2.3
import "./../common"
import "./../common/platformutils.js" as PlatformUtils
import "./../settings"


Repeater {
    id: root

    BetterTab {
        id: serverTab

        Component {
            id: serverTabButton

            BetterTabButton {
                icon.source: PlatformUtils.getThemeIcon("database.svg")

                text: tabName

                onCloseClicked: {
                    serverStatsModel.closeTab(tabIndex)
                }
            }
        }

        Component.onCompleted: {
            var tabButton = serverTabButton.createObject(serverTab);
            tabButton.self = tabButton;
            tabButton.tabRef = serverTab;
            tabBar.addItem(tabButton)
            tabBar.activateTabButton(tabButton)
            tabs.activateTab(serverTab)
        }

        property var model: tabModel

        onModelChanged: {
            if (!model)
                return;

            serverTab.model.init()
        }


        function getValue(cat, prop) {
            try {
                return model.serverInfo[cat][prop]
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
            return Math.round(hits / total * 100 * 100) / 100
        }

        function getTotalKeysValue() {
            var total = 0;

            for (var key in model.serverInfo["keyspace"]) {
                var line = model.serverInfo["keyspace"][key]
                var parts = line.split(/[\:\=\,]/);
                var count = parseInt(parts[1])
                total += count
            }
            return total
        }

        Rectangle {
            id: wrappingBackground
            anchors.fill: parent

            color: sysPalette.base

            ColumnLayout {                
                clip: true
                anchors.fill: parent
                anchors.margins: 15

                Component {
                    id: actionsMenu

                    ColumnLayout {

                        SettingsGroupTitle {
                            Layout.fillWidth: true
                            Layout.topMargin: 20
                            text: qsTranslate("RESP", "Actions")
                        }

                        GridLayout {
                            id: tileGrid
                            columns: 5

                            Layout.fillWidth: true

                            property int tileSize: 90
                            property int tileIconSize: 64

                            ImageButton {
                                Layout.fillWidth: true
                                implicitHeight: tileGrid.tileSize

                                text: qsTranslate("RESP", "Server Stats")

                                showBorder: true

                                imgWidth: tileGrid.tileIconSize
                                imgHeight: tileGrid.tileIconSize
                                iconSource: PlatformUtils.getThemeIcon("server-stats.svg")
                                onClicked: {
                                    currentAction.text = text
                                    serverStackView.push(serverCharts)
                                }
                            }

                            ImageButton {
                                Layout.fillWidth: true
                                implicitHeight: tileGrid.tileSize

                                text: qsTranslate("RESP", "Console")

                                showBorder: true

                                imgWidth: tileGrid.tileIconSize
                                imgHeight: tileGrid.tileIconSize
                                iconSource: PlatformUtils.getThemeIcon("console.svg")
                                onClicked: {
                                    serverTab.model.openTerminal()
                                }
                            }

                            ImageButton {
                                Layout.fillWidth: true
                                implicitHeight: tileGrid.tileSize

                                text: qsTranslate("RESP", "Monitor Commands")

                                showBorder: true

                                imgWidth: tileGrid.tileIconSize
                                imgHeight: tileGrid.tileIconSize
                                iconSource: PlatformUtils.getThemeIcon("console.svg")
                                onClicked: {
                                    serverTab.model.monitorCommands()
                                }
                            }

                            ImageButton {
                                Layout.fillWidth: true
                                implicitHeight: tileGrid.tileSize

                                text: qsTranslate("RESP", "Slowlog")

                                showBorder: true

                                imgWidth: tileGrid.tileIconSize
                                imgHeight: tileGrid.tileIconSize
                                iconSource: PlatformUtils.getThemeIcon("slowlog.svg")
                                onClicked: {
                                    currentAction.text = text
                                    serverTab.model.refreshSlowLog = true
                                    serverStackView.push(serverSlowlog)
                                }
                            }

                            ImageButton {
                                Layout.fillWidth: true
                                implicitHeight: tileGrid.tileSize

                                text: qsTranslate("RESP", "Clients")

                                showBorder: true

                                imgWidth: tileGrid.tileIconSize
                                imgHeight: tileGrid.tileIconSize
                                iconSource: PlatformUtils.getThemeIcon("clients.svg")
                                onClicked: {
                                    currentAction.text = text
                                    serverTab.model.refreshClients = true
                                    serverStackView.push(serverClients)
                                }
                            }

                            ImageButton {
                                Layout.fillWidth: true
                                implicitHeight: tileGrid.tileSize

                                text: qsTranslate("RESP", "Pub/Sub Channels")

                                showBorder: true

                                imgWidth: tileGrid.tileIconSize
                                imgHeight: tileGrid.tileIconSize
                                iconSource: PlatformUtils.getThemeIcon("pub-sub-channels.svg")
                                onClicked: {
                                    currentAction.text = text
                                    serverTab.model.refreshPubSubMonitor = true
                                    serverStackView.push(serverPubSub)
                                }
                            }

                            ImageButton {
                                Layout.fillWidth: true
                                implicitHeight: tileGrid.tileSize

                                text: qsTranslate("RESP", "Server Config")

                                showBorder: true

                                imgWidth: tileGrid.tileIconSize
                                imgHeight: tileGrid.tileIconSize
                                iconSource: PlatformUtils.getThemeIcon("server-config.svg")
                                onClicked: {
                                    currentAction.text = text
                                    serverStackView.push(serverConfig)
                                }
                            }
                        }                        

                        Item {
                            Layout.fillHeight: true
                        }
                    }
                }

                Component {
                    id: serverCharts

                    ServerCharts {
                        model: serverTab.model
                    }
                }

                Component {
                    id: serverClients

                    ServerClients {
                        model: serverTab.model
                    }
                }

                Component {
                    id: serverConfig

                    ServerConfig {
                        model: serverTab.model
                    }
                }

                Component {
                    id: serverPubSub

                    ServerPubSub {
                        model: serverTab.model
                    }
                }

                Component {
                    id: serverSlowlog

                    ServerSlowlog {
                        model: serverTab.model
                    }
                }

                GridLayout {
                    id: serverHighlights
                    Layout.fillWidth: true
                    Layout.columnSpan: 3

                    rows: 2
                    flow: GridLayout.TopToBottom

                    property int colWidth: wrappingBackground.width / 8

                    Text {
                        Layout.preferredWidth: serverHighlights.colWidth

                        text: qsTranslate("RESP","Redis Version")
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
                        Layout.preferredWidth: serverHighlights.colWidth

                        text: qsTranslate("RESP","Used memory")
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
                        Layout.preferredWidth: serverHighlights.colWidth

                        text: qsTranslate("RESP","Clients")
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
                        Layout.preferredWidth: serverHighlights.colWidth

                        text: qsTranslate("RESP","Commands Processed")
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
                        Layout.preferredWidth: serverHighlights.colWidth

                        text: qsTranslate("RESP","Uptime")
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
                        Layout.preferredWidth: serverHighlights.colWidth

                        text: qsTranslate("RESP","Total Keys")
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
                        Layout.preferredWidth: serverHighlights.colWidth

                        text: qsTranslate("RESP","Hit Ratio")
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
                        target: model? model : null

                        function onServerInfoChanged() {
                            usedMemoryLabel.text = serverTab.getValue("memory", "used_memory_human")
                            redisVersionLabel.text = serverTab.getValue("server", "redis_version")
                            connectedClientsLabel.text = serverTab.getValue("clients", "connected_clients")
                            totalCommandsProcessedLabel.text = serverTab.getValue("stats", "total_commands_processed")
                            uptimeLabel.text = serverTab.getValue("server", "uptime_in_days") + qsTranslate("RESP"," day(s)")
                            totalKeysLabel.text = serverTab.getTotalKeysValue()
                            hitRatioLabel.text = serverTab.getHitRatio() + "%"
                        }
                    }
                }

                RowLayout {
                    visible: serverStackView.depth > 1

                    BetterButton {
                        text: qsTr("Server Actions")

                        onClicked: {
                            serverStackView.currentItem.stopTimer()
                            serverStackView.pop()
                        }
                    }

                    BetterLabel { text: "❯" }

                    BetterLabel {id: currentAction}

                }

                StackView {
                    id: serverStackView
                    Layout.fillHeight: true
                    Layout.fillWidth: true

                    initialItem: actionsMenu
                }
            }
        }
    }
}
